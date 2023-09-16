/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/18 18:00:34 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/16 00:42:31 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"
#include "../inc/Response.hpp"
#include "../inc/Request.hpp"
#include "../inc/HttpGet.hpp"

Response::Response() : _headers(), _methodsFunctions(){
    _chosenSocket = NULL;
	_methodsFunctions["GET"] = &Response::httpGet;
	_methodsFunctions["POST"] = &Response::postMethod;
	_methodsFunctions["DELETE"] = &Response::deleteMethod;
    _body = "";
    _response = ""; //criar uma string response que, será todo esse cabeçalho + body (ver exemplos no chat)
    _code = "";
    _path = "";
    _statusMessages = StatusMessages();
}

Response::~Response()
{}

Response::Response(Request request)
{
    //define corpo da resposta lendo o conteudo do arquivo html
    // _body = readHtmlFile("web/html_upload_arquivo/index.html");
    
    // precisa definir os campos do cabeçalho aqui, entao se for inserir mais É AQUI
    //setStatusCode("200 OK");
    //setContentType("text/html");
    //setDateAndTime();
    //setContentLength(_body.length());
    _chosenSocket = NULL;
}

// métodos pra lidar com cada função respectiva
// Funções de exemplo
/*
If you don't have a specific location directive in your Nginx configuration and you try to POST an image or any other data to the server, Nginx will typically handle the request as follows:

Nginx will receive the POST request with the image data from the client.

It will process the request headers as usual.

Since there is no specific location block defined to handle the POST request, Nginx will not perform any custom processing on the request data.

Instead, it will look for a default behavior to handle the POST data, which often involves saving the POST data to a temporary file on the server's filesystem. The exact location and behavior may depend on your Nginx configuration, but Nginx will generally handle the data as a file upload.

Nginx will not perform any image processing or special handling of the uploaded image by default. It won't display or process the image unless you have a separate process or script in your server's backend that handles uploaded files.

In summary, without a specific location directive to handle POST requests, Nginx will treat the uploaded image as a file upload and save it to the server.
*/

std::string Response::postMethod(Request &request, SocketS &server, Response *this_response) {
    // return "Resposta para POST";

	// se for "POST", ele chama essa função... daí???

	// já terá escolhido o socket, é claro.. então...
	// tem que VER O LOCATION DO CGI, CERTO?
	// porque pra ter POST, vai ter que usar um script CGI
	// se nao tiver essa location, o que acontece????
	// ele envia uma resposta de erro????? 
	// ----> Não, o ngnix vai apenas POSTAR NO SERVER (diretorio) o conteudo,
	// ele só NÃO VAI FAZER NENHUM PROCESSAMENTO DESSE CONTEUDO

	// TO DO:
	// entao vemos se tem location pra decidir se: ---> é pra ser um location pro script CGI ( location /cgi-bin/)
	// -- ao receber um POST, vamos usar um script
	// -- ou, como nao tem location (logo, nao tem script pra chamar), APENAS SALVA O CONTEUDO

	// --> tambem temos que ver se tem "root" (seja no bloco de server ou do location)
	// --- pra saber em que diretorio buscar

	// Sobre se nao houver root em nenhum lugar do bloco server doarquivo:
	/*
		 Se nenhum root for especificado em qualquer lugar, o Nginx usará o diretório
		 raiz padrão do sistema como ponto de partida para atender as solicitações.
		 Isso é geralmente definido durante a compilação ou instalação do Nginx e pode
		 variar de acordo com o sistema operacional.
	*/

	/*
		ANTES DE TUDO:
		-> verificar o TAMANHO DO CONTEUDO DO POST (vê se está dentro dos limites de max-content
		determinado no arquivo de configuração. Se não estiver, corta na hora reornando uma
		RESPONSE DE ERRO DE ACORDO COM A SITUAÇÃO)
	*/


	std::string root_for_response; // essa é uma variavel temporaria daqui
	//if (server.getRoot().size() > 0)
	//	root_for_response = server.getRoot();
	// agora vamos ver se tem o location do CGI (se nao tiver, ir pro caminho "padrão" do server (vai ser apenas uma postagem no diretorio do server (faz um metodo pra isso?)))
	// /cgi-bin/
	root_for_response = "./";

	size_t found = request.getRequest().find("/process_data.cgi");
	if (found == std::string::npos)
	{
		std::cerr << "---- Opa, essa request NÃO tem process_data CGI !!! ----" << std::endl;
		std::string response_error =
        				"HTTP/1.1 404 Not Found\r\n"
        				"Server: nginx/1.20.0\r\n"
        				"Date: Sat, 03 Sep 2023 12:34:56 GMT\r\n"
        				"Content-Type: text/html\r\n"
        				"Content-Length: 162\r\n"
        				"Connection: keep-alive\r\n\r\n";
		return response_error;
	}
	std::map<std::string, LocationDirective> serverLocations = server.getLocations();
	std::map<std::string, LocationDirective>::iterator it = serverLocations.find("cgi-bin");
	if (it != serverLocations.end())
	{
		CGI script;
		// quer dizer que teve esse location
		std::cout << "Há um LOCATION PRO CGI!" << std::endl;
		// agora verificamos se há um "root" dentro desse location (um root mais "especifico", portanto)
		std::map<std::string, std::vector< std::string > > locationDirectives;
		locationDirectives = it->second.getDirectives();
        std::map<std::string, std::vector< std::string > >::iterator itLocationRoot = locationDirectives.find("root");
		if (itLocationRoot != locationDirectives.end())
		{
			// quer dizer que teve um root mais especifico no location... daí substituímos o root_for_response por essa..
			root_for_response = itLocationRoot->second[0];
			std::cout << "Root ESPECIFICO da location do CGI:::: " << root_for_response << std::endl;
			// tem que pegar outras duas diretivas dentro do it (ponteiro pra esse location):
			/*
				cgi_path --> location of interpreters installed on the current system, mandatory parameter
				cgi_ext --> extensions for executable files, mandatory parameter
			*/
			// passar aqui pra classe de ValidationsPost
			std::map<std::string, std::vector< std::string > >::iterator commandOfCGI = locationDirectives.find("cgi_path");
			if (commandOfCGI != locationDirectives.end())
			{
				// encontrou CGI-path...
				// tem que gravar isso numa variavel (pode ser um vetor, porque pode ter mais de um tipo de script CGI e, por
				// consequencia, mais de um comando pra executar cada um)
				std::vector<std::string> scriptsCommands = commandOfCGI->second;
				for (int i = 0; i < scriptsCommands.size() ; i++)
					std::cout << "Comandos para executar os scripts CGI ------> " << scriptsCommands[i] << std::endl;
				std::cout << "------------------------------------------------------" << std::endl;
				// bora encontrar a(s) extensão(ões)
				std::map<std::string, std::vector< std::string > >::iterator CGIExtension = locationDirectives.find("cgi_ext");
				if (CGIExtension != locationDirectives.end())
				{
					std::vector<std::string> scriptsExtensions = CGIExtension->second;
					for (int j = 0; j < scriptsExtensions.size(); j++)
						std::cout << "Extensões dods scripts CGI ------> " << scriptsExtensions[j] << std::endl;
					std::cout << "------------------------------------------------------" << std::endl;
					
					// setando todas essas variaveis no objeto CGI pra processar o resto A PARTIR DELE
					script.setRoot(root_for_response);
					script.setCommands(scriptsCommands);
					script.setExtensions(scriptsExtensions);
					// podia já declarar um objeto CGI e inserir essas informações (extensão e comando) dentro dele, certo?
					// e daí prosseguir a partir dele...
					// chamando seus métodos aqui pra executar e criar a resposta...
					// daí no final só armazenaria o que ele retorna na _response daqui....
					
					// NÃO SE ESQUEÇA DE IMPLEMENTAR O LOOP DE CHUNCKS TAMBEM

					script.handleCGIRequest(request);
					return script.getResponse(); // retorna a response
				}
				else
				{
					// se nao encontrar a diretiva 'cgi-ext'.. o que acontece? ele só grava sem nenhum tratamento ou dá resposta de erro?
					this_response->setDateAndTime();
					std::string date = this_response->getDate();
					std::string response_error =
        				"HTTP/1.1 404 Not Found\r\n"
        				"Server: webserv/1.0.0\r\n"
        				"Date: Sat, 03 Sep 2023 12:34:56 GMT\r\n"
        				"Content-Type: text/html\r\n"
        				"Content-Length: 162\r\n"
        				"Connection: keep-alive\r\n\r\n";
					return response_error;
				}
			}
			else
			{
				// se nao encontrar a diretiva 'cgi-path'.. o que acontece? ele só grava sem nenhum tratamento ou dá resposta de erro?
				std::string response_error =
        				"HTTP/1.1 404 Not Found\r\n"
        				"Server: nginx/1.20.0\r\n"
        				"Date: Sat, 03 Sep 2023 12:34:56 GMT\r\n"
        				"Content-Type: text/html\r\n"
        				"Content-Length: 162\r\n"
        				"Connection: keep-alive\r\n\r\n";
				return response_error;
			}
		}
	}
	else
	{
		//quer dizer que nao teve o locatin do cgi-bin, então lidamos com a postagem da forma "padrão" (grava o conteudo num arquivo no diretorio raiz do projeto)
		std::cout << "NÃO HÁ um location PRO CGI! Vai pro caminho padrão..." << std::endl;
		// ou seja, só "posta" o que tiver que postar num arquivo no diretório
	}
	std::string response_error =
        				"HTTP/1.1 505 Not Found\r\n"
        				"Server: nginx/1.20.0\r\n"
        				"Date: Sat, 03 Sep 2023 12:34:56 GMT\r\n"
        				"Content-Type: text/html\r\n"
        				"Content-Length: 162\r\n"
        				"Connection: keep-alive\r\n\r\n";
	return response_error;
}

std::string Response::deleteMethod(Request &request, SocketS &server, Response *this_response) {
    return "Resposta para DELETE";
}



//metodo para ler o conteudo do html
std::string Response::readHtmlFile(const std::string& filePath)
{
    std::ifstream file(filePath.c_str());
    if(!file.is_open())
        return""; //retorna string vazia por enquanto se o arquivo nao pode ser lido
    std::string content; //ler caractere a caractere e add a string content
    char character;
    while(file.get(character))
        content += character;
    file.close();
    return content;
}

void Response::setStatusCode(const std::string& statusCode){
    _headers["Status"] = statusCode;
}

void Response::setContentType(const std::string& contentType){
    _headers["Content-Type"] = contentType;
}

void Response::setDateAndTime(){
    char buffer[80];
    time_t rawTime;
    struct tm *timeInfo;
    
    time(&rawTime);
    timeInfo = localtime(&rawTime);
    
    //string info 
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", timeInfo);
    _headers["Date"] = buffer;
}

std::string Response::getDate(void)const{
	std::map<std::string, std::string >::const_iterator it = _headers.find("Date");
	if (it != _headers.end())
		return it->second;
	return "Date not found";
}

// void Response::setContentLength(size_t length)
// {
//     _headers["Content-Length"] = to_string(length); //***   contentLength(_response.length());
// }

void Response::setResponse(const std::string& response){
    _response = response;
}

const std::string& Response::getHeader(const std::string& header)const
{
    static std::string emptyHeader = ""; // Cria uma string vazia para retorno padrão

    // Implementação para buscar e retornar o cabeçalho específico
    // Se o cabeçalho não existir, retorne a string vazia criada acima

    return emptyHeader;
}

bool Response::contains(const std::vector<std::string>& vec , const std::string& content)
{
    for(size_t i = 0; i < vec.size(); ++i)
    {
        if(vec[i] == content)
            return true;
    }
    return false;
}

// void Response::setPath(const std::string& allPath){
//     _path = allPath;
// }


// refazer esse metodo selectServer




/*


Na configuração que você forneceu, onde existem dois servidores virtuais ouvindo em portas diferentes, mas com o mesmo nome de servidor, 
a ordem de verificação será a seguinte:

Verificação da Porta e Endereço IP: O Nginx primeiro verificará a combinação de porta e endereço IP da solicitação recebida. 
Ele tentará corresponder a combinação de porta e IP com a configuração dos servidores virtuais. No seu caso:

Se a solicitação for recebida em localhost (127.0.0.1) na porta 8034, ela será direcionada para o primeiro servidor virtual.
Se a solicitação for recebida no endereço IP 142.0.0.1 na porta 8033, ela será direcionada para o segundo servidor virtual.

Verificação do Campo "Host" (Nome do Servidor): Se a solicitação for direcionada para um servidor virtual com base na porta e no IP, 
o Nginx, em seguida, verificará o campo "Host" (nome do servidor) no cabeçalho da solicitação. Ele comparará o valor do campo "Host" 
com os nomes de domínio configurados (server_name) para determinar qual servidor virtual deve lidar com a solicitação.

No seu caso, ambos os servidores virtuais têm o mesmo nome de servidor (exemplo.net e www.exemplo.net), então o Nginx usará o campo "Host" no cabeçalho da solicitação para escolher qual servidor virtual deve ser usado.

Escolha do Servidor Virtual: O Nginx selecionará o primeiro servidor virtual que corresponda ao valor do campo "Host". Portanto, mesmo que os servidores virtuais tenham endereços IP diferentes, se o campo "Host" for exemplo.net ou www.exemplo.net, a solicitação será direcionada para o primeiro servidor virtual que você definiu.
Em resumo, a ordem de verificação é:

Verificação da combinação de porta e endereço IP.
Verificação do campo "Host" (nome do servidor) para selecionar o servidor virtual correspondente.
Certifique-se de configurar os servidores virtuais com nomes de domínio distintos se você desejar que diferentes endereços IP e portas sejam tratados de forma distinta.




*/
int Response::selectServer(Request& stringDomain, std::vector<SocketS> serverSocket)
{
    int indexChosenSocket = -1;
    for (size_t serverIndex = 0; serverIndex < serverSocket.size(); ++serverIndex)
    {
        std::cout << "-------------- DENTRO DO SELECT SERVER ------------" << std::endl;
        std::cout << "Dominio da request: " << stringDomain.getDomainRequest() << "| Size: " << stringDomain.getDomainRequest().size() << std::endl;
        std::cout << "Address do servidor: " << serverSocket[serverIndex].getAddress() << "| Size: " << serverSocket[serverIndex].getAddress().size() << std::endl;
        std::cout << "Porta do servidor: " << serverSocket[serverIndex].getPort() << " | Size: " << serverSocket[serverIndex].getPort().size() << std::endl;
        std::cout << "Porta da request: " << stringDomain.getPortRequest() << " | Size: " << stringDomain.getPortRequest().size() << std::endl;
        
        if (stringDomain.getDomainRequest() == serverSocket[serverIndex].getAddress())
        {
            std::cout << "Dominio igual!" << std::endl;
            //vê a porta
            if (stringDomain.getPortRequest() == serverSocket[serverIndex].getPort())
            {
                std::cout << "Porta igual!!!!!" << std::endl;
                // precisa verificar o sevrer_name nesse caso?
                //se for tudo certinho...
                indexChosenSocket = serverIndex; //depois passar isso pra essa condicao abaixo
            }
        }
        
    }
    return indexChosenSocket;
    // construir resposta de erro... (se sair do loop é que deu merda)

/*
    for (size_t serverIndex = 0; serverIndex < serverSocket.size(); ++serverIndex)
    {
        if(stringDomain.getDomainRequest().find(".") != std::string::npos) //ip
        {
            if(stringDomain.getDomainRequest() == serverSocket[serverIndex].getAddress())
            {
                std::cout << "é igual a ipaddress" << std::endl;
                if (stringDomain.getPortRequest().size() > 0)
                {
                    // teve port, daí tem que comparar isso e ver se está configurado no server tambem
                    if(stringDomain.getPortRequest() == serverSocket[serverIndex].getPort())
                    {
                        std::cout << "tem port e é o mesmo";
                        // daí sim prossegue pra construir uma response que pode ser bem sucedida
                        //ou seja: chama um objeto Response e passa essa request e esse serverSocket[...] como parametro
                        //......
                        
                        break;
                    }
                    else
                    {
                        std::cout << "Não é o mesmo port. Então NÃO ROLA" << std::endl;
                        //construir uma response de erro (ver qual é o statusCode condizente e 
                        //como ficará o resto do cabeçalho)
                        // response de erro é STRING               
                    }
                }
            }    
        }
        else if(contains(serverSocket[serverIndex].getServerName(), stringDomain.getDomainRequest()))
        {
          std::cout << "tá no server name" << std::endl;
          if (stringDomain.getPortRequest().size() > 0)
            {
                // teve port, daí tem que comparar isso e ver se está configurado no server tambem
                if(stringDomain.getPortRequest() == serverSocket[serverIndex].getPort())
                {
                    std::cout << "tem port e é o mesmo";
                    // daí sim prossegue pra construir uma response que pode ser bem sucedida
                    //ou seja: chama um objeto Response e passa essa request e esse serverSocket[...] como parametro
                    //.....
                    break;
                }
                else
                {
                    std::cout << "Não é o mesmo port. Então NÃO ROLA" << std::endl;
                    //construir uma response de erro (ver qual é o statusCode condizente e 
                    //como ficará o resto do cabeçalho)
                    // response de erro é STRING
                }
            }
        }
    }
    return "";
*/
}

std::string Response::getResponse()const{
    return _response; //Retorna o corpo da resposta
}

const std::string Response::getPath()const{
    return _path;
}

void Response::setPath(const std::string& allPath){
    _path = allPath;
}

std::string readFileToString(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cout << "Não deu pra abrir o html" << std::endl;
        return ""; // Retorna uma string vazia se não for possível abrir o arquivo
    }
    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n"; // Adiciona cada linha ao conteúdo da string
    }
    file.close();
    return content;
}
/*

        //error404Html(response, root);
        
        // std::string errorFilePath = root + "Error404.html";
        // std::string errorHtml = readFileToString(errorFilePath);
        // if (!errorHtml.empty()){ // deucertinho?
        //     std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n" + errorHtml;
        //     setResponse(response);
        //     return response;
        // } else {
        //     // se na tiver o .htm aprece uma msg
        //     std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n" + errorFilePath;
        //     setResponse(response);
        //     return response;
        // }
        
        
        // error405Html(response, root);
        // error500Html(response, root);
        // error503Html(response, root);
        // error505Html(response, root);
        
*/
        

std::string Response::errorCodeHtml(int statusCode, SocketS &server){

        std::string errorFileName;
        std::string errorDefault;

        errorFileName = server.getErrorPage(statusCode); //caminho
        errorDefault = _statusMessages.getMessage(statusCode); //mensagm
        
        //le o arquivo
        //monta as headers
        // concatena o arquivo com as headers
        std::string errorHtml = readFileToString(errorFileName);
		size_t content_length = errorHtml.size();
		std::string response;
        response += "HTTP/1.1 " + std::to_string(statusCode) + " " + errorDefault + "\r\n";
        response += "Content-Type: text/html\r\n";
		response += "Content-Length: " + std::to_string(content_length) + "\r\n";
		response += "Connection: close\r\n\r\n" + errorHtml;
		setResponse(response);
        return response;

}

std::string Response::generateResponse(int statusCode, const Request& request, const std::string& root, const std::string& filePath){

    std::string contentType;
    std::string content = readFileToString(root + filePath);
    std::string response;
    
    if(request.getURI().find("css"))
        contentType = "text/css";
    else if(request.getURI().find("jpg") || request.getURI().find("jpeg"))
        contentType = "image/jpeg";
    else if(request.getURI().find("png"))
        contentType = "image/png";
    else if(request.getURI().find("gif"))
        contentType = "image/gif";
    else
        contentType = "text/html";
    response += "HTTP/1.1 " + std::to_string(statusCode) + " OK\r\n";
    response += "Content-Type: " + contentType + "\r\n";
    response += "Content-Length: " + std::to_string(content.length()) + "\r\n";
    response += "Connection: Keep-Alive\r\n";
    response += "\r\n"; // Linha em branco indica o fim dos cabeçalhos
    response += content;
    setResponse(response);
    return response;

}

std::string Response::httpGet(Request &request, SocketS &server, Response *this_response){
    std::string root;
	std::string indexPage;

	root = server.getRoot();
	// ACIMA: não é preciso também ver se tem um ROOT ESPECIFICO DO LOCATION? (e, caso tenha,
	// utilizá-lo como referencia?)

    std::map<std::string, LocationDirective> serverLocations = server.getLocations(); //obtenho informações das loalizacores d drtivas
    std::map<std::string, LocationDirective>::iterator it = serverLocations.find(request.getURI()); //cria um iterador p percorrer o map e encontrar uma chave correspondente ao vlr de retorno da getURI
    std::map<std::string, std::vector< std::string > > locationDirectives; //cria map p armzenar dirtivas de localizacao
	// tem que ver se, caso seja um diretorio... daí tem toda
	// a questão do autoindex
	// se não for um diretorio (nao termina com '/')
	// ou: se for um arquivo, tem extensão?

    if (it != serverLocations.end()){ // s enconra as dirtivas
        std::cout << "Directives from this Location found!" << std::endl;
        locationDirectives = it->second.getDirectives(); //obtem a diretia de localizcao
         std::map<std::string, std::vector< std::string > >::iterator itRoot = locationDirectives.find("root"); //procura a dirtiva root
        if (itRoot != locationDirectives.end())
            root = itRoot->second[0]; // se tem root atualiza os valores especificados

        std::cout << "Directives from this Location found!" << std::endl; //o mesmo para Index
        locationDirectives = it->second.getDirectives();
         std::map<std::string, std::vector< std::string > >::iterator itIndex = locationDirectives.find("index");
        if (itIndex != locationDirectives.end()){ //se tiver a diret index
			    indexPage = itIndex->second[0];
		      }
		    else
			    indexPage = server.getIndexFile();
		// tem que testar pra validar que esse caminho existe...
		// se não dá o que? server error?! --> como o nginx lida com isso?
		// ele valida nesse ponto aqui mesmo, no momento da requisição
		// se o arquivo NÃO EXISTE, ele retorna um 404 (not found)


        // COMPLETAR O CAMINHO DO ARQUIVO COM O ROOT (daí tem que verificar a diretiva root tambem)
        this_response->setPath(root + indexPage); //isso deveria atualizar o valor do path com o root mais a index
        std::string bodyHTML = readFileToString(this_response->getPath()); //declara o corpo do HTML
		struct stat info;
		if (stat(this_response->getPath().c_str(), &info) != 0)
		{
			// retorna pagina de erro (o caminho pra pagina nao existe)
			
		}
        this_response->_body = bodyHTML;
        std::cout << "----------- CONSTRUÇÃO DA RESPONSE DO HTTPGET COMEÇA AQUI O: -----------------------------" << std::endl;
		std::string response;
		std::cout << "REQUEST URI: " << request.getURI() << std::endl;
		if (request.getURI().find("styles") != std::string::npos)
			response = "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\n\r\n" + this_response->_body;
        else
			response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + this_response->_body;
			// response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + this_response->_body;
        this_response->setResponse(response);
        return response;
    	} else {
    	    std::cout << "This server doesnt have this location!!" << std::endl;
    	    this_response->errorCodeHtml(404, server);
    	    //error404Html(response, root);
		}
        // std::string errorFilePath = root + "Error404.html";
        // std::string errorHtml = readFileToString(errorFilePath);
        // if (!errorHtml.empty()){ // deucertinho?
        //     std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n" + errorHtml;
        //     setResponse(response);
        //     return response;
        // } else {
        //     // se na tiver o .htm aprece uma msg
        //     std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n" + errorFilePath;
        //     setResponse(response);
        //     return response;
        // }
        // }
        
        /*
        
            else {
            std::cout << "This server doesnt have this location!!" << std::endl;
            std::string errorFilePath = root + "error404.html"; // Caminho para o arquivo HTML de erro 404

            if (fileExists(errorFilePath)) {
            std::string bodyHTML = readFileToString(errorFilePath);
            std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n" + bodyHTML;
            setResponse(response);
            return response;
            } else {
            // Se o arquivo de erro personalizado não existir, use uma mensagem de erro padrão
            std::string errorMessage = "<html><head></head><body><h1>Error 404</h1></body></html>";
            std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n" + errorMessage;
            setResponse(response);
            return response;
            }
        }
        
        
        
        */
        
        //std::string response = "HTTP/1.1 404 Not found\r\nContent-Type: text/html\r\n\r\n<html><head></head><body><h1>Error 404</h1></body></html>";
        // soma  body que leio da pagina
        // std::string bodyHTML = readFileToString(getPath());
        // juntar as strings
        // cabçalho e o body 
        
        // this_response->setResponse(response);
        // return response;
    return "DEU MERDA JOHNSONS";
}

std::string Response::buildResponse(Request &request, SocketS &server)
{
    
    //////////    
    // Implementação para construir a resposta completa, incluindo cabeçalho e corpo
    // Incluir os cabeçalhos necessários, como Content-Length e outros
    // Retorne a resposta completa construída
    //////////

    
	std::cout << "----> CHEGAMOS AO BUILD RESPONSE -----" << std::endl;
    // verificar se o método requisitado pela solicitação é permitido pra esse servidor
    std::vector<std::string> allowed_methods = server.getMethods();
    std::string requestMethod = request.getMethod();
    
    bool found = false;

    for (std::vector<std::string>::iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it) {
        if (strcmp(it->c_str(), requestMethod.c_str()) == 0) {
            found = true;
            break;
        }
    }

    std::string hasRoot;

    if (found) {
        // método é permitido pra esse servidor. Continua...
		std::cout << "Encontramos o método permitido!" << std::endl;
		std::string resposta = _methodsFunctions[requestMethod](request, server, this);
		setResponse(resposta);
		std::cout << "A resposta é:::: " << getResponse() << std::endl;
    	return resposta;
    }
    else
    {
        //constrói resposta de erro porque esse método não é permitido
        // e retorna
        std::cout << "MÉTODO NÃO PERMITIDO!";
        std::cout << "This server doesnt have this location!!" << std::endl;
        std::string response = "HTTP/1.1 404 Not found\r\nContent-Type: text/html\r\n\r\n<html><head></head><body><h1>Error 404</h1></body></html>";
        setResponse(response);
        return response;    
    }
    std::string fullResponse;
    
    // Construa os cabeçalhos aqui, usando os dados em _headers
    
    fullResponse += "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"; // Carla não esquece de adds uma linha em branco entre cabeçalhos e corpo
    fullResponse += _body;  // Adicione o corpo da resposta
    setResponse(fullResponse);
    return fullResponse;
}



/*

Agora preciso que seja criado um metodo  
que avaliara se no atributo SocketS tem uma diretiva location , 
se tiver a diretiva location verifica se tem uma root se tiver uma 
root entao essa root sera usada para determinar o diretorio raiz. 
Se não houver a diretiva location ou se nao tiver um root dentro da 
location o servidor entao usa a diretiva root global definida fora. 
Agora fica a questao , e se nao tiver uma root na global?



*/


void Response::reset()
{
    _headers.clear(); //limpa os cabeçalhos
    _body.clear(); //limpa o corpo
    _code = ""; //limpa o código de status
}

void Response::httpError(std::string errorCode, const std::string &errorMessage)
{
    _code = errorCode;
    _response = errorMessage;
    _headers.clear(); //limpa cabeçalhos anteriores
}



/*





    std::map<std::string, std::string>  _headers;
    std::string                         _body;
    std::string                         _response; //criar uma string response que, será todo esse cabeçalho + body (ver exemplos no chat)
    std::string                         _code;
    
    ------------------------------------------- LETICIA DEIXOU ANOTADO ----------------------------------------------
    criar uma string response que, será todo esse cabeçalho + body (ver exemplos no chat)
    PRA DEIFINIR O BODY (um arquivo html que voce precisa "pegar" do diretorio que temos, por exemplo)
    1. Identificar QUAL é o método sendo pedido pela request e verificar que esse socket ACEITA ESSA MÉTODO (get, por exemplo)
        ---> se NÃO aceitar, construir uma RESPOSTA DE ERRO (com o statusCode adequado) e ARMAZENAR isso na response da classe e retornar (pra interromper o metodo)
    2. SE ACEITAR, daí IDENTIFICAR qual é o RECURSO (URI) pedido pela request e verificar se
         esse recurso está entre os locations (map) desse SERVER SOCKET (qualquer duvida, dar uma olhada no hpp do SocketS)
        ---> Se esse recurso NÃO ESTIVER DENTRE AS LOCATIONS, tem que construir uma RESPOSTA DE ERRO (404?) e armazenar na resposne da classe e retornar (pra interromper o metodo)
    3. SE ESTIVER PRESENTE, acessar o valor (LocationsDirective) desse location no map e indeitificar o arquivo INDEX pedido (está em algum lugar do diretorio) e
        criar um método pra abrir esse arquivo e armazenar seu conteúdo numa string (a string body, no caso)
 precisa devolver essa string de alguma forma (get?) pra mainLoop
 pra que ela possa fazer send() dessa response pro clientSocket

*/



   // Response::Response(){}
   // Response::~Response(){}
   // Response::Response(Request *req, Server *_server){}
   // 
   // //getters and setters
   // size_t Response::getIdClient()const{}
   // const std::string& Response::getHeader(const std::string& header)const{} // busca pelo campo de cabeçalho na lista de cabeçalhos
   // 
   // //response methods
   // void Response::status(int statusCode, const std::string& header)const{} // status da resposta com base no código e no texto fornecidos
   // void Response::text(){} //configuração do corpo da resposta como texto
   // void Response::reset(){} //implementa a redefinição de resposta, limpando cabeçalhos e corpo
   // 
   // //response methods
   // void Response::processResponse(){} //processa a solicitacao, verifica método, URI, etc. e constrói a resposta de acordo com os requisitos
   // void Response::createResponse(std::string requestedContent){}
   // void Response::readRequestedFile(std::string requestedContent){}

/*

Receber uma Request e Identificar o Socket do Servidor:

Quando o servidor recebe uma solicitação (request) de um cliente, ele primeiro analisa essa solicitação.
Para determinar qual servidor socket deve processar essa solicitação, o servidor verifica o cabeçalho Host da solicitação.
(IPADRESS, )
Ele compara o valor do cabeçalho Host (que geralmente é o nome de domínio do site) com os endereços IP, nomes de servidor e portas associados aos seus próprios servidores sockets.
Com base nessa comparação, o servidor identifica o socket específico que deve lidar com essa solicitação.
Passar o Socket e a Request para o Método da Classe Response:

Uma vez que o servidor tenha identificado o socket correto, ele passa o socket e o objeto da solicitação (Request) para um método na classe Response.
Isso permite que o objeto Response saiba qual socket está lidando com a solicitação e possa trabalhar com ele.
Verificar o Método e Validar o Socket:

No método da classe Response, o objeto Response verifica o método (GET, POST, etc.) que foi solicitado na Request.
Também verifica se o socket correspondente suporta esse método. Se não suportar, constrói uma resposta de erro indicando que o método não é permitido.
Se houver um erro, a Response cria uma resposta de erro e retorna essa resposta para a MainLoop.
Comparar a URI (Recurso) com os Locations Disponíveis:

Se o método for suportado pelo socket e não houver erro, a Response continua verificando a URI (recurso) que foi solicitada na Request.
Ela compara essa URI com os possíveis "locations" que esse socket tem gravados.
Os "locations" são mapeados usando um mapa que relaciona URIs a objetos LocationDirectives.
Construir a Resposta Básica:

Se a URI solicitada corresponder a um dos "locations" disponíveis para esse socket, a Response acessa o objeto LocationDirectives correspondente.
Se houver um recurso chamado "index" dentro desse objeto LocationDirectives (representado por uma entrada no mapa), a Response usa o valor associado para localizar o recurso real (por exemplo, "index.html") no diretório.
Ela lê o conteúdo desse recurso e o armazena em uma string.
Enviar a Resposta para o Cliente:

A Response, após ler e processar o conteúdo do recurso, constrói uma resposta de sucesso usando o conteúdo do recurso como o "body" da resposta.
Ela retorna essa resposta construída para a MainLoop.
A MainLoop, por sua vez, envia essa resposta de sucesso para o cliente usando o socket associado.
Esse é um fluxo detalhado de como o servidor identifica o socket, verifica o método, 
compara a URI e constrói uma resposta básica. Lembre-se de que essa é uma descrição 
de alto nível e que a implementação real envolverá várias etapas, verificações e 
tratamento de erros ao longo do processo.



SE FOR NUMERO CONFERE O FORMATO E CONFERE SE É O MESMO QUE ESTA EM IP ADRESS BATE COM O DO LISTEM
SE FOR ESCRITO ENTAO PUXA NO SERVER_NAME
SE TIVER PORTA, SE ELA ESTA PREENCHIDA NA PORT DO SOCKETS
SE NAO TIVER PORTA É DEFAULT ENTAO PRECISA VERFICAR SE ESTÁ HABILITADA
SE DER ALGO ERRADO ENTAO CONSTROI UMA RESPNSE DE ERRO COM O STATUS DE ERRO E RETORNA ISSO PRA MAIN LOOP E A MAIN LOOP FAZ UMA SEND PARA O CLIENT
SE DER CERTO DAI VOCE PROSSEGUE, MANDA O OBJETO REQUEST E ESSE SOCKET QUE ENCONTREI. NESSA FUNCAO QUE TEM
*/

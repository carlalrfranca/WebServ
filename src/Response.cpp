/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/18 18:00:34 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/24 21:40:13 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Response.hpp"

Response::Response() : _headers(), _methodsFunctions()
{
    _chosenSocket = NULL;

	/*
		--- NOTE 18.09.2023 ---
		
		map onde as chaves são os verbos e os valores os métodos:
		
		httpGet | A função retorna a resposta HTTP gerada, que pode conter o conteúdo do arquivo solicitado ou uma resposta de erro.
		postMethod | A função retorna a resposta HTTP gerada, que pode ser a resposta do CGI bem-sucedida ou uma resposta de erro.
		deleteMethod |
		
	*/ 
	
	_methodsFunctions["GET"] = &Response::httpGet;
	_methodsFunctions["POST"] = &Response::postMethod;
	_methodsFunctions["DELETE"] = &Response::deleteMethod;
    _body = "";
    _code = "";
    _path = "";
    _response = ""; //criar uma string response que, será todo esse cabeçalho + body (ver exemplos no chat)
    _statusMessages = StatusMessages();
}

Response::~Response(){}

Response::Response(Request request)
{
    // define corpo da resposta lendo o conteudo do arquivo html
    // _body = readHtmlFile("web/html_upload_arquivo/index.html");
    
    // precisa definir os campos do cabeçalho aqui, entao se for inserir mais É AQUI
    // setStatusCode("200 OK");
    // setContentType("text/html");
    // setDateAndTime();
    // setContentLength(_body.length());
    _chosenSocket = NULL;
}

std::string Response::postMethod(Request &request, SocketS &server, Response *this_response)
{
	// TO DO:
	/*
		ANTES DE TUDO:
		-> verificar o TAMANHO DO CONTEUDO DO POST (vê se está dentro dos limites de max-content
		determinado no arquivo de configuração. Se não estiver, corta na hora reornando uma
		RESPONSE DE ERRO DE ACORDO COM A SITUAÇÃO)
	*/
	/*
		1) Pegar o root base
		2) Pegar o location e decidir qual é o location mais adequado
		3) Se tem .cgi é script -> vai pra um fluxo (que tá feito)
		4) Se não tem, é porque é /upload/ e segue OUTRO FLUXO
		---> tem que ter a parte de identificar se a uri tem varias partes e teria que extrair pedaços em comum? talvez..
	*/
	
	// ---------------------------------------------------------------------------------
	std::string root_for_response = server.getRoot();
	// pega o root... encontra o location que tá sendo pedindo na uri
	// pra post vai ser ou algo com o cgi.. ou um post comum, com /upload, por exemplo
	// -----> lembre-se: o delete vai vir implicito numa solicitação POST, então..
	// -----> é preciso identificar se a solicitação POST é mesmo um post OU SE É UM DELETE antes
	// -----> de chegar aqui -> definir na buildResponse e modificar a variavel do método de acordo
	// -----> (manipulação mesmo) pra forçar a chamar a função correta...
	std::map<std::string, LocationDirective> serverLocations = server.getLocations();
	std::map<std::string, LocationDirective>::iterator it = this_response->findRequestedLocation(request, server, serverLocations);
	std::string uri = request.getURI();
    std::map<std::string, std::vector< std::string > > locationDirectives;

	// ---- o de antes...
	std::cout << RED << "Esta é a URI DESSE POST::: " << uri << END << std::endl;
	// precisa usar o find de acordo com a URI -> definir se ele é pro /process_data.cgi OU pro /images/
	// daí cada um tem um fluxo
	/*
		PARA ISSO, PRECISA FAZER UM CAMINHO SIMILAR AO DO GET:
		1 - procurar o location adequado (nesse caso, o do cgi ou do /images/)
		2 - ver o root (se tiver, senao, usa o default)
		3 - ver se tem algum allow_methods interno impedindo o POST (isso pode ser feito antes)
		4 - (depois tem que testar quando chega sem '/' no inicio, o httpGET que não consegue extrair a diferença da URI - aconteceu pra pedir o css do cgi com 'location cgi-bin')
	*/
	if (it != serverLocations.end())
		std::cout << BLUE << "Location found! >> " << it->first << END << std::endl;
	// isso abaixo já é pro caso de script cgi - pra retirar o nome do script e coisa do tipo
	size_t foundLastSlash = uri.find_last_of('/');
	std::string scriptName;
	if (foundLastSlash != std::string::npos && foundLastSlash != (uri.size() - 1))
	{
		// se nao é uma barra no final da uri... extrai da ultima barra até o final (deve extrair 'process_data.cgi' se for cgi)
		// tem que verificar isso antes, não? Porque pode ter um POST pra /upload/ tambem...
		// e daí É UM FLUXO DIFERENTE
		scriptName = uri.substr(foundLastSlash);
		std::cout << BLUE << "NOME DO SCRIPT: " << scriptName << END << std::endl;
		// criar um else pra isso, pra caso haja uma barra no final, daí pegar a anterior,
		// extrair o in between e verificar se é .cgi? porque daí tudo bem tambem nao?
		// se nao, daí dar NÃO ENCONTRADO (404)?
	}
	size_t found = scriptName.find(".cgi");
	if (found == std::string::npos)
	{
		std::cerr << "---- Opa, essa request NÃO tem process_data CGI !!! ----" << std::endl;
		this_response->errorCodeHtml(404, server);
		return this_response->getResponse();
	}
	
	if (it != serverLocations.end())
	{
		CGI script;
		size_t foundCGIExt = scriptName.find(".cgi");
		if (foundCGIExt != std::string::npos)
		{
			scriptName = scriptName.substr(0, foundCGIExt);
		}
		// agora verificamos se há um "root" dentro desse location (um root mais "especifico", portanto)
		std::map<std::string, std::vector< std::string > > locationDirectives;
		locationDirectives = it->second.getDirectives();
        std::map<std::string, std::vector< std::string > >::iterator itLocationRoot = locationDirectives.find("root");
		if (itLocationRoot != locationDirectives.end())
		{
			// quer dizer que teve um root mais especifico no location... daí substituímos o root_for_response por essa..
			root_for_response = itLocationRoot->second[0];
			std::cout << "Root ESPECIFICO da location do CGI:::: " << root_for_response << std::endl;
		}
		// tem que pegar outras duas diretivas dentro do it (ponteiro pra esse location):
		/*
			cgi_path --> location of interpreters installed on the current system, mandatory parameter
			cgi_ext --> extensions for executable files, mandatory parameter
		*/
		// passar aqui pra classe de ValidationsPost
		std::map<std::string, std::vector< std::string > >::iterator commandOfCGI = locationDirectives.find("cgi_path");
		if (commandOfCGI != locationDirectives.end())
		{
			// encontrou CGI-path... LIMITAR ISSO A UM TIPO APENAS?
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
					std::cout << "Extensões dos scripts CGI ------> " << scriptsExtensions[j] << std::endl;
				std::cout << "------------------------------------------------------" << std::endl;
				std::string pathToScript = root_for_response + scriptName;
				
				// setando todas essas variaveis no objeto CGI pra processar o resto A PARTIR DELE
				script.setRoot(root_for_response);
				script.setCommands(scriptsCommands);
				script.setExtensions(scriptsExtensions);
				script.setPathToScript(scriptName);
				// std::cout << BLUE << "CAMINHO SCRIPT COM EXTENSÃO >> " << script.getPathToScript() << END << std::endl;
				struct stat info;
		 		if (stat(script.getPathToScript().c_str(), &info) != 0)
		 		{
					std::cerr << "Error: Config file doesn't exist!" << std::endl;
					this_response->errorCodeHtml(404, server);
					return this_response->getResponse();
		 		}
				std::cout << YELLOW << "<< CAMINHO INTEIRO PARA O SCRIPT CONSTRUIDO >>" << END << std::endl;
				std::cout << YELLOW << script.getPathToScript() << END << std::endl;
				std::cout << std::endl;
				// podia já declarar um objeto CGI e inserir essas informações (extensão e comando) dentro dele, certo?
				// e daí prosseguir a partir dele...
				// chamando seus métodos aqui pra executar e criar a resposta...
				// daí no final só armazenaria o que ele retorna na _response daqui....
				int resultCode = 0;
				resultCode = script.handleCGIRequest(request); //pode dar um retorno caso precise retornar página de erro?
				if (resultCode != 204)
				{
					this_response->errorCodeHtml(resultCode, server);
					return this_response->getResponse();
				}
				std::cout << RED << "RESPONSE DO CGI" << END << std::endl;
				std::cout << RED << script.getResponse() << END << std::endl;
				this_response->setResponse(script.getResponse());
				return script.getResponse(); // retorna a response
			}
			else
			{
			//  se nao encontrar a diretiva 'cgi-ext'.. o que acontece? ele só grava sem nenhum tratamento ou dá resposta de erro?
				this_response->setDateAndTime();
				std::string date = this_response->getDate();
				this_response->errorCodeHtml(404, server);
				return this_response->getResponse();
			}
		}
		else
		{
			// se nao encontrar a diretiva 'cgi-path'.. o que acontece? ele só grava sem nenhum tratamento ou dá resposta de erro?
			this_response->errorCodeHtml(404, server);
			return this_response->getResponse();
		}
	}
	// quer dizer que nao teve o locatin do cgi-bin, então lidamos com a postagem da forma "padrão" (grava o conteudo num arquivo no diretorio raiz do projeto)
	std::cout << "NÃO HÁ um location PRO CGI! Vai pro caminho padrão..." << std::endl;
	// ou seja, só "posta" o que tiver que postar num arquivo no diretório
	this_response->errorCodeHtml(404, server);
	return this_response->getResponse();
}


// std::string OLAR = "TAMO NO DELETE METHOD";
// return OLAR;
// curl -X DELETE -i -v http://google.com.br
	
	

std::string Response::getParameter(std::string query, std::string parameter)const
{
	size_t start = query.find(parameter + "=");
	
	if(start != std::string::npos)
	{
		start += parameter.length() + 1;
		size_t end = query.find("_method", start);
		if(end != std::string::npos)
		{
			end = query.length();
		}
		return query.substr(start, end - start);
	}
	return ""; //retorna vazio se o parametro não for encontado
}
	
std::string Response::getMethod()const
{
	const std::string override = getParameter("", "name"); //pega  o parmtro no post 
	
	if(!override.empty())
		return override;
		
	return "-->metodo original<--";
}

	
std::string Response::deleteMethod(Request &request, SocketS &server, Response *this_response)
{
	// 1. extrair a uri da solicitacao HTTP recebida p identificar o recurso que deve ser excluído
	// std::string uri = request.getURI();
	// construir o caminho do recurso
	// std::string caminhoDoRecurso = //constroi aqui

	std::map<std::string, LocationDirective> serverLocations = server.getLocations();
	std::map<std::string, LocationDirective>::iterator it = this_response->findRequestedLocation(request, server, serverLocations);
	std::string uri = request.getURI();

	std::cout << BLUE << "CHAMAMOS O METODO DELETE" << END << std::endl;
	if (it != serverLocations.end())
		std::cout << BLUE << "Location found! >> " << it->first << END << std::endl;
	std::cout << BLUE << "URI >>> " << uri << END << std::endl;
	std::string root;
	if(server.getRoot().size() > 0)
		root = server.getRoot();
	std::map<std::string, std::vector< std::string > > locationDirectives;
	locationDirectives = it->second.getDirectives(); //obtem as diretivas da location
	std::map<std::string, std::vector< std::string > >::iterator itRoot = locationDirectives.find("root"); //procura a dirtiva root
	if (itRoot != locationDirectives.end())
		root = itRoot->second[0];
	this_response->buildPathToResource(root, request, server, locationDirectives, it);
	std::cout << YELLOW << "PATH::: " << this_response->getPath() << END << std::endl;
	// std::string resourcePath = root + uri;
	std::string resourcePath = this_response->getPath();
	//4. verificando antes de excluir
	if(this_response->_utils.fileExists(resourcePath) == false)
	{
		std::cout << RED << "O arquivo NÃO EXISTE nesse caminho" << END << std::endl;
		this_response->errorCodeHtml(404, server);
		return this_response->getResponse();
	}
	if(remove(resourcePath.c_str()) == 0)
	{
		std::cout << BLUE << "Excluiu [aparentmente]" << END << std::endl;
		this_response->generateResponse(204, request); //exclusão bem-sucedida
	}
	else
		this_response->errorCodeHtml(500, server);

	return this_response->getResponse();
}


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

void Response::setStatusCode(const std::string& statusCode)
{
    _headers["Status"] = statusCode;
}

void Response::setContentType(const std::string& contentType)
{
    _headers["Content-Type"] = contentType;
}

void Response::setDateAndTime()
{
    char buffer[80];
    time_t rawTime;
    struct tm *timeInfo;
    
    time(&rawTime);
    timeInfo = localtime(&rawTime);
    
    //string info 
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", timeInfo);
    _headers["Date"] = buffer;
}

std::string Response::getDate()const
{
	std::map<std::string, std::string >::const_iterator it = _headers.find("Date");
	if (it != _headers.end())
		return it->second;
	return "Date not found";
}

void Response::setResponse(const std::string& response)
{
    _response = response;
}

const std::string& Response::getHeader(const std::string& header)const
{
    static std::string emptyHeader = ""; // Cria uma string vazia para retorno padrão

    // Implementação para buscar e retornar o cabeçalho específico
    // Se o cabeçalho não existir, retorne a string vazia criada acima

    return emptyHeader;
}

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

std::string Response::getResponse()const
{
    return _response; //Retorna o corpo da resposta
}

const std::string Response::getPath()const
{
    return _path;
}

void Response::setPath(const std::string& allPath)
{
    _path = allPath;
}

std::string readFileToString(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
	{
        std::cout << "Não deu pra abrir o html" << std::endl;
        return ""; // Retorna uma string vazia se não for possível abrir o arquivo
    }
    std::string content;
    std::string line;
    while (std::getline(file, line))
        content += line + "\n"; // Adiciona cada linha ao conteúdo da string
    file.close();
    return content;
}

void Response::listFilesAndGenerateHtml(std::map<std::string, LocationDirective>::iterator& it)
{

	//passar essa função pra parte de execução do autoIndex:
    std::string path = getPath();
	try
	{
        // if (path == "on")
		// {
            // const char *directoryPath = ".";
            // DIR *directory = opendir(directoryPath);
			std::cout << "PATH> " << path << std::endl;
			DIR *directory = opendir(path.c_str());
            if (directory){
                struct dirent *entry;
                std::vector<std::string> fileList;
                while ((entry = readdir(directory)) != NULL)
				{
                    fileList.push_back(std::string(entry->d_name));
				}
                closedir(directory);
				std::cout << "Tamanho da Lista de Itens do Diretório > " << fileList.size() << std::endl;
                for (size_t i = 0; i < fileList.size(); ++i){
					// aqui tá só listando, mas vamos precisar gerar um html com isso
                    std::cout << i << " - " << fileList[i] << std::endl;
				}
				std::cout << BLUE << "cabo listagem..." << END << std::endl;
				generateHtmlFromFiles(fileList, it);
				std::cout << BLUE << "Passou do generate HTML com base na lista" << END << std::endl;
            } else 
			{
                throw std::runtime_error("Error when open directory.");
            }
        // } else if (path == "off")
		// {
        //     std::cout << "Autoindex is on." << std::endl;
        // } else
		// {
        //     throw std::runtime_error("Invalid value to autoindex.");
        // }
    } catch (const std::exception &e)
	{
        std::cout << "Error: " << e.what() << std::endl;
		// produz a pagina de erro 403 Forbidden (ou 404?)
    }
	std::cout << BLUE << "Depois do try catch mas ainda na função" << END << std::endl;
}

void Response::generateHtmlFromFiles(const std::vector<std::string>& fileList, std::map<std::string, LocationDirective>::iterator& it){

    std::string html;
	std::string location = it->first;
	std::string path = getPath();
	std::cout << "Path: " << path << std::endl;
	if (!location.empty() && location[location.size() - 1] != '/') {
        location += '/';
    }
    html += "<html>\n";
    html += "<head><title></title></head>\n";
    html += "<body>\n";
    html += "<h1>File List</h1>\n";
    html += "<ul>\n";
	std::cout << BLUE << "cabo listagem...2" << END << std::endl;
	// se é um arquivo, será um li
	// se é um diretorio, tem que ser um link, e esse link tem que começar com /images
	//    -> porque ele substitui pelo root do location, que é web/images (e daí teria que completar com o resto do uri,
	// 		 e se estiver sem '/' final, tem que incluir)

	if (!_uri.empty() && _uri[_uri.size() - 1] != '/')
       	_uri += '/';
    for(size_t i = 0; i < fileList.size(); ++i)
	{
		// ver se é um arquivo ou um diretorio, se for um diretorio, tem que ser EM FORMA DE LINK
		std::string itemName = fileList[i];
		bool isDir = isDirectory(path + itemName);
		//html += "<li>" + fileList[i] + "</li>\n";
		if(isDir)
		{
			if (!itemName.empty() && itemName[itemName.size() - 1] != '/')
        		itemName += '/';
			std::cout << RED << "É diretorio: " << _uri + itemName << END << std::endl;
			html += "<li><a href='" + _uri + itemName + "'>" + itemName + "</a></li>\n";
		}
		else
		{
			std::cout << YELLOW << "É arquivo: " << _uri + itemName << END << std::endl;
			html += "<li>" + itemName + "</li>\n";		
		}

    }
	std::cout << BLUE << "cabo listagem...3" << END << std::endl;
    html += "</ul>\n";
    html += "</body>\n";
    html += "</html>\n";

    std::string headers;
	std::string contentType = "text/html";
	size_t content_length = html.size();
	std::stringstream contentLengthToString;
    contentLengthToString << content_length;
    std::string contentLengthStr = contentLengthToString.str();
	std::string statusCodeStr = "200";

    headers += "HTTP/1.1 " + statusCodeStr + " OK\r\n";
    headers += "Content-Type: " + contentType + "\r\n";
    headers += "Content-Length: " + contentLengthStr + "\r\n";
    headers += "\r\n";
    //junta os dois e seta a response

    std::string response = headers + html;
    setResponse(response);
}

void Response::errorCodeHtml(int statusCode, SocketS &server)
{
    std::string errorFileName;
    std::string errorDefault;
    errorFileName = server.getErrorPage(statusCode); //caminho
	std::cout << "Error File Name Path: " << errorFileName << std::endl;
    errorDefault = _statusMessages.getMessage(statusCode); //mensagm
    
    // le o arquivo
    // monta as headers
    // concatena o arquivo com as headers
    std::string errorHtml = readFileToString(errorFileName);
	size_t content_length = errorHtml.size();
	std::string response;
	std::stringstream toConvertToString;
    toConvertToString << statusCode;
    std::string statusCodeStr = toConvertToString.str();
    std::stringstream contentLengthToString;
    contentLengthToString << content_length;
    std::string contentLengthStr = contentLengthToString.str();

	response += "HTTP/1.1 " + statusCodeStr + " " + errorDefault + "\r\n";
    response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + contentLengthStr + "\r\n";
	// response += "Connection: close\r\n\r\n" + errorHtml;
	response += "\r\n" + errorHtml;
	setResponse(response);
    // return response;
}

void Response::generateResponse(int statusCode, const Request& request)
{
    std::string contentType;
    std::string content = readFileToString(getPath());
    std::string response;
	std::cout << " ----- URI ------------------------ " << request.getURI() << std::endl;
    if(request.getURI().find("css") != std::string::npos)
        contentType = "text/css";
    else if(request.getURI().find("jpg" ) != std::string::npos || request.getURI().find("jpeg") != std::string::npos)
        contentType = "images/jpeg";
    else if(request.getURI().find("png") != std::string::npos)
        contentType = "images/png";
    else if(request.getURI().find("gif") != std::string::npos)
        contentType = "images/gif";
    else
        contentType = "text/html";
	std::stringstream toConvertToString;
    toConvertToString << statusCode;
    std::string statusCodeStr = toConvertToString.str();
	std::stringstream toConvertToStr;
    toConvertToStr << content.length();
    std::string contentLenStr = toConvertToStr.str();
	
    response += "HTTP/1.1 " + statusCodeStr + " OK\r\n";
    response += "Content-Type: " + contentType + "\r\n";
    response += "Content-Length: " + contentLenStr + "\r\n";
    // response += "Connection: Keep-Alive\r\n";
    response += "\r\n"; // Linha em branco indica o fim dos cabeçalhos
    response += content;
    setResponse(response);
	std::cout << RED << "Response\n" << getResponse() << END << std::endl;
    // return response;
}

bool Response::isDirectory(const std::string& path)
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0){
        // Erro ao obter informações do arquivo Leti LEEEEEEEEEEE//ti quer cafe?
        std::cout << RED << "Esse file >>> " << path << " e aí?" << END << std::endl;
		return false;
	}// leti quer cafe?
    return (info.st_mode & S_IFDIR) != 0;
}

std::map<std::string, LocationDirective>::iterator Response::findRequestedLocation(Request &request, SocketS &server, std::map<std::string, LocationDirective>& serverLocations)
{
	//obtenho informações das loalizacoes
    std::map<std::string, LocationDirective>::iterator it = serverLocations.find(request.getURI()); //cria um iterador p percorrer o map e encontrar uma chave correspondente ao vlr de retorno da getURI
	std::string uri = request.getURI();
	if (it == serverLocations.end())
	{
		bool canBeRootLoc = false;
		// casos em que nao tem literalmente o que vem na URI nos location (tipo /styles/styles.css ou /about.html)
		// vamos ter que encontrar o location que lida com esse tipo de URI. No caso do 'styles/styles.css', tem que ser o location 'styles/'
		// e no caso do '/about.html', tem que ser o location 'about'
		for(std::map<std::string, LocationDirective>::iterator locFind = serverLocations.begin(); locFind != serverLocations.end(); ++locFind) //cria um iterador p percorrer o map e encontrar uma chave correspondente ao vlr de retorno da getURI
		{
			// bool canBeRootLoc = false;
			if (locFind->first == "/")
				canBeRootLoc = true;
			if (locFind->first != "/")
			{
				// nao pode tirar as duas primeiras condições? só deixar a terceira?
		        if(uri == locFind->first || uri.find(locFind->first + "/") != std::string::npos || uri.find(locFind->first) != std::string::npos){
	                it = locFind;
					std::cout << YELLOW << "LOCATION que se ENCAIXA nessa URI >>>>> " << it->first << END << std::endl;
	                return it;
	            }
			}
	    }
		if (it == serverLocations.end() && canBeRootLoc == true) //caso nao haja um location mais especifico, o location '/' vai responder pela requisição
			it = serverLocations.find("/");
	}
	std::cout << YELLOW << "LOCATION que se ENCAIXA >> " << it->first << END << std::endl;
	return it;
}

bool Response::isResponseADirectoryListingOrErrorPage(std::string path, SocketS &server, std::map<std::string, std::vector< std::string > >& locationDirectives, std::map<std::string, LocationDirective>::iterator& it, std::string indexPage)
{
	// std::cout << path << " é um diretório." << std::endl;
	std::cout << BLUE << "LOCAAAAATION>>> " << it->first << END << std::endl;
	locationDirectives = it->second.getDirectives();
       std::map<std::string, std::vector< std::string > >::iterator itIndex = locationDirectives.find("index");
    if (itIndex != locationDirectives.end()){ //não é o oposto? primeiro ve que se tem autoindex, se nao tiver (ou tiver off?) ve se tem index e serve se tiver? (ou gera pagina de erro se nao?)
		indexPage = itIndex->second[0];
	}
	else if (server.getIndexFile().size() > 0)
		indexPage = server.getIndexFile();
	else
	{
		// verificar se tem o autoindex, se não -> retorna pagina de erro
		std::cout << YELLOW << "Aqui teria que produzir o autoindex" << END << std::endl;
		// vê se tem auto_index on, se nao, dá erro, se sim, produzir o autoindex
		std::map<std::string, std::vector< std::string > >::iterator itAutoIndex = locationDirectives.find("auto_index");
		if (itAutoIndex != locationDirectives.end())
		{
			std::cout << BLUE << itAutoIndex->second[0] << END << std::endl;
			std::cout << BLUE << "Tamanho AUTOINDEX: " << itAutoIndex->second.size() << END << std::endl;
			if (itAutoIndex->second[0] == "on")
			{
				setPath(path);
				listFilesAndGenerateHtml(it);
				return true;
			}
			else
			{
				errorCodeHtml(403, server);
				return true;
			}
		}
		else
		{
			errorCodeHtml(404, server);
			return true;
		}
		// this_response->setPath(path); // nesse caso, o root desse location
		// indexPage = server.getIndexFile();
	}
	// se nao tiver index, na verdade, ver se o auto_index está on e preparar isso como resposta
	path += indexPage;
	setPath(path);
	return false;
}
/*

std::string Response::extractUriAndBuildPathToResource(std::string root, std::vector<std::string>& parts_uri, std::string& uri, std::map<std::string, LocationDirective>::iterator& it)
{
	std::string commonSubstring = "";
	std::string this_location = it->first;
	if (!uri.empty() && uri[0] == '/')
		uri.erase(0, 1);
	size_t minlen = std::min(this_location.length(), uri.length());
	for (size_t i = 0; i < minlen; ++i)
	{
    	if (this_location[i] == uri[i])
    	    commonSubstring += root[i];
    	else
			break; //aqui leti ele para quando encontra uma diferenca
	}
	std::string caminhoCompleto;
	std::cout << "Uri: " << uri << " | Common Substring: " << commonSubstring << std::endl;
	// if (commonSubstring == uri)
	// {
		// caminhoCompleto = commonSubstring;
		// return caminhoCompleto;
	// }
	std::string uriSemParteEmComum = uri.substr(commonSubstring.length());

	if (uriSemParteEmComum != root)
		caminhoCompleto = root + uriSemParteEmComum;
	else
		caminhoCompleto = root;

	// if (!caminhoCompleto.empty() && caminhoCompleto[caminhoCompleto.length() - 1] == '/')
		// caminhoCompleto.erase(caminhoCompleto.length() -1);
	return caminhoCompleto;
		
}*/

/**/
std::string Response::extractUriAndBuildPathToResource(std::string root, std::vector<std::string>& parts_uri, std::string& uri, std::map<std::string, LocationDirective>::iterator& it)
{
	std::cout << "Tem caminho além do arquivo | Tamanho: " << parts_uri.size() << std::endl;
	std::cout << parts_uri[0] << " - " << parts_uri[0].size() << " e " << parts_uri[1] << std::endl;
	std::string this_location = it->first;
	std::string commonSubstring = "";
	if (!uri.empty() && uri[0] == '/')
		uri.erase(0, 1);
	// _uri = uri;
	size_t minlen = std::min(root.length(), uri.length());
	size_t j = 0;
	size_t i = 0;
	std::cout << BLUE << "Root | " << root << " URI: " << uri << END << std::endl;

    for (size_t i = 0; i < minlen; ++i) {
        if (this_location[i] == uri[j]) {
            commonSubstring += this_location[i];
			j++;
        }
    }
	// commonSubstring = uri.substr(0, j);
	std::cout << "-- PARTE A EXTRAIR PRA FORMAR O CAMINHO REAL: " << commonSubstring << std::endl;
	// retirar a commonSubstring do uri e juntar?
	std::string uriSemParteEmComum = uri.substr(commonSubstring.length());
	std::string caminhoCompleto = "";
	if (uri != "/" && uri != root)
		caminhoCompleto = root + uriSemParteEmComum; // isso ja vai adicionar todo o caminho que vem da uri, se tiver?
	else
		caminhoCompleto = root;
	// std::cout << "Caminho completo quando tem ROOT ESPECIFICA: " << caminhoCompleto << std::endl;
	return caminhoCompleto;
}


/*
// pra formar o caminho:
-> chega a uri -> exemplo: /images/web/images
-> a location vai ser images/, que tem como root web/images/
-> vamos PRIMEIRO extrair da uri o que tem em relação à location (no caso, )
std::string Response::extractUriAndBuildPathToResource(std::string root, std::vector<std::string>& parts_uri, std::string& uri, std::map<std::string, LocationDirective>::iterator& it)
{
	std::cout << "Tem caminho além do arquivo | Tamanho: " << parts_uri.size() << std::endl;
	std::cout << parts_uri[0] << " - " << parts_uri[0].size() << " e " << parts_uri[1] << std::endl;
	std::string this_location = it->first;
	std::string commonSubstring = "";
	size_t minlen = std::min(this_location.length(), uri.length());
	std::cout << RED << "Esse LOCATION: " << this_location << END << std::endl;
	std::cout << RED << "Essa URI: " << uri << END << std::endl;
	std::cout << RED << "Min LEN: " << minlen << END << std::endl;
	size_t j = 0;
	size_t i = 0;
	// while (this_location[i] == uri[j])
	if (!uri.empty() && uri[0] == '/')
		uri.erase(0, 1);
	std::cout << BLUE << "Essa URI: " << uri << END << std::endl;
	while (root[i] == uri[j])
	{ // é claro que ele nao tira o root porque o uri começa com '/'
		i++;
		j++;
	}
	std::string newUri = uri.substr(j);
	std::cout << BLUE << "NOVA URI: " << newUri << std::endl;
    // for (size_t i = 0; i < minlen; ++i)
	// {

        // if (this_location[i] == uri[j])
		// {
        //     commonSubstring += this_location[i];
		// 	j++;
        // }
    // }
	// std::cout << "-- PARTE A EXTRAIR PRA FORMAR O CAMINHO REAL: " << commonSubstring << std::endl;
	// retirar a commonSubstring do uri e juntar?
	// std::string uriSemParteEmComum = uri.substr(commonSubstring.length());
	std::string caminhoCompleto = "";
	if (uri != "/" && newUri != root)
		caminhoCompleto = root + newUri; // isso ja vai adicionar todo o caminho que vem da uri, se tiver?
	else
		caminhoCompleto = root;
	std::cout << "Caminho completo quando tem ROOT ESPECIFICA: " << caminhoCompleto << std::endl;
	return caminhoCompleto;
}
*/
bool Response::buildPathToResource(std::string root, Request &request, SocketS &server, std::map<std::string, std::vector< std::string > >& locationDirectives, std::map<std::string, LocationDirective>::iterator& it)
{
	std::string indexPage;
	std::string uri = request.getURI();
	std::vector<std::string> parts_uri = _utils.splitString(uri, '/');

	if (parts_uri.size() > 1)
	{
		// NESSE IF: a uri teve várias partes, exemplo: styles/styles.css, ou seja, precisamos ver se a uri tem algo
		// de comum com o root usado pra remover do path final, e juntar o resto
		std::string pathToResource = extractUriAndBuildPathToResource(root, parts_uri, uri, it);
		std::cout << YELLOW << "Caminho completo quando tem ROOT ESPECIFICA: " << pathToResource << END << std::endl;
		
		if (isDirectory(pathToResource)) {
			bool isErrorPageOrAutoIndexPage = isResponseADirectoryListingOrErrorPage(pathToResource, server, locationDirectives, it, indexPage);
			if (isErrorPageOrAutoIndexPage == true)
				return true;
    	} else {
    	    std::cout << YELLOW << pathToResource << " é um arquivo." << END << std::endl;
			setPath(pathToResource);
    	}
	}
	else
	{
		// NESSE ELSE: a uri NÃO teve várias partes, exemplo: about.html ou /images/ | /images
		// (ou seja, não precisa extrair alguma parte da uri, ou a uri é igual à location - e daí é só usar o root, ou basta juntá-la com o root)
		std::cout << "Tem só o arquivo de caminho ou só diretorio" << std::endl;
		std::string path;
		_uri = uri;
		if (uri == it->first || uri + '/' == it->first || it->first + '/' == uri) //essa terceira condição vai ferrar outros casos do site?
			path = root;
		else
			path = root + parts_uri[0];
		
		/* esse que tem que separar*/
		if (isDirectory(path))
		{
			//incluir a funcao para verificar se o caminhoexiste (acho que nao precisa aqui)
			bool isErrorPageOrAutoIndexPage = isResponseADirectoryListingOrErrorPage(path, server, locationDirectives, it, indexPage);
			if (isErrorPageOrAutoIndexPage == true)
				return true;
		}
		else //inserir condição de que existe (porque o isDirectory() retorna se nao existe mas tambem se teve algum erro), senao é pagina de erro
		{
			std::cout << path << " é um arquivo" << std::endl;
			setPath(path);
		}
		// mais um else pro caso de nao existir?
		// Domingo: testar auto_index on com o /assets!!!!!!!!!!
	}
	return false;
}

std::string Response::httpGet(Request &request, SocketS &server, Response *this_response)
{
    std::string root;
	root = server.getRoot();

	std::map<std::string, LocationDirective> serverLocations = server.getLocations();
	std::map<std::string, LocationDirective>::iterator it = this_response->findRequestedLocation(request, server, serverLocations);
	std::string uri = request.getURI();
    std::map<std::string, std::vector< std::string > > locationDirectives; //cria map p armazenar diretivas de localizacao

    if (it != serverLocations.end())
	{
        std::cout << "Location found! >> " << it->first << std::endl;
        locationDirectives = it->second.getDirectives(); //obtem as diretivas da location
        std::map<std::string, std::vector< std::string > >::iterator itRoot = locationDirectives.find("root"); //procura a dirtiva root
		if (itRoot != locationDirectives.end())
		{
            root = itRoot->second[0];
		}
		// 	// Domingo: testar auto_index on com o /assets!!!!!!!!!!
		bool isErrorPageOrAutoIndexPage = this_response->buildPathToResource(root, request, server, locationDirectives, it);
		if (isErrorPageOrAutoIndexPage == true)
			return this_response->getResponse();
		std::cout << "CAMINHO COMPLETO DO RECURSO: " << this_response->getPath() << std::endl;
        std::string bodyHTML = readFileToString(this_response->getPath()); //declara o corpo do HTML
		struct stat info;
		if (stat(this_response->getPath().c_str(), &info) != 0)
		{
			// retorna pagina de erro (o caminho pra pagina nao existe)
			this_response->errorCodeHtml(404, server);
			return this_response->getResponse();
		}
		this_response->generateResponse(200, request);
    }
	else
	{
        std::cout << "This server doesnt have this location!!" << std::endl;
        this_response->errorCodeHtml(404, server);
		return this_response->getResponse();
        //error404Html(response, root);
	}
	return this_response->getResponse();
}

void Response::buildResponse(Request &request, SocketS &server)
{
    /*
    	Implementação para construir a resposta completa, incluindo cabeçalho e corpo
    	Incluir os cabeçalhos necessários, como Content-Length e outros
    	Retorne a resposta completa construída
    */

	std::cout << YELLOW << "-------> CHEGAMOS AO BUILD RESPONSE <--------" << END << std::endl;
    // verificar se o método requisitado pela solicitação é permitido pra esse servidor
    std::vector<std::string> allowed_methods = server.getMethods();
    std::string requestMethod = request.getMethod();
    // std::cout << RED << requestMethod << END << std::endl;

	// -------------------------------
	// ver antes se tem allowed_methods interno da location...
	// se tiver, ele em precedencia..
	// se não, daí olha o do server...
    bool isAllowedMethod = false;

	std::map<std::string, LocationDirective> serverLocations = server.getLocations();
	std::map<std::string, LocationDirective>::iterator itLocation = findRequestedLocation(request, server, serverLocations);
	if (itLocation != serverLocations.end())
	{
		// procurar o allow_methods interno
		std::map<std::string, std::vector< std::string > > locationDirectives;
		locationDirectives = itLocation->second.getDirectives();
		std::map<std::string, std::vector< std::string > >::iterator itLocationAllowMethods = locationDirectives.find("allow_methods");
		if (itLocationAllowMethods != locationDirectives.end())
		{
			std::vector<std::string> locationAllowedMethods = itLocationAllowMethods->second;
			for (std::vector<std::string>::iterator it = locationAllowedMethods.begin(); it != locationAllowedMethods.end(); ++it)
			{
    		    if (strcmp(it->c_str(), requestMethod.c_str()) == 0)
				{
    		        isAllowedMethod = true;
    		        break;
    		    }
    		}
		}
		else
		{
			for (std::vector<std::string>::iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it)
			{
    		    if (strcmp(it->c_str(), requestMethod.c_str()) == 0)
				{
    		        isAllowedMethod = true;
    		        break;
    		    }
    		}
		}
	}
    
	std::string hasRoot;
    if (isAllowedMethod)
	{
		std::string resposta = _methodsFunctions[requestMethod](request, server, this);
		// std::cout << BLUE << " ***** A resposta é *****\n" << getResponse() << END << std::endl;
		/// consta a delete aqui
    }
    else
    {
        std::cout << "MÉTODO NÃO PERMITIDO!";
        // std::string response = "HTTP/1.1 404 Not found\r\nContent-Type: text/html\r\n\r\n<html><head></head><body><h1>Error 404</h1></body></html>";
        // setResponse(response);
		errorCodeHtml(405, server); // statusCode 405 = "Method Not Allowed"
    }
}

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

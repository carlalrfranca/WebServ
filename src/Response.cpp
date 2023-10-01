/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/18 18:00:34 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/30 23:59:55 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Response.hpp"

Response::Response() : _headers(), _body(""), _response(""), _code(""),
	_path("")
{
    _chosenSocket = NULL;
	_methodsFunctions["GET"] = &Response::httpGet;
	_methodsFunctions["POST"] = &Response::postMethod;
	_methodsFunctions["DELETE"] = &Response::deleteMethod;
    _statusMessages = StatusMessages();
}

Response::~Response()
{}

// Response::Response(Request request)
// {
//     // define corpo da resposta lendo o conteudo do arquivo html
//     // _body = readHtmlFile("web/html_upload_arquivo/index.html");
    
//     // precisa definir os campos do cabeçalho aqui, entao se for inserir mais É AQUI
//     // setStatusCode("200 OK");
//     // setContentType("text/html");
//     // setDateAndTime();
//     // setContentLength(_body.length());
//     _chosenSocket = NULL;
// }

std::string Response::postMethod(Request &request, SocketS &server, Response *this_response)
{
	// TO DO:
	/*
		ANTES DE TUDO:
		-> verificar o TAMANHO DO CONTEUDO DO POST (vê se está dentro dos limites de max-content
		determinado no arquivo de configuração. Se não estiver, corta na hora reornando uma
		RESPONSE DE ERRO DE ACORDO COM A SITUAÇÃO)
	*/
	
	// ---------------------------------------------------------------------------------
	std::string root_for_response = server.getRoot();
	std::map<std::string, LocationDirective> serverLocations = server.getLocations();
	std::map<std::string, LocationDirective>::iterator it = this_response->findRequestedLocation(request, serverLocations);
	std::string uri = request.getURI();
    std::map<std::string, std::vector< std::string > > locationDirectives;

	// daí cada um tem um fluxo
	/*
		- (depois tem que testar quando chega sem '/' no inicio, o httpGET que não consegue extrair a diferença da URI - aconteceu pra pedir o css do cgi com 'location cgi-bin')
		- tem que tratar uris que podem ter varios '/'? que nem extrair? (acho que sim ne, por praxe)
	*/
	if (it != serverLocations.end())
		std::cout << BLUE << "Location found! >> " << it->first << END << std::endl;
	std::string scriptName = this_response->extractScriptName(uri);
	if (scriptName.size() == 0)
	{
		std::cerr << "---- Opa, essa request NÃO tem process_data CGI !!! ----" << std::endl;
		this_response->errorCodeHtml(404, server);
		return this_response->getResponse();
	}
	
	if (it != serverLocations.end())
	{
		size_t foundCGIExt = scriptName.find(".cgi");
		if (foundCGIExt != std::string::npos)
			scriptName = scriptName.substr(0, foundCGIExt);
		// agora verificamos se há um "root" dentro desse location (um root mais "especifico", portanto)
		std::map<std::string, std::vector< std::string > > locationDirectives;
		locationDirectives = it->second.getDirectives();
        std::map<std::string, std::vector< std::string > >::iterator itLocationRoot = locationDirectives.find("root");
		if (itLocationRoot != locationDirectives.end())
			root_for_response = itLocationRoot->second[0];
		std::map<std::string, std::vector< std::string > >::iterator commandOfCGI = locationDirectives.find("cgi_path");
		std::map<std::string, std::vector< std::string > >::iterator CGIExtension = locationDirectives.find("cgi_ext");
		if (commandOfCGI == locationDirectives.end() || CGIExtension == locationDirectives.end())
		{
			this_response->errorCodeHtml(404, server);
			return this_response->getResponse();
		}
		// extrair extensoes e comandos pra executar os scripts
		std::vector<std::string> scriptsCommands = commandOfCGI->second;
		std::vector<std::string> scriptsExtensions = CGIExtension->second;
		std::string pathToScript = root_for_response + scriptName;
		try
		{
			CGI script(root_for_response, scriptsCommands, scriptsExtensions, scriptName);
			std::cout << "CAMINHO INTEIRO PARA O SCRIPT CONSTRUIDO >>" << YELLOW << script.getPathToScript() << END << std::endl;
			int resultCode = 0;
			resultCode = script.handleCGIRequest(request); //pode dar um retorno caso precise retornar página de erro?
			if (resultCode != 204)
			{
				this_response->errorCodeHtml(resultCode, server);
				return this_response->getResponse();
			}
			this_response->setResponse(script.getResponse());
			return script.getResponse(); // retorna a response
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			this_response->errorCodeHtml(404, server);
			return this_response->getResponse();
		}
	}
	this_response->errorCodeHtml(404, server);
	return this_response->getResponse();
}

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
	std::map<std::string, LocationDirective>::iterator it = this_response->findRequestedLocation(request, serverLocations);
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
		std::cout << BLUE << "Excluiu [aparentemente]" << END << std::endl;
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

const std::map<std::string, std::string>& Response::getHeader(void) const
{
    // static std::string emptyHeader = ""; // Cria uma string vazia para retorno padrão

    // Implementação para buscar e retornar o cabeçalho específico
    // Se o cabeçalho não existir, retorne a string vazia criada acima

    return _headers;
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
    std::string path = getPath();
	try
	{
		std::cout << "PATH> " << path << std::endl;
		DIR *directory = opendir(path.c_str());
		if (directory){
			struct dirent *entry;
			std::vector<std::string> fileList;
			while ((entry = readdir(directory)) != NULL)
				fileList.push_back(std::string(entry->d_name));
			closedir(directory);
			std::cout << "Tamanho da Lista de Itens do Diretório > " << fileList.size() << std::endl;
			// for (size_t i = 0; i < fileList.size(); ++i)
			// 	std::cout << i << " - " << fileList[i] << std::endl;
			generateHtmlFromFiles(fileList, it);
		}
		else
			throw std::runtime_error("Error when open directory.");
    } catch (const std::exception &e)
	{
        std::cout << "Error: " << e.what() << std::endl;
		// produz a pagina de erro 403 Forbidden (ou 404?)
    }
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
	setDateAndTime();
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
	response += "Date: " + getDate() + "\r\n";
	// response += "Connection: close\r\n\r\n" + errorHtml;
	response += "\r\n" + errorHtml;
	setResponse(response);
    // return response;
}

std::string Response::generateHeaders(int statusCode, const Request& request)
{
	std::string response;
	std::string contentType;	
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

	std::size_t headerEndPos = response.find("\r\n\r\n");

	// Calcule o tamanho do conteúdo subtraindo a posição do fim do cabeçalho do tamanho total
	std::size_t contentLength = response.length() - headerEndPos - 4;

	// Converta contentLength para uma string usando std::stringstream
	std::stringstream ss;
	ss << contentLength;
	std::string contentLenStr = ss.str();

	// Agora contentLenStr contém a representação de contentLength como uma string
	// std::cout << "Content-Length: " << contentLenStr << std::endl;
	// std::string contentLenStr = std::to_string(response.length() - response.find("\r\n\r\n") -4);

	setDateAndTime();
	std::string headers;
    headers += "HTTP/1.1 " + statusCodeStr + " OK\r\n";
    headers += "Content-Type: " + contentType + "\r\n";
    headers += "Content-Length: " + contentLenStr + "\r\n";
	headers += "Date: " + getDate() + "\r\n";
    headers += "\r\n"; // Linha em branco indica o fim dos cabeçalhos	


	return headers;
}

void Response::generateResponse(int statusCode, const Request& request)
{
    std::string content = readFileToString(getPath());
    std::string response;
	std::cout << " --------------------- URI ------------------- " << request.getURI() << std::endl;
	std::string headers = generateHeaders(statusCode, request);
	response += headers;
    response += content;
	
	setResponse(response);
	// std::cout << RED << "Response\n" << getResponse() << END << std::endl;
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

std::map<std::string, LocationDirective>::iterator Response::findRequestedLocation(Request &request, std::map<std::string, LocationDirective>& serverLocations)
{
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
		std::map<std::string, std::vector< std::string > >::iterator itAutoIndex = locationDirectives.find("auto_index");
		if (itAutoIndex != locationDirectives.end())
		{
			std::cout << BLUE << itAutoIndex->second[0] << END << std::endl;
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

std::string Response::extractUriAndBuildPathToResource(std::string root, std::vector<std::string>& parts_uri, std::string& uri, std::map<std::string, LocationDirective>::iterator& it)
{
	std::cout << "Tem caminho além do arquivo | Tamanho: " << parts_uri.size() << std::endl;
	std::cout << parts_uri[0] << " - " << parts_uri[0].size() << " e " << parts_uri[1] << std::endl;
	std::string this_location = it->first;
	std::string commonSubstring = "";
	if (!uri.empty() && uri[0] == '/')
		uri.erase(0, 1);
	size_t minlen = std::min(this_location.length(), uri.length());
	size_t j = 0;
	std::cout << BLUE << "Root | " << root << " URI: " << uri << END << std::endl;

    for (size_t i = 0; i < minlen; ++i) {
        if (this_location[i] == uri[j]) {
            commonSubstring += this_location[i];
			j++;
        }
    }
	std::cout << "-- PARTE A EXTRAIR PRA FORMAR O CAMINHO REAL: " << commonSubstring << std::endl;
	std::string uriSemParteEmComum = uri.substr(commonSubstring.length());
	std::string caminhoCompleto = "";
	if (uri != "/" && uri != root)
		caminhoCompleto = root + uriSemParteEmComum; // isso ja vai adicionar todo o caminho que vem da uri, se tiver?
	else
		caminhoCompleto = root;
	return caminhoCompleto;
}

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


std::string Response::buildHeaderReturn(std::string statusCode, std::string resource, Response *this_response)
{

	std::string headers;
	std::string codeMessage;
	codeMessage = this_response->_statusMessages.getMessage(atoi(statusCode.c_str()));
	this_response->setDateAndTime();
	
	headers += "HTTP/1.1 " + statusCode + "" + codeMessage + "\r\n";
    headers += "Location: " + resource + "\r\n";
    //headers += "Content-Type: " + contentType + "\r\n";
    //precisa adicionar o date of time
    headers += "Connection: close\r\n";
    headers += this_response->getDate() + "\r\n";
    headers += "\r\n"; // Linha em branco indica o fim dos cabeçalhos
    
	this_response->setResponse(headers);
	return this_response->getResponse();
}


std::string Response::extractScriptName(std::string& uri)
{
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
		return "";
	return scriptName;
}


std::string Response::httpGet(Request &request, SocketS &server, Response *this_response)
{
    std::string root;
	root = server.getRoot();
	std::map<std::string, LocationDirective> serverLocations = server.getLocations(); //obtem as diretivas de localizacao
	std::map<std::string, LocationDirective>::iterator it = this_response->findRequestedLocation(request, serverLocations);
	std::string uri = request.getURI();
    std::map<std::string, std::vector< std::string > > locationDirectives;
    if (it != serverLocations.end())
	{
        std::cout << "Location found! >> " << it->first << std::endl;
        locationDirectives = it->second.getDirectives();
        std::map<std::string, std::vector<std::string> >::iterator itReturn = locationDirectives.find("return");
        if(itReturn != locationDirectives.end())
        {
			std::string headerReturn = buildHeaderReturn(itReturn->second[0], itReturn->second[1], this_response);
            return this_response->getResponse();
        }
        /*
            tentando encontrar a return
            se encontrar a return chama o metodo que eu vou criar para lidar com a return que vai 
            setar essa response apropriada para o return
            ela tem uma header location que tem um recurso pra resirecionar
            ...
            no final precisa -->>	return this_response->getResponse();
        */
        std::map<std::string, std::vector< std::string > >::iterator itRoot = locationDirectives.find("root"); //procura a dirtiva root
		if (itRoot != locationDirectives.end())
		{
            root = itRoot->second[0];
		}
		bool isErrorPageOrAutoIndexPage = this_response->buildPathToResource(root, request, server, locationDirectives, it);
		if (isErrorPageOrAutoIndexPage == true)
			return this_response->getResponse();	
		std::cout << "**** CAMINHO COMPLETO DO RECURSO: " << this_response->getPath() << std::endl;
        // verificar se o location é cgi.. se for, encaminhar pra outra função
		// pra fazer o CGI do GET
		// se o location nao tem cgi, entao procurar se nao ha
		// essas diretivas dentro do location
		if (it->first.find("cgi") != std::string::npos && this_response->getPath().find(".cgi") != std::string::npos)
		{
			std::string scriptName = this_response->extractScriptName(uri);
			if (scriptName.size() == 0)
			{
				this_response->errorCodeHtml(404, server);
				return this_response->getResponse();
			}
			// agora, depois de resolver o scriptName,
			// temos que tirar a extensão .cgi pra poder substituir pela extensao correta
			size_t foundCGIExt = this_response->getPath().find(".cgi");
			std::string scriptPath;
			if (foundCGIExt != std::string::npos)
				scriptPath = this_response->getPath().substr(0, foundCGIExt);
			else
				std::cout << RED << "Isso não é pra script" << END << std::endl;
			std::cout << "**** CAMINHO DO SCRIPT PRA GET METHOD: " << scriptPath << std::endl;
			// vai precisar declarar um objeto CGI e se basear nesse trecho do método pro POST
			std::map<std::string, std::vector< std::string > >::iterator commandOfCGI = locationDirectives.find("cgi_path");
			std::map<std::string, std::vector< std::string > >::iterator CGIExtension = locationDirectives.find("cgi_ext");
			if (commandOfCGI == locationDirectives.end() || CGIExtension == locationDirectives.end())
			{
				this_response->errorCodeHtml(404, server);
				return this_response->getResponse();
			}
			std::vector<std::string> scriptsCommands = commandOfCGI->second;
			std::vector<std::string> scriptsExtensions = CGIExtension->second;
			CGI script;
			script.setCommands(scriptsCommands);
			script.setExtensions(scriptsExtensions);
			script.setRoot("");
			script.setPathToScript(scriptPath);
			std::cout << "**** CAMINHO A PARTIR DO OBJ SCRIPT: " << script.getPathToScript() << std::endl;
			int resultCode = 0;
			resultCode = script.handleCGIRequest(request);
			if (resultCode != 200)
			{
				this_response->errorCodeHtml(404, server);
				return this_response->getResponse();
			}
			std::string headers = this_response->generateHeaders(200, request);
			std::string bodyResponse = script.getResponse();
			std::string response = headers + bodyResponse;
			this_response->setResponse(response);
			return this_response->getResponse();
			// chama o handleCGI, ele vai retornar 200 e,
			// nisso, só precisa pegar o getResponse() do CGI
			// e retornar isso
		}
		
		// --------------------------------------------
		std::string bodyHTML = readFileToString(this_response->getPath()); //declara o corpo do HTML
		struct stat info;
		if (stat(this_response->getPath().c_str(), &info) != 0)
		{
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
	}
	return this_response->getResponse();
}

bool Response::isThisMethodAllowed(std::map<std::string, LocationDirective>& serverLocations, Request &request, SocketS &server, std::string& requestMethod)
{
	std::vector<std::string> allowed_methods = server.getMethods();
	std::map<std::string, LocationDirective>::iterator itLocation = findRequestedLocation(request, serverLocations);
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
    		        return true;
    		}
		}
		else
		{
			for (std::vector<std::string>::iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it)
			{
    		    if (strcmp(it->c_str(), requestMethod.c_str()) == 0)
					return true;
    		}
		}
	}
	return false;
}

void Response::buildResponse(Request &request, SocketS &server)
{
	std::cout << YELLOW << "-------> CHEGAMOS AO BUILD RESPONSE <--------" << END << std::endl;
    // verificar se o método requisitado pela solicitação é permitido pra esse servidor
    std::vector<std::string> allowed_methods = server.getMethods();
    std::string requestMethod = request.getMethod();
	std::map<std::string, LocationDirective> serverLocations = server.getLocations();
    if (isThisMethodAllowed(serverLocations, request, server, requestMethod))
	{
		std::string resposta = _methodsFunctions[requestMethod](request, server, this);
    }
    else
    {
        std::cerr << "MÉTODO NÃO PERMITIDO!" << std::endl;
		errorCodeHtml(405, server);
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

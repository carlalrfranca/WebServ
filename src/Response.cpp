/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/18 18:00:34 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/06 23:10:36 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Response.hpp"

Response::Response() : _headers(), _body(""), _response(""), _code(""), _path("")
{
    _chosenSocket = NULL;
	_methodsFunctions["GET"] = &Response::httpGet;
	_methodsFunctions["POST"] = &Response::postMethod;
	_methodsFunctions["DELETE"] = &Response::deleteMethod;
    _statusMessages = StatusMessages();
    _extensionToContentType = initContentTypes();
}

Response::~Response()
{}

std::string Response::postMethod(Request &request, SocketS &server, Response *this_response)
{
	std::string root_for_response = server.getRoot();
	std::map<std::string, LocationDirective> serverLocations = server.getLocations();
	std::map<std::string, LocationDirective>::iterator it = this_response->_utilsResponse.findRequestedLocation(request, serverLocations);
	std::string uri = request.getURI();
    std::map<std::string, std::vector< std::string > > locationDirectives;
	size_t maxBodySize = server.getMaxBodySize();
	// daí cada um tem um fluxo
	/*
		- (depois tem que testar quando chega sem '/' no inicio, o httpGET que não consegue extrair a diferença da URI - aconteceu pra pedir o css do cgi com 'location cgi-bin')
		- tem que tratar uris que podem ter varios '/'? que nem extrair? (acho que sim ne, por praxe)
	*/
	if(it != serverLocations.end())
		std::cout << BLUE << "Location found! >> " << it->first << END << std::endl;
	std::string scriptName = this_response->extractScriptName(uri);
	if(scriptName.size() == 0)
	{
		std::cerr << "---- Opa, essa request NÃO tem process_data CGI !!! ----" << std::endl;
		this_response->errorCodeHtml(404, server);
		return this_response->getResponse();
	}

	if(it != serverLocations.end())
	{
		size_t foundCGIExt = scriptName.find(".cgi");
		if(foundCGIExt != std::string::npos)
			scriptName = scriptName.substr(0, foundCGIExt);
		// agora verificamos se há um "root" dentro desse location (um root mais "especifico", portanto)
		std::map<std::string, std::vector< std::string > > locationDirectives;
		locationDirectives = it->second.getDirectives();
        std::map<std::string, std::vector< std::string > >::iterator itLocationRoot = locationDirectives.find("root");
		std::map<std::string, std::vector< std::string > >::iterator itLocationMaxBodySize = locationDirectives.find("client_max_body_size");
		if(itLocationMaxBodySize != locationDirectives.end())
			maxBodySize = UtilsResponse::strToSizeT(itLocationMaxBodySize->second[0]);
		size_t requestContentLength = UtilsResponse::strToSizeT(request.getContentLength());
		if(requestContentLength > maxBodySize)
		{
			std::cout << RED << "Content-Length da Request: " << requestContentLength << END << std::endl;
			std::cout << RED << "Max_Body_Size permitido: " << maxBodySize << END << std::endl;
			this_response->errorCodeHtml(413, server);
			return this_response->getResponse();
		}
		if(itLocationRoot != locationDirectives.end())
			root_for_response = itLocationRoot->second[0];
		std::map<std::string, std::vector< std::string > >::iterator commandOfCGI = locationDirectives.find("cgi_path");
		std::map<std::string, std::vector< std::string > >::iterator CGIExtension = locationDirectives.find("cgi_ext");
		if(commandOfCGI == locationDirectives.end() || CGIExtension == locationDirectives.end())
		{
			this_response->errorCodeHtml(404, server);
			return this_response->getResponse();
		}
		std::map<std::string, std::vector< std::string > >::iterator CGIUploadStore = locationDirectives.find("upload_store");
		std::string uploadStoreFolder;
		if(CGIUploadStore != locationDirectives.end())
		{
			uploadStoreFolder = CGIUploadStore->second[0];
			if(uploadStoreFolder[uploadStoreFolder.size() - 1] != '/')
				uploadStoreFolder = uploadStoreFolder + '/';
		}
		else
			uploadStoreFolder = "./";
		// extrair extensoes e comandos pra executar os scripts
		std::vector<std::string> scriptsCommands = commandOfCGI->second;
		std::vector<std::string> scriptsExtensions = CGIExtension->second;
		std::string pathToScript = root_for_response + scriptName;
		try
		{
			CGI script(root_for_response, scriptsCommands, scriptsExtensions, scriptName);
			script.setUploadStoreFolder(uploadStoreFolder);
			std::cout << "CAMINHO INTEIRO PARA O SCRIPT CONSTRUIDO >>" << YELLOW << script.getPathToScript() << END << std::endl;
			int resultCode = 0;
			resultCode = script.handleCGIRequest(request); //pode dar um retorno caso precise retornar página de erro?
			if(resultCode != 204)
			{
				this_response->errorCodeHtml(resultCode, server);
				return this_response->getResponse();
			}
			this_response->setResponse(script.getResponse());
			return this_response->getResponse(); // retorna a response
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
	
	// 1. extrair a uri da solicitacao HTTP recebida p identificar o recurso que deve ser excluído
	// std::string uri = request.getURI();
	// construir o caminho do recurso
	// std::string caminhoDoRecurso = //constroi aqui
	
std::string Response::deleteMethod(Request &request, SocketS &server, Response *this_response)
{
	std::map<std::string, LocationDirective> serverLocations = server.getLocations();
	std::map<std::string, LocationDirective>::iterator it = this_response->_utilsResponse.findRequestedLocation(request, serverLocations);
	std::string uri = request.getURI();
	if(it != serverLocations.end())
		std::cout << BLUE << "Location found! >> " << it->first << END << std::endl;
	std::cout << BLUE << "URI >>> " << uri << END << std::endl;
	std::string root;
	if(server.getRoot().size() > 0)
		root = server.getRoot();
	std::map<std::string, std::vector< std::string > > locationDirectives;
	locationDirectives = it->second.getDirectives(); //obtem as diretivas da location
	std::map<std::string, std::vector< std::string > >::iterator itRoot = locationDirectives.find("root"); //procura a dirtiva root
	if(itRoot != locationDirectives.end())
		root = itRoot->second[0];
	if (root[root.size() - 1] != '/')
		root = root + '/';
	if (request.getIsDeleteMaskedAsPost())
		this_response->setPath(root + request.getFilename());
	else
		this_response->buildPathToResource(root, request, server, locationDirectives, it);
	std::string resourcePath = this_response->getPath();
	if(this_response->_utils.fileExists(resourcePath) == false)
	{
		this_response->errorCodeHtml(404, server);
		return this_response->getResponse();
	}
	if(remove(resourcePath.c_str()) == 0)
		this_response->generateResponse(204, request); //exclusão bem-sucedida
	else
		this_response->errorCodeHtml(500, server);
	return this_response->getResponse();
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
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", timeInfo);
    _headers["Date"] = buffer;
}

std::string Response::getDate()const
{
	std::map<std::string, std::string >::const_iterator it = _headers.find("Date");

	if(it != _headers.end())
		return it->second;
	return "Error: Date not found";
}

void Response::setResponse(const std::string& response)
{
    _response = response;
}

const std::map<std::string, std::string>& Response::getHeader() const
{
    // static std::string emptyHeader = ""; // Cria uma string vazia para retorno padrão

    // Implementação para buscar e retornar o cabeçalho específico
    // Se o cabeçalho não existir, retorne a string vazia criada acima

    return _headers;
}

std::string Response::getUri()const
{
	return _uri;
}

void Response::setUri(const std::string& uri)
{
	_uri = uri;
}

std::string Response::getResponse()const
{
    return _response;
}

const std::string Response::getPath()const
{
    return _path;
}

void Response::setPath(const std::string& allPath)
{
    _path = allPath;
}

bool Response::listFilesAndGenerateHtml(std::map<std::string, LocationDirective>::iterator& it)
{
    std::string path = getPath();
	try
	{
		std::cout << "PATH> " << path << std::endl;
		DIR *directory = opendir(path.c_str());
		if(directory)
		{
			struct dirent *entry;
			std::vector<std::string> fileList;
			while((entry = readdir(directory)) != NULL)
				fileList.push_back(std::string(entry->d_name));
			closedir(directory);
			std::cout << "Tamanho da Lista de Itens do Diretório > " << fileList.size() << std::endl;
			// for (size_t i = 0; i < fileList.size(); ++i)
			// 	std::cout << i << " - " << fileList[i] << std::endl;
			generateHtmlFromFiles(fileList, it);
		} else
			throw ErrorException("Error when open directory.");
    }catch(const std::exception &e)
	{
        std::cout << "Error: " << e.what() << std::endl;
		return false;
    }
	return true;
}

void Response::generateHtmlFromFiles(const std::vector<std::string>& fileList, std::map<std::string, LocationDirective>::iterator& it)
{
    std::string html;
	std::string location = it->first;
	std::string path = getPath();
	std::cout << "Path: " << path << std::endl;

	if(!location.empty() && location[location.size() - 1] != '/')
        location += '/';
    html += "<html>\n";
    html += "<head><title></title></head>\n";
    html += "<body>\n";
    html += "<h1>File List</h1>\n";
    html += "<ul>\n";
	if(!getUri().empty() && getUri()[getUri().size() - 1] != '/')
	{
		std::string newUri = getUri();
       	newUri += '/';
		setUri(newUri);
	}
    for(size_t i = 0; i < fileList.size(); ++i)
	{
		// ver se é um arquivo ou um diretorio, se for um diretorio, tem que ser EM FORMA DE LINK
		std::string itemName = fileList[i];
		bool isDir = _utilsResponse.isDirectory(path + itemName);
		//html += "<li>" + fileList[i] + "</li>\n";
		if(isDir)
		{
			if(!itemName.empty() && itemName[itemName.size() - 1] != '/')
        		itemName += '/';
			std::cout << RED << "É diretorio: " << getUri() + itemName << END << std::endl;
			html += "<li><a href='" + getUri() + itemName + "'>" + itemName + "</a></li>\n";
		} else {
			std::cout << YELLOW << "É arquivo: " << getUri() + itemName << END << std::endl;
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
	setDateAndTime();
    std::string errorHtml = UtilsResponse::readFileToString(errorFileName);
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
}

std::map<std::string, std::string> Response::initContentTypes()
{
	std::map<std::string, std::string> contentType;
    contentType["txt"]  = "text/css";
    contentType["html"] = "text/html";
    contentType["css"]  = "text/css";
    contentType["png"]  = "images/png";
    contentType["jpg"]  = "images/jpeg";
    contentType["jpeg"] = "images/jpeg";
    contentType["gif"]  = "images/gif";
    //contentType["json"] = "application/json";
    //contentType["xml"]  = "application/xml";
    //contentType["pdf"]  = "application/pdf";
    //contentType["zip"]  = "application/zip";
    //contentType["gzip"] = "application/gzip";
    //contentType["tar"]  = "application/x-tar";

    return (contentType);
}

std::string Response::getContentTypeFromExtension(const std::string& extension)
{
    std::map<std::string, std::string>::iterator it = _extensionToContentType.find(extension);
    if (it != _extensionToContentType.end()) {
        return it->second;
    }
    return "text/html";
}

std::string Response::generateHeaders(int statusCode, const Request& request, std::string content)
{
	std::string response = content;
	std::stringstream toConvertToString;
	
    toConvertToString << statusCode;
    std::string statusCodeStr = toConvertToString.str();
	std::size_t headerEndPos = response.find("\r\n\r\n");
	std::size_t contentLength = response.length() - headerEndPos - 4;
	std::stringstream ss;
	ss << contentLength;
	std::string contentLenStr = ss.str();
	setDateAndTime();
	
	std::string uri = request.getURI();
    size_t dotPos = uri.rfind('.');
	std::string contentType = "text/html";
	
	if (dotPos != std::string::npos) 
	{
        std::string extension = uri.substr(dotPos + 1);
        contentType = getContentTypeFromExtension(extension);
    }
	
	std::string headers;
    headers += "HTTP/1.1 " + statusCodeStr + " OK\r\n";
    headers += "Content-Type: " + contentType + "\r\n";
    headers += "Content-Length: " + contentLenStr + "\r\n";
	headers += "Date: " + getDate() + "\r\n";
	headers += "Server: Webserv-42SP\r\n";	
	
	if(request.getURI().find("chromium") != std::string::npos)
	{
		headers += "Access-Control-Allow-Origin: *\r\n";
        //headers += "Access-Control-Allow-Methods: GET, POST, DELETE\r\n";
        headers += "Access-Control-Allow-Headers: Content-Type\r\n";
        headers += "Cache-Control: no-cache, no-store\r\n";
	}
    headers += "\r\n";	
    
	return headers;
}

void Response::generateResponse(int statusCode, const Request& request)
{
    std::string content = UtilsResponse::readFileToString(getPath());
    std::string response;
	std::cout << " --------------------- URI ------------------- " << request.getURI() << std::endl;
	std::string headers = generateHeaders(statusCode, request, content);
	response += headers;
    response += content;
	setResponse(response);
}

bool Response::isResponseADirectoryListingOrErrorPage(std::string path, SocketS &server, std::map<std::string, std::vector< std::string > >& locationDirectives, std::map<std::string, LocationDirective>::iterator& it, std::string indexPage)
{
	bool isDirectoryAllowed;
	std::cout << BLUE << "LOCAAAAATION>>> " << it->first << END << std::endl;
	locationDirectives = it->second.getDirectives();
       std::map<std::string, std::vector< std::string > >::iterator itIndex = locationDirectives.find("index");
    if(itIndex != locationDirectives.end()) //não é o oposto? primeiro ve que se tem autoindex, se nao tiver (ou tiver off?) ve se tem index e serve se tiver? (ou gera pagina de erro se nao?)
		indexPage = itIndex->second[0];
	else if(server.getIndexFile().size() > 0)
		indexPage = server.getIndexFile();
	else
	{
		std::map<std::string, std::vector< std::string > >::iterator itAutoIndex = locationDirectives.find("auto_index");
		if(itAutoIndex != locationDirectives.end())
		{
			std::cout << BLUE << itAutoIndex->second[0] << END << std::endl;
			if(itAutoIndex->second[0] == "on")
			{
				setPath(path);
				isDirectoryAllowed = listFilesAndGenerateHtml(it);
				if(!isDirectoryAllowed)
					errorCodeHtml(403, server);
				return true;
			} else {
				errorCodeHtml(403, server);
				return true;
			}
		} else {
			errorCodeHtml(404, server);
			return true;
		}
	}
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
	if(!uri.empty() && uri[0] == '/')
		uri.erase(0, 1);
	size_t minlen = std::min(this_location.length(), uri.length());
	size_t j = 0;
	std::cout << BLUE << "Root | " << root << " URI: " << uri << END << std::endl;

    for(size_t i = 0; i < minlen; ++i)
	{
        if(this_location[i] == uri[j])
		{
            commonSubstring += this_location[i];
			j++;
        }
    }
	std::cout << "-- PARTE A EXTRAIR PRA FORMAR O CAMINHO REAL: " << commonSubstring << std::endl;
	std::string uriClear = uri.substr(commonSubstring.length());
	std::string wholePath = "";
	if(uri != "/" && uri != root)
		wholePath = root + uriClear; // isso ja vai adicionar todo o caminho que vem da uri, se tiver?
	else
		wholePath = root;
	return wholePath;
}

bool Response::buildPathToResource(std::string root, Request &request, SocketS &server, std::map<std::string, std::vector< std::string > >& locationDirectives, std::map<std::string, LocationDirective>::iterator& it)
{
	std::string indexPage;
	std::string uri = request.getURI();
	std::vector<std::string> parts_uri = _utils.splitString(uri, '/');

	if (parts_uri.size() > 1)
	{
		std::string pathToResource = extractUriAndBuildPathToResource(root, parts_uri, uri, it);
		std::cout << YELLOW << "Caminho completo quando tem ROOT ESPECIFICA: " << pathToResource << END << std::endl;
		
		if(_utilsResponse.isDirectory(pathToResource))
		{
			bool isErrorPageOrAutoIndexPage = isResponseADirectoryListingOrErrorPage(pathToResource, server, locationDirectives, it, indexPage);
			if(isErrorPageOrAutoIndexPage == true)
				return true;
    	} else {
    	    std::cout << YELLOW << pathToResource << " é um arquivo." << END << std::endl;
			setPath(pathToResource);
    	}
	} else {
		std::cout << "Tem só o arquivo de caminho ou só diretorio" << std::endl;
		std::string path;
		setUri(uri);
		if(getUri() == it->first || getUri() + '/' == it->first || it->first + '/' == getUri()) //essa terceira condição vai ferrar outros casos do site?
			path = root;
		else
			path = root + parts_uri[0];
		if(_utilsResponse.isDirectory(path))
		{
			bool isErrorPageOrAutoIndexPage = isResponseADirectoryListingOrErrorPage(path, server, locationDirectives, it, indexPage);
			if(isErrorPageOrAutoIndexPage == true)
				return true;
		} else {
			std::cout << path << " é um arquivo" << std::endl;
			setPath(path);
		}
	}
	return false;
}

std::string Response::buildHeaderReturn(std::string statusCode, std::string resource, Response *this_response)
{
	std::string headers;
	std::string codeMessage;
	codeMessage = this_response->_statusMessages.getMessage(atoi(statusCode.c_str()));
	this_response->setDateAndTime();
	headers += "HTTP/1.1 " + statusCode + " " + codeMessage + "\r\n";
    headers += "Location: " + resource + "\r\n";
    headers += "Connection: close\r\n";
    headers += this_response->getDate() + "\r\n";
    headers += "\r\n";
	this_response->setResponse(headers);
	std::cout << BLUE << this_response->getResponse() << END << std::endl;
	return this_response->getResponse();
}


std::string Response::extractScriptName(std::string& uri)
{
	size_t foundLastSlash = uri.find_last_of('/');
	std::string scriptName;
	if(foundLastSlash != std::string::npos && foundLastSlash != (uri.size() - 1))
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
	if(found == std::string::npos)
		return "";
	return scriptName;
}

std::string Response::httpGet(Request &request, SocketS &server, Response *this_response)
{
    std::string root;
	root = server.getRoot();
	std::map<std::string, LocationDirective> serverLocations = server.getLocations(); //obtem as diretivas de localizacao
	std::map<std::string, LocationDirective>::iterator it = this_response->_utilsResponse.findRequestedLocation(request, serverLocations);
	std::string uri = request.getURI();
    std::map<std::string, std::vector< std::string > > locationDirectives;
    if(it != serverLocations.end())
	{
        std::cout << "Location found! >> " << it->first << std::endl;
        locationDirectives = it->second.getDirectives();
        std::map<std::string, std::vector<std::string> >::iterator itReturn = locationDirectives.find("return");
        if(itReturn != locationDirectives.end())
        {
			std::string headerReturn = buildHeaderReturn(itReturn->second[0], itReturn->second[1], this_response);
            return this_response->getResponse();
        }
        std::map<std::string, std::vector< std::string > >::iterator itRoot = locationDirectives.find("root"); //procura a dirtiva root
		if(itRoot != locationDirectives.end())
            root = itRoot->second[0];
		bool isErrorPageOrAutoIndexPage = this_response->buildPathToResource(root, request, server, locationDirectives, it);
		if(isErrorPageOrAutoIndexPage == true)
			return this_response->getResponse();	
		std::cout << "**** CAMINHO COMPLETO DO RECURSO: " << this_response->getPath() << std::endl;
        // verificar se o location é cgi.. se for, encaminhar pra outra função
		// pra fazer o CGI do GET
		// se o location nao tem cgi, entao procurar se nao ha
		// essas diretivas dentro do location
		if(it->first.find("cgi") != std::string::npos && this_response->getPath().find(".cgi") != std::string::npos)
		{
			std::string scriptName = this_response->extractScriptName(uri);
			if(scriptName.size() == 0)
			{
				this_response->errorCodeHtml(404, server);
				return this_response->getResponse();
			}
			// agora, depois de resolver o scriptName,
			// temos que tirar a extensão .cgi pra poder substituir pela extensao correta
			size_t foundCGIExt = this_response->getPath().find(".cgi");
			std::string scriptPath;
			if(foundCGIExt != std::string::npos)
				scriptPath = this_response->getPath().substr(0, foundCGIExt);
			else
				std::cout << RED << "Isso não é pra script" << END << std::endl;
			std::cout << "**** CAMINHO DO SCRIPT PRA GET METHOD: " << scriptPath << std::endl;
			// vai precisar declarar um objeto CGI e se basear nesse trecho do método pro POST
			// esse trecho abaixo é muito parecido com o do POST...
			// tem como generalizar isso num método próprio que será chamado tanto aqui
			// quanto no POST? (menos a parte de atribuir "./" ao upload store, porque isso pode ser um problema)
			// tem que ser o web/images <- e verificar se existe, se não existe, criar.
			std::map<std::string, std::vector< std::string > >::iterator commandOfCGI = locationDirectives.find("cgi_path");
			std::map<std::string, std::vector< std::string > >::iterator CGIExtension = locationDirectives.find("cgi_ext");
			if(commandOfCGI == locationDirectives.end() || CGIExtension == locationDirectives.end())
			{
				this_response->errorCodeHtml(404, server);
				return this_response->getResponse();
			}
			std::map<std::string, std::vector< std::string > >::iterator CGIUploadStore = locationDirectives.find("upload_store");
			std::string uploadStoreFolder;
			if(CGIUploadStore != locationDirectives.end())
			{
				uploadStoreFolder = CGIUploadStore->second[0];
				if(uploadStoreFolder[uploadStoreFolder.size() - 1] != '/')
					uploadStoreFolder = uploadStoreFolder + '/';
			}
			else
				uploadStoreFolder = "web/images/";
			std::vector<std::string> scriptsCommands = commandOfCGI->second;
			std::vector<std::string> scriptsExtensions = CGIExtension->second;
			CGI script;
			script.setUploadStoreFolder(uploadStoreFolder);
			script.setCommands(scriptsCommands);
			script.setExtensions(scriptsExtensions);
			script.setRoot("");
			script.setPathToScript(scriptPath);
			std::cout << "**** CAMINHO A PARTIR DO OBJ SCRIPT: " << script.getPathToScript() << std::endl;
			int resultCode = 0;
			resultCode = script.handleCGIRequest(request);
			if(resultCode != 200)
			{
				this_response->errorCodeHtml(404, server);
				return this_response->getResponse();
			}
			std::string bodyResponse = script.getResponse();
			std::string headers = this_response->generateHeaders(200, request, bodyResponse);
			std::string response = headers + bodyResponse;
			this_response->setResponse(response);
			return this_response->getResponse();
			// chama o handleCGI, ele vai retornar 200 e,
			// nisso, só precisa pegar o getResponse() do CGI
			// e retornar isso
		}
		std::string bodyHTML = UtilsResponse::readFileToString(this_response->getPath()); //declara o corpo do HTML
		struct stat info;
		if(stat(this_response->getPath().c_str(), &info) != 0)
		{
			this_response->errorCodeHtml(404, server);
			return this_response->getResponse();
		}
		this_response->generateResponse(200, request);
    } else {
        std::cout << "This server doesnt have this location!!" << std::endl;
        this_response->errorCodeHtml(404, server);
		return this_response->getResponse();
	}
	return this_response->getResponse();
}

void Response::buildResponse(Request &request, SocketS &server)
{
	std::cout << YELLOW << "-------> CHEGAMOS AO BUILD RESPONSE <--------" << END << std::endl;
    // verificar se o método requisitado pela solicitação é permitido pra esse servidor
    std::vector<std::string> allowed_methods = server.getMethods();
    std::string requestMethod = request.getMethod();
	std::map<std::string, LocationDirective> serverLocations = server.getLocations();
    if(_utilsResponse.isThisMethodAllowed(serverLocations, request, server, requestMethod))
		std::string resposta = _methodsFunctions[requestMethod](request, server, this);
    else {
        std::cerr << "MÉTODO NÃO PERMITIDO!" << std::endl;
		errorCodeHtml(405, server);
    }
}

void Response::reset()
{
    _headers.clear();
    _body.clear();
    _code = "";
}

void Response::httpError(std::string errorCode, const std::string &errorMessage)
{
    _code = errorCode;
    _response = errorMessage;
    _headers.clear();
}

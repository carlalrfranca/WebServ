/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/24 18:26:41 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/02 20:54:39 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Request.hpp"

Request::Request(const std::string& request)
{
    _request = request;
    _hasError = false;
	_method = "";
    _hostLine = "";
    _hostContent = "";
    _portRequest = "";
	_contentLength = "";
	_contentType = "";
    _domainContent = "";
    _requestStream.str(request);
	_statusMessages = StatusMessages();
	_errorPage["503"] = "./web/error/Error503.html";
	_errorPage["400"] = "./web/error/Error400.html";
	_errorPage["403"] = "./web/error/Error403.html";
	_errorPage["413"] = "./web/error/Error413.html";
	_errorPage["405"] = "./web/error/Error405.html";
	_errorPage["505"] = "./web/error/Error505.html";
	_errorPage["504"] = "./web/error/Error504.html";
	_errorPage["500"] = "./web/error/Error500.html";
	// talvez aqui tenha que abrir pra um metodo
	// que PARSEIA E VALIDA A REQUEST TODA
    std::getline(_requestStream, _firstLine);
	// -------------
	size_t headerEndPos = _request.find("\r\n\r\n");
	std::string body;
	std::string headers;
	headers = _request.substr(0, headerEndPos + 4);
	_header = headers;
	if (headerEndPos != std::string::npos)
	{
		body = _request.substr(headerEndPos + 4);
		setBody(body);
		// std::cout << RED << "******************" << END << std::endl;
		// std::cout << RED << body << " | Body Size: " << body.size() << END << std::endl;
		// std::cout << RED << "******************" << END << std::endl;
		size_t methodDelete = body.find("_method=DELETE");
		if (methodDelete != std::string::npos)
			setMethod("DELETE");
		// tambem resgatar o valor do "imagemSelecionada"
	}
}

Request::~Request()
{}

void Request::setRequest(const std::string& request)
{
    _request = request;
    _requestStream.str(request);
    std::getline(_requestStream, _firstLine);
}

std::string Request::getRequest(void) const
{
	return _request;
}

void Request::printRequest()
{
    std::istringstream iss(_request);
    std::string line;
    while (std::getline(iss, line) && line != "\r")
        std::cout << line << std::endl;
}

void Request::setMethod(const std::string& method)
{
    _method = method;
}

void Request::setHasError(bool hasError)
{
    _hasError = hasError;
}

void Request::setHeader(const std::string& header)
{
    _header = header;
}

void Request::setBody(const std::string& body)
{
    _body = body;
}

void Request::setContentLength(const std::string& contentLength)
{
    _contentLength = contentLength;
}

void Request::setContentType(const std::string& contentType)
{
    _contentType = contentType;
}

std::string Request::getContentLength()const        
{
    return _contentLength;
}

std::string Request::getContentType()const
{
    return _contentType;
}

std::string Request::getHeader()const
{
    return _header;
}

std::string Request::getBody()const
{
    return _body;
}

bool Request::getHasError() const
{
    return _hasError;
}

std::string Request::getDomainRequest(void)const
{
    return _domainContent;
}

std::string Request::getPortRequest(void)const
{
    return _portRequest;
}

void Request::setBoundary(const std::string& boundary)
{
    _boundary = boundary;
}

void Request::setFilename(const std::string& filename)
{
    _filename = filename;
}

std::string Request::getBoundary()const
{
    return _boundary;
}

std::string Request::getFilename()const
{
    return _filename;
}

int Request::isFirstLineValid()
{
	std::istringstream firstLineStream(_firstLine);
    std::vector<std::string> _tokens;
    std::string _token;
    // separa os tokens da primeira linha
	while (std::getline(firstLineStream, _token, ' '))
	{
		_utils.trimSpaces(_token);
        _tokens.push_back(_token);
	}
    for (size_t i = 0; i < _tokens.size(); ++i)
        std::cout << "Token " << i << " (da primeira Linha da Request): " << _tokens[i] << " | Tamanho: " << _tokens[i].size() << std::endl;
    // valida os tokens da primeira linha
	if (_tokens.size() != 3)
		return 400; // retorna 400 (Bad Request)
	if (_tokens[0] != "GET" && _tokens[0] != "POST" && _tokens[0] != "DELETE")
		return 501; // 501 Not Implemented
	if (_tokens[2] != "HTTP/1.1")
		return 505; //505 (HTTP Version Not Supported)
	if (_method.size() == 0)
    	_method = _tokens[0];
	_uri = _tokens[1];
    _version = _tokens[2];
	std::cout << BLUE << "Primeira linha DIVIDIDA > " << _method << " | " << _uri << " | " << _version << END << std::endl;
	//valida http
    // if(_firstLine.find("GET") == std::string::npos && 
        // _firstLine.find("POST") == std::string::npos && 
        //  _firstLine.find("DELETE") == std::string::npos)
        // return 405; // 405 Method Not Allowed
    //verifica se tem espaço após o metodo
    // size_t spacePos = _firstLine.find(' ');
    // if(spacePos == std::string::npos || spacePos == _firstLine.size() - 1 )
        // return 400; //dar returns em vez de exceptions?
    //valida HTTP após o metodo e espaço
    // std::string version = _firstLine.substr(spacePos + 1);
    // if(version.find("HTTP/1.1") == std::string::npos)
        // return 505; //pagina de status 505 (HTTP Version Not Supported)
    //valida espaço apos a versão
    // spacePos = version.find(' ');
    // if (spacePos == std::string::npos || spacePos == version.size() - 1)
        // return 400; // retorna 400 (Bad Request)
    //demais validacoes aqui, se houver
    return SUCCESS;
}

/*
    FLEXIBILIZAÇÃO] separar "boundary=" e "filename=" no 
    método validateRequest() do objeto Request (dentro da 
    condição que verifica se o _method é POST). LEMBRE:
    "boundary=" e "filename=" estarão no _body, então é ele
    que tem que parsear
*/

int Request::validateRequest()
{
    // std::istringstream firstLineStream(_firstLine);
    // std::vector<std::string> _tokens;
    // std::string _token;
    // while (std::getline(firstLineStream, _token, ' '))
    //     _tokens.push_back(_token);
    // for (size_t i = 0; i < _tokens.size(); ++i)
    //     std::cout << "Token " << i << " (da primeira Linha da Request): " << _tokens[i] << std::endl;
    int result = isFirstLineValid();
	if (result != SUCCESS)
		return result;
	std::string line;
	std::istringstream headerStream(_header);
	while (std::getline(headerStream, line))
    {
        if (line.substr(0, 6) == "Host: ")
        {
			_hostLine = line;
            _hostContent = _hostLine.substr(6);
            std::cout << "Host content: " << _hostContent << std::endl;
            // break;
        }
		if (line.substr(0, 16) == "Content-Length: ")
		{
			_contentLength = line.substr(16);
			std::cout << "Content-Length na REQUEST: " << _contentLength << std::endl;
		}
		if (line.substr(0, 14) == "Content-Type: ")
		{
			_contentType = line.substr(14);
			std::cout << "Content-Type na REQUEST: " << _contentType << std::endl;
		}
    }
	if (_hostContent.size() == 0)
		errorCodeHtml(400);//return 400; // retorna 400 (Bad Request)
	if (_method == "POST")
	{
		if (_contentLength.size() == 0 || _contentType.size() == 0)
		{
		    errorCodeHtml(400);//return 400; //retorna 400 (Bad Request)
		}
	    //-------------- atualizacao 02.10.2023 ----------------
	    // parsear : procurar a posicao do boundary
	    size_t boundPos = _body.find("boundary=");
	    if(boundPos != std::string::npos)
        {
            boundPos += 9; //tamanho do boundary=
            size_t boundEnd = _body.find('&', boundPos); //encontra a próxima referência após a substring "filename="
            std::string boundary = _body.substr(boundPos, boundEnd - boundPos); //aqui ele estrai e armazena em boundary
            setBoundary(boundary);
            size_t filenamePos = _body.find("filename");
            if(filenamePos != std::string::npos)
            {
                filenamePos += 9; //tamanho de filename=
                size_t filenameEnd = _body.find('&', filenamePos);
                std::string filename = _body.substr(filenamePos, filenameEnd - filenamePos);
                setFilename(filename);
                std::cout << RED << "Boundary: " << boundary << END << std::endl;
                std::cout << RED << "Filename: " << filename << END << std::endl;
            }
        }
	}
    size_t posDiv = _hostContent.find(":");
    if(posDiv != std::string::npos) //127.0.0.1:5005  ou localhost:2005
    {
        _domainContent = _hostContent.substr(0, posDiv);
        _portRequest = _hostContent.substr(posDiv+1, (_hostContent.size() - 1));
        _utils.trimSpaces((_portRequest));
    } else {
        _domainContent = _hostContent;
        _portRequest = "";
    }  
    return SUCCESS;
}

const std::string& Request::getMethod(void) const
{
    return _method;
}

const std::string& Request::getURI(void) const
{
    return _uri;
}

const std::string& Request::getVersion(void) const
{
    return _version;
}

const std::string Request::getErrorPage(int statusCode)const
{
	std::stringstream toConvertToStr;
    toConvertToStr << statusCode;
    std::string statusCodeStr = toConvertToStr.str();
	std::map<std::string, std::string>::const_iterator it = _errorPage.find(statusCodeStr);
	if (it != _errorPage.end())
		return it->second;
	return "";
}

std::string Request::getDateAndTime()const
{
    char buffer[80];
    time_t rawTime;
    struct tm *timeInfo;
    
    time(&rawTime);
    timeInfo = localtime(&rawTime);
    
    //string info 
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", timeInfo);
    std::string date(buffer);
	return date;
}

std::string Request::errorCodeHtml(int statusCode)
{
    std::string errorFileName;
    std::string errorDefault;
    std::string date;
	errorFileName = getErrorPage(statusCode); //caminho
	std::cout << "Error File Name Path: " << errorFileName << std::endl;
    errorDefault = _statusMessages.getMessage(statusCode); //mensagm
	date = getDateAndTime();
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
	response += "Date: " + date + "\r\n";
	// response += "Connection: close\r\n\r\n" + errorHtml;
	response += "\r\n" + errorHtml;
	return response;
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/24 18:26:41 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/29 22:03:05 by cleticia         ###   ########.fr       */
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
    _domainContent = "";
    _requestStream.str(request);
    std::getline(_requestStream, _firstLine);
	// -------------
	size_t headerEndPos = _request.find("\r\n\r\n");
	std::string body;
	if (headerEndPos != std::string::npos)
	{
		body = _request.substr(headerEndPos + 4);
		setBody(body);
		std::cout << RED << "******************" << END << std::endl;
		std::cout << RED << body << " | Body Size: " << body.size() << END << std::endl;
		std::cout << RED << "******************" << END << std::endl;
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

bool Request::isFirstLineValid()
{
    //valida http
    if(_firstLine.find("GET") == std::string::npos && 
        _firstLine.find("POST") == std::string::npos && 
         _firstLine.find("DELETE") == std::string::npos)
        throw ErrorException(); // statusCode de not allowed method?
    //verifica se tem espaço após o metodo
    size_t spacePos = _firstLine.find(' ');
    if(spacePos == std::string::npos || spacePos == _firstLine.size() - 1 )
        throw ErrorException();
    //valida HTTP após o metodo e espaço
    std::string version = _firstLine.substr(spacePos + 1);
    if(version.find("HTTP/1.1") == std::string::npos)
        throw ErrorException(); //pagina de status 505
    //valida espaço apos a versão
    spacePos = version.find(' ');
    if (spacePos == std::string::npos || spacePos == version.size() - 1)
        throw ErrorException(); 
    //demais validacoes aqui, se houver
    return true;
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

bool Request::validateRequest()
{
    std::istringstream firstLineStream(_firstLine);
    std::vector<std::string> _tokens;
    std::string _token;
    while (std::getline(firstLineStream, _token, ' '))
        _tokens.push_back(_token);
    for (size_t i = 0; i < _tokens.size(); ++i)
        std::cout << "Token " << i << ": " << _tokens[i] << std::endl;
    while (std::getline(_requestStream, _hostLine))
    {
        if (_hostLine.substr(0, 6) == "Host: ")
        {
            _hostContent = _hostLine.substr(6);
            std::cout << "Host content: " << _hostContent << std::endl;
            break;
        }
    }
	if (_method.size() == 0)
    	_method = _tokens[0];
    _uri = _tokens[1];
    _version = _tokens[2];
    size_t posDiv = _hostContent.find(":");
    if(posDiv != std::string::npos) //127.0.0.1:5005  ou localhost:2005
    {
        _domainContent = _hostContent.substr(0, posDiv);
        _portRequest = _hostContent.substr(posDiv+1, (_hostContent.size() - 1));
        _utils.trimSpaces((_portRequest));
        
        std::cout << "este é domainContent " << _domainContent << std::endl;
        std::cout << "este é o  portRequest " << _portRequest << "a" << std::endl;
        std::cout << "TAMANHO DO PORT REQUEST: " << _portRequest.size() << std::endl;  
    } else {
        _domainContent = _hostContent;
        _portRequest = "80";
        std::cout << "este é o domainContent sem a porta " << _domainContent << std::endl;
        std::cout << "este é o portRequest DEFAULT " << _portRequest << std::endl;
    }  
    return true;
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

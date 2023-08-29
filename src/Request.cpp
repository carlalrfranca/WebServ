/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/24 18:26:41 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/28 21:30:52 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Request.hpp"

Request::Request(const std::string& request)
{
    _request = request;
    _hasError = false;
    _hostLine = "";
    _hostContent = "";
    _domainContent = "";
    _portRequest = "";
    _requestStream.str(request);
    std::getline(_requestStream, _firstLine);
}

Request::~Request()
{}

void Request::setRequest(const std::string& request)
{
    _request = request;
    _requestStream.str(request);
    std::getline(_requestStream, _firstLine);
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
        throw RequestException();
    //verifica se tem espaço após o metodo
    size_t spacePos = _firstLine.find(' ');
    if(spacePos == std::string::npos || spacePos == _firstLine.size() - 1 )
        throw RequestException();
    //valida HTTP após o metodo e espaço
    std::string version = _firstLine.substr(spacePos + 1);
    if(version.find("HTTP/1.1") == std::string::npos)
        throw RequestException();
    //valida espaço apos a versão
    spacePos = version.find(' ');
    if (spacePos == std::string::npos || spacePos == version.size() - 1)
        throw RequestException(); 
    //demais validacoes aqui, se houver
    return true;
}

void Request::setHasError(bool hasError)
{
    _hasError = hasError;
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
    while (std::getline(_requestStream, _hostLine)){
        if (_hostLine.substr(0, 6) == "Host: "){
            _hostContent = _hostLine.substr(6);
            std::cout << "Host content: " << _hostContent << std::endl;
            break;
        }
    }
    size_t posDiv = _hostContent.find(":");
    if(posDiv != std::string::npos) //127.0.0.1:5005  ou localhost:2005
    {
        _domainContent = _hostContent.substr(0, posDiv);
        _portRequest = _hostContent.substr(posDiv+1);
        std::cout << "este é domainContent" << _domainContent << std::endl;
        std::cout << "este é o  portRequest" << _portRequest << std::endl;
        
    }
    else
    {
        _domainContent = _hostContent;
        _portRequest = "80";
        std::cout << "este é o domainContent sem a porta " << _domainContent << std::endl;
        std::cout << "este é o portRequest DEFAULT " << _portRequest << std::endl;
    }
        
    return true;
}










/*

	if(!isFirstLineValid(request, _firstLine)){
	    hasError = true;
	    close(clientSocket);
	}
	if(hasError)
	    responseError();
	// ---------------------------------------------------------------------------------------------------
	std::istringstream firstLineStream(_firstLine);
	std::vector<std::string> _tokens;
	std::string _token;
	while (std::getline(firstLineStream, _token, ' ')){
	    _tokens.push_back(_token);
	}
	for (size_t i = 0; i < _tokens.size(); ++i){
	    std::cout << "Token " << i << ": " << _tokens[i] << std::endl;
	}
	
	std::string _hostLine;
	std::string _hostContent;
	
	while (std::getline(requestStream, _hostLine)){
	    if (_hostLine.substr(0, 6) == "Host: ") {
	        _hostContent = _hostLine.substr(6);
	        std::cout << "Host content: " << _hostContent << std::endl;
	        break;



*/

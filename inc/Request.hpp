/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 16:43:49 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/05 16:14:10 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _REQUEST_HPP_
#define _REQUEST_HPP_

#include "HeadersLibs.hpp"

class Request
{
    
    public:

        Request(const std::string& request); //criar instancia
        ~Request(void);
        
        void setRequest(const std::string& request);
		std::string getRequest(void) const;
        void printRequest(void);
        bool isFirstLineValid(void);
        bool validateRequest(void);
        
        void setHasError(bool hasError);
        bool getHasError(void)const;
        
        std::string getDomainRequest(void)const;
        std::string getPortRequest(void)const;
        void trimSpaces(std::string &s);
        const std::string& getMethod(void) const;
        const std::string& getURI(void) const;
        const std::string& getVersion(void) const;
    
    private:
    
        std::istringstream  _requestStream;
        std::string         _request;
        std::string         _firstLine; 
        std::string         _hostLine;
        std::string         _hostContent;
        std::string         _domainContent;
        std::string         _portRequest;
        std::string         _method;
        std::string         _uri;
        std::string         _version;
        bool                _hasError;
        
    
        class RequestException: public std::exception {
        public:
        
            virtual const char* what() const throw(){
            return "\nError: Invalid request detected.\n";
        }
            
    }; 
};

std::ostream& operator<<(std::ostream& output, const Request& rhs);
#endif

/*
void WebServ::printRequest(const std::string& request)
{
    std::istringstream iss(request);//permite tratar a string request como um fluxo de entrada
    std::string line;
    while(std::getline(iss, line) && line != "\r")
        std::cout << line << std::endl;
}

bool WebServ::isFirstLineValid(const std::string& request, std::string& _firstLine)
{
    std::istringstream requestStream(request);
    std::getline(requestStream, _firstLine);
    
    //valida http
    if(_firstLine.find("GET") == std::string::npos && 
        _firstLine.find("POST") == std::string::npos && 
         _firstLine.find("DELETE") == std::string::npos)
        return false;
    
    //verifica se tem espaço após o metodo
    size_t spacePos = _firstLine.find(' ');
    if(spacePos == std::string::npos || spacePos == _firstLine.size() - 1 )
        return false;
    
    //valida HTTP após o metodo e espaço
    std::string version = _firstLine.substr(spacePos + 1);
    if(version.find("HTTP/1.1") == std::string::npos)
        return false;
    
    //valida espaço apos a versão
    spacePos = version.find(' ');
    if(spacePos == std::string::npos || spacePos == version.size() - 1)
        return false;
    return true;
}







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
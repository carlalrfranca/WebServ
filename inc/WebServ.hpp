/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/07/22 17:24:29 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _WEBSERV_HPP_
#define _WEBSERV_HPP_

#include "ConfigParser.hpp"
#include "HeadersLibs.hpp"
#include "Response.hpp"
#include "SocketS.hpp"
#include "Request.hpp"
#include "Epoll.hpp"
#include "UtilsResponse.hpp"
#include "Utils.hpp"
#include "CGI.hpp"

class ConfigParser;
class SocketS;

class WebServ
{

    public:

        WebServ();
        WebServ(std::string filename);
        ~WebServ();
        void mainLoop();
        void responseError();
    	void checkForDuplicates();
        void printRequest(const std::string &request);
        void configSocket();
		void initServers();

        Epoll &getEpollS();
        std::string getResponse()const;
        void setResponse(const std::string& response);
        
		int	convertContentLength(std::string& header, size_t contentLengthPos);
		bool hasBodyContent(const std::string& header);
        bool isEventFromServerSocket(struct epoll_event *events, int index);
        bool isFirstLineValid(const std::string &request, std::string &_firstLine);
        void handleRequest(std::string &requestString);
        void removeClientFromEpoll(Epoll& epollS);
		int readRequest(int clientSocket);


    private:

        int						_clientSocket;
        Epoll					_epollS;
        Utils                   _utils;
		size_t 					_contentLength;
		size_t					_totalBytesRead;
        socklen_t				_clientAddressLength;
		std::string				_response;
		std::string				_requestString;
        std::string				_nameConfigFile;
        ConfigParser			_configParser;
        std::vector<SocketS>	_serverSocket;
        struct sockaddr_in		_clientAddress;

    class ErrorException : public std::exception
    {
        public:

            ErrorException(const std::string& message) throw() : _errorMessage(message) {}
    		virtual ~ErrorException() throw() {}
    		virtual const char* what() const throw()
            {
                return  _errorMessage.c_str();
            }

        private:

            std::string _errorMessage;
    	};
    };

#endif

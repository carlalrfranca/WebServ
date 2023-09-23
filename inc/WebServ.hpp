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
        void configSocket(size_t index);

        Epoll &getEpollS();
        bool isFirstLineValid(const std::string &request, std::string &_firstLine);
        bool isEventFromServerSocket(struct epoll_event *events, int index);
        void handleRequest(int clientSocket, char *buffer, ssize_t bytesRead, std::string &requestString);
		void readRequest(int clientSocket);
		std::string				_response;
		int						_totalBytesRead;
		int 					_contentLength;
		std::string				_requestString;

    private:

        int						_clientSocket;
        Epoll					_epollS;
        std::string				_nameConfigFile;
        ConfigParser			_configParser; // sujeito comentado
        std::vector<SocketS>	_serverSocket; // ----vetor criado
        socklen_t				_clientAddressLength;
        struct sockaddr_in		_clientAddress;
        Utils                   _utils;

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

/*
    em 31.08.2023

   g++ -std=c++98 -I inc/ src/main.cpp src/CGI.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp src/LocationDirective.cpp src/Epoll.cpp src/Request.cpp src/Response.cpp -o executavel_com_response
    ./executavel_com_response src/config.txt

*/

/*

    static int getTypePath(std::string const path);
    static int checkFile(std::string const path, int mode);
    std::string	readFile(std::string path);
    static int isFileExistAndReadable(std::string const path, std::string const index);
    std::string getPath();
    int getSize();

*/

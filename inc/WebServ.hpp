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
		//void addEndPoint(const std::string& ipAddress, const std::string& port);

        Epoll &getEpollS();
        std::string getResponse()const;
        void setResponse(const std::string& response);
        
		int	convertContentLength(std::string& header, size_t contentLengthPos);
		bool hasBodyContent(const std::string& header);
        bool isEventFromServerSocket(struct epoll_event *events, int index);
        bool isFirstLineValid(const std::string &request, std::string &_firstLine);
        void handleRequest(std::string &requestString);
        void removeClientFromEpoll(Epoll& epollS);
		void readRequest(int clientSocket);


    private:

        int						_clientSocket;
        Epoll					_epollS;
        Utils                   _utils;
		size_t 					_contentLength;
		size_t					_totalBytesRead;
        socklen_t				_clientAddressLength;
		std::string				_response; //set e get
		std::string				_requestString; //
        std::string				_nameConfigFile;
        ConfigParser			_configParser; // sujeito comentado
        std::vector<SocketS>	_serverSocket; // ----vetor criado
        struct sockaddr_in		_clientAddress;
        //std::set<std::pair<std::string, std::string> > _endPoints;

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

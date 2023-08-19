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

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <netdb.h>
#include <exception>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <map>
#include <string>

#include "SocketS.hpp"
#include "ConfigParser.hpp"

class ConfigParser;

class WebServ {

    public:
    
        WebServ();
        WebServ(std::string filename);
        ~WebServ();
        void mainLoop();
        void configSocket(size_t index);
        void responseError();
        bool isFirstLineValid(const std::string& request, std::string& _firstLine);
        void printRequest(const std::string& request);
		std::string handleCGIRequest(std::string& request);
		std::map<std::string, std::string> parseFormData(const std::string& formData);
        
    private:
    
        int             _clientSocket;
        std::string     _nameConfigFile;
        ConfigParser    _configParser; //sujeito comentado
        
        
        std::vector<SocketS> _serverSocket; // ----vetor criado
        
        
        //SocketS         _serverSocket;
        socklen_t       _clientAddressLength;
       // std::string     _request;

        struct sockaddr_in clientAddress;    

        class WebServException: public std::exception{
        public:
            virtual const char* what() const throw(){
                return "\nError: Inserir uma mensagem no WebServ Exception\n";
            }
        };
};

#endif


/*
    g++ -std=c++98 -I inc/ src/main.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp -o executavel
    ./executavel src/config.txt

*/
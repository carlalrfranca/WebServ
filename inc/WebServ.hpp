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
#include <string>
#include <cstdlib>
#include <netdb.h>
#include <exception>
#include <fstream>
#include <string>
//#include <vector>
#include <sstream>
#include <cstdlib>

#include "SocketS.hpp"
#include "ConfigParser.hpp"

class WebServ {

    public:
        WebServ();
        WebServ(std::string filename);
        ~WebServ();
        void mainLoop();
        SocketS         _serverSocket;
        
    private:
    
        int             _clientSocket;
        std::string     _nameConfigFile;
        ConfigParser    _configParser;
        socklen_t       _clientAddressLength;

        struct sockaddr_in clientAddress;    

        class WebServException: public std::exception{
        public:
            virtual const char* what() const throw(){
                return "\nError: Inserir uma mensagem no WebServ Exception\n";
            }
        };
};
//esse aqui está mais para depuração e apresentação das informações
std::ostream& operator<<(std::ostream& output, const WebServ &rhs);
#endif


/*

    g++ -std=c++98 -I inc/ src/main.cpp src/WebServ.cpp src/SocketS.cpp -o executavel
    ./executavel ./cfgs/default.config

*/
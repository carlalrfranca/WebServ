/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketS.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 17:46:53 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/06 20:30:35 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _SOCKETS_HPP_
#define _SOCKETS_HPP_

#include <string>
#include <cstdlib>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <stdexcept>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// criação do sockt, vinculação com o endereço e a escuta
// tudo em relação ao socket do SocketS

class SocketS {

    public:
        
        SocketS();
        ~SocketS();
        const int& getFD()const;
        void loadConfiguration();
        
        void setPort(std::string portNumber);
        void setAddress(std::string ipAddress);
        void setWebServSocket(int webServSocket);
        
        const std::string& getPort(void)const;
        const std::string& getAddress(void)const;
        const int& getWebServSocket(void)const;

        
        
         
    private:
        
        int             _webServSocket; //FD
        std::string     _portNumber;
        std::string     _ipAddress;
        std::string     _response; //talvez na response class
        std::string     _cssContent;
        // struct sockaddr_in _socketAddress;
        // int bindSocketSListenConnections(); //vincula socket a um endereço e porta 
        
        
        class SocketSException: public std::exception{
        public:
            SocketSException(const char* errorMessage) : message(errorMessage){}
            virtual const char* what()const throw(){
                return message;
            }
        
        private:
            const char* message;
        
        };
};
//esse aqui está mais para depuração e apresentação das informações
std::ostream& operator<<(std::ostream& output, const SocketS& instance);
#endif


/*
    g++ -std=c++98 -I inc/ src/main.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp -o executavel
    ./executavel src/config.txt

*/
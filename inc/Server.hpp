/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 17:46:53 by cleticia          #+#    #+#             */
/*   Updated: 2023/07/24 14:15:32 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

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
#include "WebServ.hpp"

class WebServ;

// criação do sockt, vinculação com o endereço e a escuta
// tudo em relação ao socket do server

class Server {

    public:
        
    Server(std::string filename);
    ~Server();
    const int& getFD()const;
    
    void loadConfiguration();
         
    private:
        
    const int _webServSocket; //FD
    std::string *_response; //talvez na response class
    std::string _cssContent;
    struct sockaddr_in _serverAddress = {0};

    int bindSocketListenConnections(); //vincula socket a um endereço e porta 
    
    class ServerException: public std::exception{

    public:
    ServerException(const char* errorMessage) : message(errorMessage){}
    virtual const char* what()const throw(){
        return message;
    }
    
    private:
    const char* message;
    };
};
//esse aqui está mais para depuração e apresentação das informações
std::ostream& operator<<(std::ostream& output, const Server& instance);
#endif
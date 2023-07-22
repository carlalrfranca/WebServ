/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 17:46:53 by cleticia          #+#    #+#             */
/*   Updated: 2023/07/21 18:48:00 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <netdb.h>
#include <iostream>
#include <stdexcept>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "Server.hpp"

class Server;

class Socket {

    public:
        
        Socket(void);
        ~Socket(void);
         
    private:
        
        char _buffer;
        int _serverSocket;
        int _clientSocket;
        ssize_t _bytesRead;
        socklen_t _clientAddressLength;
        // std::string _request; //talvez fique  na request class
        // size_t _foundRequest; //talvez fique  na request class
        // std::string *_response; //talvez na response class


};

std::ostream& operator<<(std::ostream& output, const Socket& rhs);
#endif
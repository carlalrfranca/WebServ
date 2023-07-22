/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 17:46:53 by cleticia          #+#    #+#             */
/*   Updated: 2023/07/21 21:44:40 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <string>
#include <cstdlib>
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
    Socket(int _port); //para incializar o objeto
    Socket(const Socket& copy);
        
         
    private:
        
    char _buffer;
    int _serverSocket;
    int _clientSocket;
    int _socketFD;
    
    ssize_t _bytesRead;
    socklen_t _clientAddressLength;
    std::string _request; //talvez fique na request class
    size_t _foundRequest; //talvez fique na request class
    std::string *_response; //talvez na response class
    ssize_t _bytesSent;
    std::string _cssContent;
    
    // flags
    bool _isOpen; //indica se o socket está aberto ou fechado
    bool _isBound; //indica se o socket está vinculado a um end e porta
    bool _isListening; //indica se o socket está n modo escuta
    int _port; //porta em que o servidor estará ouvindo por conexões
    
   bool OpenSocket(); //abre socket
   bool BindSocket(); //vincula socket a ume ndereço e porta
   bool Listen(int backLog);//coloca em modo de escuta p/ conexão de entrada?
   int Accept(); //aceita conexão de entrada, retorna o descritor do novo socket
   void close(); //fecha o socket e libera os recursos?Confirmar se apenas o destructor basta
   ssize_t Send(int _clientSocket, data, size_t dataLen); //enviar dados para um cliente conectado
   ssize_t Receive(int client_socket, void* buffer, size_t bufferLen); //receber dados de um cliente conectado
   PrintRequest();
    
    class SocketException: public std::exception{
        public:
            virtual const char* what() const throw();
    };
};
//esse aqui está mais para depuração e apresentação das informações
std::ostream& operator<<(std::ostream& output, const Socket& rhs);
#endif
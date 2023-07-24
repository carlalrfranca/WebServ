/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 17:46:53 by cleticia          #+#    #+#             */
/*   Updated: 2023/07/24 14:05:11 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

Server::Server(std::string filename){


    // aqui vão ser chamados os métodos (que serão criados ainda)
    // pra parseamento do arquivo de configuração
    // e armazenamento de seus dados importantes

    /*cria socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1){
        std::cerr << "Erro ao criar socket: " << std::endl;
        return 1;
    }
    //configura endereço do servidor e inicializa os campos da estrutura com 0
    struct sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080); 
    server_address.sin_addr.s_addr = INADDR_ANY;
    */
}

Server::~Server(){}

const int& Server::getFD()const{

    return _webServSocket;
}

void Server::loadConfiguration(){}

int Server::bindSocketListenConnections(){

    try{
        if(bind(_webServSocket,(struct sockaddr*)&_serverAddress, sizeof(_serverAddress)) == -1){
            throw ServerException("Error: when binding socket.");
        }
        if(listen(_webServSocket, 5) == -1){
            throw ServerException("Error: when listening socket.");
        }
 
    }catch(const ServerException& e){
        std::cerr << e.what() << std::endl;
        close(_webServSocket);
        return -1;
    }
    return 0; //coloquei 1 mais para tirar a linha vermelha
}

std::ostream& operator<<(std::ostream& output, const Server& instance){
    output << "Server Info:" << std::endl;
    output << "WebServ Socket : " << instance.getFD() << std::endl;
    output << "Socket FD: " << instance.getFD() << std::endl;
    return (output);
}


    /*
     if(bind(server_socket,(struct sockaddr*)&server_address, sizeof(server_address)) == -1){
        std::cerr << "Erro ao vincular socket: " << std::endl;
        close(server_socket);
        return 1;
    }
    
    //habilitar o socket para aguardar conexões de entrada. ) 5 representa o tamanho máximo da fila de conexões pendentes.
    if(listen(server_socket, 5) == -1){
        std::cerr << "Erro ao ouvir socket: " << std::endl;
        close(server_socket);
        return 1;
    }
    
    */
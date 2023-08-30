/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketS.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 17:46:53 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/30 19:30:12 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"
#include "../inc/SocketS.hpp"

SocketS::SocketS(){
    _portNumber = "";
    _ipAddress = "";
    _response = "";
    _cssContent = "";
    _webServSocket = 0;
}


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

SocketS::~SocketS(){}

// getter e setter do locations

std::map<std::string, LocationDirective> SocketS::getLocations(void) const
{
	return _locations;
}

void SocketS::setLocations(std::map<std::string, LocationDirective> locations)
{
	_locations = locations;
}

// 

const int& SocketS::getFD()const{

    return _webServSocket;
}

void SocketS::loadConfiguration(){}


void SocketS::setPort(std::string portNumber){
    this->_portNumber = portNumber;
}

void SocketS::setAddress(std::string ipAddress){
    this->_ipAddress = ipAddress;
}

void SocketS::setWebServSocket(int webServSocket){
    this->_webServSocket = webServSocket;
}

const std::string& SocketS::getPort(void)const{
    return _portNumber;
}

const std::string& SocketS::getAddress(void)const{
    return _ipAddress;
}

const int& SocketS::getWebServSocket(void)const{
    return _webServSocket;
}

void SocketS::setServerName(std::vector<std::string> serverName)
{
     this->_serverName = serverName;
}

std::vector<std::string> SocketS::getServerName(void)const
{
    return _serverName;
}
 
void SocketS::setMethods(std::vector<std::string> methods)
{
    _methods = methods;
}

std::vector<std::string> SocketS::getMethods(void) const
{
    return _methods;
}
 
     
// int SocketS::bindSocketListenConnections(){
// 
    // try{
        // if(bind(_webServSocket,(struct sockaddr*)&_serverAddress, sizeof(_serverAddress)) == -1){
            // throw SocketSException("Error: when binding socket.");
        // }
        // if(listen(_webServSocket, 5) == -1){
            // throw SocketSException("Error: when listening socket.");
        // }
//  
    // }catch(const SocketSException& e){
        // std::cerr << e.what() << std::endl;
        // close(_webServSocket);
        // return -1;
    // }
    // return 0; //coloquei 1 mais para tirar a linha vermelha
// }
// 
// std::ostream& operator<<(std::ostream& output, const SocketS& instance){
    // output << "SocketS Info:" << std::endl;
    // output << "WebServ Socket : " << instance.getFD() << std::endl;
    // output << "Socket FD: " << instance.getFD() << std::endl;
    // return (output);
// }


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
    
    
    
    
/*

    g++ -std=c++98 -I inc/ src/main.cpp src/WebServ.cpp src/SocketS.cpp -o executavel
    ./executavel ./cfgs/default.config

*/
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 21:24:02 by cleticia          #+#    #+#             */
/*   Updated: 2023/07/28 21:35:06 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ConfigParser.hpp"

ConfigParser::ConfigParser(){
    //_line = ""; //talvez, mas acho que nao precisa mesmo
    _domainName = ""; //server_name
    _ssl = ""; //ssl_certificate
    _key = ""; //ssl_certificate
    _rules = ""; //location
    _invalidPos = std::string::npos;
    _directive = 0;
    _port = 0;
    _address = "";
    _posInit = 0;
    _posFinal = 0;

}

ConfigParser::~ConfigParser(){}

void ConfigParser::processListen(std::string& line, WebServ& wsManager){

    bool foundDefaultServer = false;
    _directive = line.find("listen");
    
    while(_directive != _invalidPos){
        size_t posIniIP = line.find_first_of("0123456789.:", _directive, + 6); // 6 para pular a palavra listen
        
        if(posIniIP != _invalidPos){
            size_t posFinIP = line.find_first_of(" \t\n\r\f\v;", posIniIP);
            std::string ipAddress = line.substr(posIniIP, posFinIP - posIniIP); // Extrai o endereço I
            
            if(ipAddress.size() >= 3 && ipAddress.substr(ipAddress.size() - 3) == ":80") // Remove o ":80"
                ipAddress = ipAddress.substr(0, ipAddress.size() - 3);    
            if(ipAddress.size() >= 5 && ipAddress.substr(ipAddress.size() - 5) == ":8080") // Remove o ":8080"
                ipAddress = ipAddress.substr(0, ipAddress.size() - 5);
            if(!foundDefaultServer){
                wsManager._serverSocket.setAddress(ipAddress);
                foundDefaultServer = true;
            }
        }
        size_t posPort = line.find("80");
        if(posPort != _invalidPos){
            std::string eighty = line.substr(posPort, 2);
            int intPort = std::atoi(eighty.c_str());
            wsManager._serverSocket.setPort(intPort);
            wsManager._serverSocket.setAddress("0.0.0.0");
        }else
            wsManager._serverSocket.setPort(80);
        
    }
    

    /*
        listen 
        
        listen 127.0.0.1;
        listen http://localhost.com;
        listen yourdomain.com;,
        
            listen 80;
        listen 80 default_server;
        listen 8080;
        listen *:80;
        listen 127.0.0.1:8080;
        listen 192.168.1.100:80;
        listen localhost:80;
        listen yourdomain.com:80;
              
        listen 443 ssl;
        listen *:443 ssl;
        listen 192.168.1.100:443 ssl;
    */

}

void ConfigParser::processServerName(std::string& line){

}

void ConfigParser::processHost(std::string& line){

}

void ConfigParser::processLocation(std::string& line){
}

void ConfigParser::applyDefaultValues(void){

}


/*

///////////////////////////////////////////////////////////////////////////
exemplo:

server {
    listen        80;
    server_name   .empresa1.com;
    ...
}
server {
    listen        80;
    server_name   .empresa2.com;
    ...
}
server {
    listen        80;
    server_name   .empresa3.com;
    ...
}
server {
    listen        127.0.0.1;
    server_name   localhost;
    ...
}
////////////////////////////////////////////////////////////////////////////////////////////////////////

propriedade

port se tiver : 

string ipAddress . . . 
int portNumber

primeiro valida que é numero
o que estiver depois passa atoi e armazena em portNumber
o que está antes do dois numeros precisa validar a string de acordo com formato de endereço IP e dai armazena ipAddress que deverá ser mantido como string
Se tiver apenas o IP então considero como porta 80 o default e armazeno o valor 80 +em portNumber
Se tiver apenas a porta então considero como IP 0.0.0.0 e armazeno esse valor em ipAddress

serverName[] vetor de strings

Ao receber um resquest ele vai iterar pelo vetor e verificar o ipAddress e portNumber
Ele deve escolher o server mais adequado para a requisicao ele vai compara o ip porta da requisicao com o ip e porta do vertor de servidores[sockets]
é nele que vai aceitar a conexao do client

Se contabilizar mais que 1 server adedaquado entao devera analisar da propriedade server_name.
Voce compara o server_name com o host que é uma linha que virá da request
Se encontrar um server então para e é ele que deverá aceitar a conexão.

Se vários sockets tiverem listen e server_name iguais entoa esoclhe-se o primeiro listado.
e os demais vetores deverão apresentar a mensagem "nginx: [warn] conflicting server name “.empresarepetida.com” on 0.0.0.0:80, ignored"
na inicialização do nginx


Caso de host desconhecido

Se hostRequest for != serverName
Verificar se algum socket tem a flag marcada como default_server
(campo listen de um deles possuia "default_server") 
no caso positivo deverá ir para o socket especificado como default 
[pode criar uma flag para definir se foi marcado como defaultServer]
a conexao devera ser aceita pelo primeiro Socket que contenha Ip e porta iguais 
ao Ip e porta do campo host da Request

o capo listen é do arquivo de configuracao e correpsonde aos atributos da classe Socket
ipAddress e portNumber;

arquivo de configuração

 arquivo deve preencher os atributos de Scket/Server
 e depois o conteudo da request deve ser ocmparado com as variaveis já preenchidas

*/

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/07/28 10:44:46 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/WebServ.hpp"
#include "../inc/SocketS.hpp"

WebServ::WebServ(std::string filename){
    std::ifstream fileToParse;
    fileToParse.open(filename.c_str());
    
    if(fileToParse.is_open()){
        std::string line;
        bool foundDefaultServer = false;

        while(getline(fileToParse, line)){
            std::cout << line << std::endl;
            
            size_t posListen = line.find("listen");
            if(posListen != std::string::npos){
                std::cout << " --------------------------------- " << std::endl;
                std::cout << "[TESTE]Posição Listen: " << posListen << std::endl;
                
                // e se o listen for http://localhost? 
                size_t posIniIP = line.find_first_of("0123456789.:", posListen, + 6); // 6 para pular a palavra listen
                if(posIniIP != std::string::npos){
                    size_t posFinIP = line.find_first_of(" \t\n\r\f\v;", posIniIP);
                    std::string ipAddress = line.substr(posIniIP, posFinIP - posIniIP); // Extrai o endereço IP
    
                    if(ipAddress.size() >= 3 && ipAddress.substr(ipAddress.size() - 3) == ":80") // Remove o ":80"
                        ipAddress = ipAddress.substr(0, ipAddress.size() - 3);
                        
                    if(!foundDefaultServer){
                        _serverSocket.setAddress(ipAddress);
                        std::cout << "[TESTE]Endereço IP: " << ipAddress << std::endl;
                        foundDefaultServer = true;
                    }
                }
                    
                size_t posPort = line.find("80");
                if(posPort != std::string::npos){
                    std::string eighty = line.substr(posPort, 2);
                    int intPort = std::atoi(eighty.c_str());
                    std::cout << "[TESTE]Posição da Porta: " << posPort << std::endl;
                    std::cout << "[TESTE]Porta: " << eighty << std::endl;
                    std::cout << " --------------------------------- " << std::endl;
                    _serverSocket.setPort(intPort);
                    _serverSocket.setAddress("0.0.0.0");
                }else
                    _serverSocket.setPort(80);
            }
            // continua para segunda linha
            size_t posServerName = line.find("server_name");
            if(posServerName != std::string::npos){
                std::cout << " --------------------------------- " << std::endl;
                std::cout << "[TESTE]Posição server_name: " << line << posServerName << std::endl;
            }//fim do if maior
        }// final do while
        std::cout << "[TESTE] ENDEREÇO ARMAZENADO: "<< _serverSocket.getAddress() << "\n";
        std::cout << "[TESTE] PORTA ARMAZENADA: "<< _serverSocket.getPort() << "\n";
        fileToParse.close();
    }else //final if de abertura do arquivo
        std::cout << "Erro ao abrir" << std::endl;
}

WebServ::WebServ(){}

WebServ::~WebServ(){}

void WebServ::mainLoop(){

    std::cout << "chegamos na mainloop parsa!" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////

/*

    if(posIP && posDelimiter){
        if(posIP != std::string::npos || posDelimiter != std::string::npos){
            std::string ipAddress = line.substr(posIP, (posDelimiter - 1) - posIP);
            std::cout << "ipAddress" << ipAddress << std::endl;
        }
        else 
            std::cout << "realmente" << std::endl;
            
                
                // ou so tem a porta
                // ou so tem o endereço
                // ou tem a porta e o endereço
            
          
                // found port
                // found address

        
*/
/*

 g++ -std=c++98 src/main.cpp src/WebServ.cpp src/SocketS.cpp -I inc/ -o executavel
 ./executavel ./cfgs/default.config 


*/        
        
/*


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
    
        
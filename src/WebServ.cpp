/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/07/28 18:17:21 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/WebServ.hpp"
#include "../inc/ConfigParser.hpp"

WebServ::WebServ(){}

WebServ::~WebServ(){}

WebServ::WebServ(std::string filename){

    std::ifstream fileToParse;
    fileToParse.open(filename.c_str());
    if(fileToParse.is_open()){
        std::string line;
        while(getline(fileToParse, line)){
            //verifica a presença de diretivas e chamar os métodos
            if(line.find("listen") != std::string::npos){
                _configParser.processListen(line, *this);
            }else if (line.find("server_name") != std::string::npos){
                _configParser.processServerName(line);
            }else if (line.find("host") != std::string::npos){
                _configParser.processHost(line);
            }else if (line.find("location") != std::string::npos){
                _configParser.processLocation(line);
            }
        }
        //método para os casos da ausencia de diretivas e aplicação da configuração default
        _configParser.applyDefaultValues();
        fileToParse.close();
                
    }else
        std::cout << "[Error] : file cannot be opened" << std::endl;       
}               


void WebServ::mainLoop(){

    std::cout << "chegamos na mainloop parsa!" << std::endl;
}


/*

 g++ -std=c++98 src/main.cpp src/WebServ.cpp src/SocketS.cpp -I inc/ -o executavel
 ./executavel ./cfgs/default.config 


*/        
        
/
                    size_t posIniIP = line.find_first_of("0123456789.:", posListen, + 6); // 6 para pular a palavra listen
                if(posIniIP != std::string::npos){
                    size_t posFinIP = line.find_first_of(" \t\n\r\f\v;", posIniIP);
                    std::string ipAddress = line.substr(posIniIP, posFinIP - posIniIP); // Extrai o endereço IP
    
                    if(ipAddress.size() >= 3 && ipAddress.substr(ipAddress.size() - 3) == ":80") // Remove o ":80"
                        ipAddress = ipAddress.substr(0, ipAddress.size() - 3);
                        
                    if(!foundDefaultServer){
                        _serverSocket.setAddress(ipAddress);
                        std::cout << "[TESTE]Endereço IP: " << ipAddress << std::endl;
                        //foundDefaultServer = true;
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
                    //_serverSocket.setAddress("0.0.0.0");
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


*/
    
        
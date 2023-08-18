/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/18 00:20:57 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/WebServ.hpp"
#include "../inc/ConfigParser.hpp"
#include "../inc/RequestParser.hpp"
#include <string.h>

// 
#include <sys/epoll.h>
#include <fcntl.h>
// 

WebServ::WebServ()
{
    //_nPos = std::string::npos;
}

WebServ::~WebServ(){}
                
//----------------17.08.2023
WebServ::WebServ(std::string filename){

    std::ifstream fileToParse;
    size_t index = -1;
    fileToParse.open(filename.c_str());
    if(fileToParse.is_open()){
        std::string line;
        bool isLocationBlock = false;
        while(getline(fileToParse, line)){
            if (line.find("server{") != std::string::npos){
                index++;
                std::cout << "Index: " << index << std::endl;
                if (index > 0){
                    configSocket(index - 1);
                }
            }
            else if (line.find("listen") != std::string::npos){
                _configParser.processListen(line);
            }else if (line.find("server_name") != std::string::npos){
                _configParser.processServerName(line);
            }else if (line.find("root") != std::string::npos){
                _configParser.processRoot(line);
            }else if (line.find("location") != std::string::npos || isLocationBlock == true){
                if (line.find("}") != std::string::npos)
				{
                    isLocationBlock = false;
					continue;
				}
                isLocationBlock = true;
                _configParser.processLocation(line);
            } // configSocket(index);
        }
    }else
        std::cout << "[Error] : file cannot be opened" << std::endl;
    if (index != 0)
        configSocket(index);
    fileToParse.close();
    mainLoop();
}

void WebServ::configSocket(size_t serverIndex){
   
    std::cout << "---------  CHEGA AQUI [2] -----------\n" << std::endl;
    SocketS temp_socket;
    //_serverSocket[serverIndex].setPort(_configParser.getPort());
    //_serverSocket[serverIndex].setAddress(_configParser.getAddress());
    temp_socket.setPort(_configParser.getPort());
    temp_socket.setAddress(_configParser.getAddress());
    
    _serverSocket.push_back(temp_socket);
    std::cout << "Port: " << _serverSocket.back().getPort() << std::endl;
    std::cout << "Address: " << _serverSocket.back().getAddress() << std::endl;
    //cria socket
    _serverSocket[serverIndex].setWebServSocket(socket(AF_INET, SOCK_STREAM, 0));//int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(_serverSocket[serverIndex].getWebServSocket() == -1){
        throw WebServException();
    }
    
    //configura endereço do servidor e inicializa os campos da estrutura com 0
    struct sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET; //socket usará os ends. IPv4
    server_address.sin_port = htons(std::atoi(_configParser.getPort().c_str())); //usa a função htons para converter8080 para a ordem de bytes da rede e atribui a sin_port
    server_address.sin_addr.s_addr = INADDR_ANY; //especifica o end.IP que o socket do server será vinculado

    //chamada para o bind - vincula o socket ao endereço e porta, 0 -1 tem haver com a falha na chamada do bind
    if(bind(_serverSocket[serverIndex].getWebServSocket(),(struct sockaddr*)&server_address, sizeof(server_address)) == -1){
        close(_serverSocket[serverIndex].getWebServSocket());
         WebServException();
    }
    
    //habilitar o socket para aguardar conexões de entrada. ) 5 representa o tamanho máximo da fila de conexões pendentes.
    if(listen(_serverSocket[serverIndex].getWebServSocket(), 5) == -1){
        close(_serverSocket[serverIndex].getWebServSocket());
        throw WebServException();
    }
    std::cout << "--------- Socket atual colocado pra escutar..." << std::endl;
}

void WebServ::printRequest(const std::string& request){
    std::istringstream iss(request);//permite tratar a string request como um fluxo de entrada
    std::string line;
    while(std::getline(iss, line) && line != "\r"){
        std::cout << line << std::endl;
    }
}

bool WebServ::isFirstLineValid(const std::string& request, std::string& _firstLine){

    std::istringstream requestStream(request);
    std::getline(requestStream, _firstLine);
    
    //valida http
    if(_firstLine.find("GET") == std::string::npos && 
        _firstLine.find("POST") == std::string::npos && 
         _firstLine.find("DELETE") == std::string::npos)
        return false;
    
    //verifica se tem espaço após o metodo
    size_t spacePos = _firstLine.find(' ');
    if(spacePos == std::string::npos || spacePos == _firstLine.size() - 1 )
        return false;
    
    //valida HTTP após o metodo e espaço
    std::string version = _firstLine.substr(spacePos + 1);
    if(version.find("HTTP/1.1") == std::string::npos)
        return false;
    
    //valida espaço apos a versão
    spacePos = version.find(' ');
    if(spacePos == std::string::npos || spacePos == version.size() - 1)
        return false;
    return true;
}

void WebServ::responseError(){
    std::string response;
    int errorCode = 400;
    if(errorCode == 400)
        response = "HTTP/1.1 400 Bad Request\r\n\r\n";
    else if(errorCode == 404)
        response = "HTTP/1.1 404 Not Found\r\n\r\n";
    else if(errorCode == 500)
        response = "HTTP/1.1 500 Internal Server\r\n\r\n";
    else //erro não reconhecido
        response = "HTTP/1.1 Error\r\n\r\n";
    ssize_t bytesSent = send(_clientSocket, response.c_str(), response.size(), 0);
    if(bytesSent < 0) {
        std::cerr << "[Error] sending error response." << std::endl;
    }
}

void WebServ::mainLoop(){

    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Servidor iniciado. Aguardando conexões..." << std::endl;
    std::cout << "-----------------------------------------\n" << std::endl;
    
	// epolll try
	int epollFd = epoll_create1(0);
	if (epollFd == -1)
	{
		perror("Error creating epoll");
		return;
	}
	// 

    //Imprimir detalhes de cada servidor
    for (size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex){
        std::cout << "Detalhes do servidor " << serverIndex << ":" << std::endl;
        std::cout << "Porta: " << _serverSocket[serverIndex].getPort() << std::endl;
        std::cout << "Endereço: " << _serverSocket[serverIndex].getAddress() << std::endl;
        std::cout << "-----------------------------------------\n" << std::endl;
		// try epoll
		struct epoll_event event;
        event.data.fd = _serverSocket[serverIndex].getWebServSocket();
        event.events = EPOLLIN | EPOLLET;
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, event.data.fd, &event) == -1) {
            perror("Error adding socket to epoll");
            return;
        }
    }
	const int maxEvents = 10;
    struct epoll_event events[maxEvents];
    
	while(true)
	{
		int numEvents = epoll_wait(epollFd, events, maxEvents, -1);
        if (numEvents == -1) {
            perror("Error in epoll_wait");
            return;
        }

        for (int i = 0; i < numEvents; ++i)
		{
			bool monitor = false;
			for (size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex)
			{
				if (events[i].data.fd == _serverSocket[serverIndex].getWebServSocket())
				{
					monitor = true;
					break;
				}
			}
				if (monitor == true)
				{
					struct sockaddr_in clientAddress = {0};
					socklen_t clientAddressLength = sizeof(clientAddress);
					int clientSocket = accept(events[i].data.fd, (struct sockaddr*)&clientAddress, &clientAddressLength);
					
					if (clientSocket == -1) {
					    perror("Error accepting client connection");
					    continue; // Move to the next event
					}
					
					// Set the client socket to non-blocking mode
					int flags = fcntl(clientSocket, F_GETFL, 0);
					fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);
					struct epoll_event event;
					event.data.fd = clientSocket;
					event.events = EPOLLIN | EPOLLET; // Listen for read events in edge-triggered mode
					
					if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &event) == -1) {
					    perror("Error adding client socket to epoll");
					    close(clientSocket); // Close the socket on error
					}
				}
				else
				{
					int clientSocket = events[i].data.fd;
					char buffer[1024];
            		ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0); //peguei a request
            		if(bytesRead <= 0){
            		    std::cerr << "Erro ao receber solicitação do client " << std::endl;
            		    continue;
            		}
            		std::string request(buffer, bytesRead); //buffer contém os dados recebidos do client

            		std::cout << "EITA -------" << std::endl;
            		printRequest(request);
            		std::cout << "EITA -------" << std::endl;
            		std::istringstream requestStream(request); //pega a 
            		std::string _firstLine;
            		std::getline(requestStream, _firstLine);
            		// ---------------------------------------------------------------------------------------------------

            		bool hasError = false;
            		if(!isFirstLineValid(request, _firstLine)){
            		    hasError = true;
            		    close(clientSocket);
            		}
	
            		// resposta de erro
            		if(hasError){
            		    responseError();
            		}
            		// ---------------------------------------------------------------------------------------------------
            		std::istringstream firstLineStream(_firstLine);
            		std::vector<std::string> _tokens;
            		std::string _token;
            		while (std::getline(firstLineStream, _token, ' ')){
            		    _tokens.push_back(_token);
            		}
            		for (size_t i = 0; i < _tokens.size(); ++i){
            		    std::cout << "Token " << i << ": " << _tokens[i] << std::endl;
            		}
            		std::string _hostLine;
            		std::string _hostContent;

            		while (std::getline(requestStream, _hostLine)){
            		    if (_hostLine.substr(0, 6) == "Host: ") {
            		        _hostContent = _hostLine.substr(6);
            		        std::cout << "Host content: " << _hostContent << std::endl;
            		        break;
            		    }
            		}
	
            		// response só pra satisfazer o client
            		const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><head><link rel='stylesheet' href='style.css'></head><body><h1>Hello World</h1></body></html>";
					ssize_t bytesSent = send(clientSocket, response, strlen(response), 0); //começa sempre pelo metodo: envia a reponse para o clienteSocket e retorna a quantidade de bytes.
					if (bytesSent == -1){
						std::cerr << "Erro ao enviar a resposta ao cliente" << std::endl;
					}


            		// if(_hostContent.empty()){
            		//     std::cerr << "Linha 'Host:' não encontrada." << std::endl;
            		// }      
	    			// Fecha o socket do client  >> se não fechar antes fica no loop
	    			close(clientSocket);
            		std::cout << "\n---------------------------------------" << std::endl;
	    			std::cout << "----- FECHOU A CONEXÃO COM O CLIENTE ----" << std::endl;
            		std::cout << "-----------------------------------------" << std::endl;  
				}
		}
	}
	for (size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex)
        close(_serverSocket[serverIndex].getWebServSocket());
}








// ---------------------------------------------------
    //abre o loop principal. Aceita novas conexões
    // for (size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex){
        // while(true){
            
            
            // TODO:
            /*
            
                INCLUIR epoll() PRA QUE NAO SEJA BLOQUEANTE (por enquanto, só conseguimos nos conectar com um por vez)
                para os clients
            
            */
            
            
    /*        struct sockaddr_in client_address = {0};
            _clientAddressLength = sizeof(client_address);
            _clientSocket = accept(_serverSocket[serverIndex].getWebServSocket(),(struct sockaddr *)&client_address, &_clientAddressLength);
            if(_clientSocket == -1){
                std::cerr << "Erro ao aceitar a conexão" << std::endl;
                continue;
            }
            //Recebe solicitação
            char buffer[1024];
            ssize_t bytesRead = recv(_clientSocket, buffer, sizeof(buffer), 0); //peguei a request
            if(bytesRead <= 0){
                std::cerr << "Erro ao receber solicitação do client " << std::endl;
                continue;
            }
            std::string request(buffer, bytesRead); //buffer contém os dados recebidos do client
            
            std::cout << "EITA -------" << std::endl;
            printRequest(request);
            std::cout << "EITA -------" << std::endl;
            std::istringstream requestStream(request); //pega a 
            std::string _firstLine;
            std::getline(requestStream, _firstLine);
            // ---------------------------------------------------------------------------------------------------
            
            bool hasError = false;
            if(!isFirstLineValid(request, _firstLine)){
                hasError = true;
                close(_clientSocket);
            }
            
            // resposta de erro
            if(hasError){
                responseError();
            }
            // ---------------------------------------------------------------------------------------------------
            std::istringstream firstLineStream(_firstLine);
            std::vector<std::string> _tokens;
            std::string _token;
            while (std::getline(firstLineStream, _token, ' ')){
                _tokens.push_back(_token);
            }
            for (size_t i = 0; i < _tokens.size(); ++i){
                std::cout << "Token " << i << ": " << _tokens[i] << std::endl;
            }
            std::string _hostLine;
            std::string _hostContent;
    
            while (std::getline(requestStream, _hostLine)){
                if (_hostLine.substr(0, 6) == "Host: ") {
                    _hostContent = _hostLine.substr(6);
                    std::cout << "Host content: " << _hostContent << std::endl;
                    break;
                }
            }
            
            // response só pra satisfazer o client
            const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><head><link rel='stylesheet' href='style.css'></head><body><h1>Hello World</h1></body></html>";
			ssize_t bytesSent = send(_clientSocket, response, strlen(response), 0); //começa sempre pelo metodo: envia a reponse para o clienteSocket e retorna a quantidade de bytes.
			if (bytesSent == -1){
				std::cerr << "Erro ao enviar a resposta ao cliente" << std::endl;
			}
            
     
            // if(_hostContent.empty()){
            //     std::cerr << "Linha 'Host:' não encontrada." << std::endl;
            // }      
	    	// Fecha o socket do client  >> se não fechar antes fica no loop
	    	close(_clientSocket);
            std::cout << "\n---------------------------------------" << std::endl;
	    	std::cout << "----- FECHOU A CONEXÃO COM O CLIENTE ----" << std::endl;
            std::cout << "-----------------------------------------" << std::endl;   
        }//fim do while
        //fecha o socket do server
        for (size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex)
        close(_serverSocket[serverIndex].getWebServSocket());
    } //fim do for
    return;   
}*/

/*
    g++ -std=c++98 -I inc/ src/main.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp -o executavel
    ./executavel src/config.txt

*/
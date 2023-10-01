/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   antigo.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/17 17:10:14 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/WebServ.hpp"
#include "../inc/ConfigParser.hpp"
#include "../inc/RequestParser.hpp"

WebServ::WebServ(){
    _nPos = std::string::npos;
}

WebServ::~WebServ(){}
                
WebServ::WebServ(std::string filename){

    std::ifstream fileToParse;
    size_t index = -1;
    fileToParse.open(filename.c_str());
    if(fileToParse.is_open()){
        std::string line;
        bool isLocationBlock = false;
        while(getline(fileToParse, line)){
            if (line.find("server{") != _nPos){
                index++;
                std::cout << "Index: " << index << std::endl;
                if (index > 0)
                    configSocket(index - 1);
            }
            else if (line.find("listen") != _nPos){
                _configParser.processListen(line);
            }else if (line.find("server_name") != _nPos){
                _configParser.processServerName(line);
            }else if (line.find("root") != _nPos){
                _configParser.processRoot(line);
            }else if (line.find("location") != _nPos || isLocationBlock == true){
                isLocationBlock = true;
                _configParser.processLocation(line);
                if (line.find("}") != _nPos)
                    isLocationBlock = false;
            } // configSocket(index);
        }
    }else
        std::cout << "[Error] : file cannot be opened" << std::endl;
    if (index != 0)
        configSocket(index);
    fileToParse.close();
    mainLoop();
}               

void WebServ::printRequest(const std::string& request){
    std::istringstream iss(request);//permite tratar a string request como um fluxo de entrada
    std::string line;
    while(std::getline(iss, line) && line != "\r")
        std::cout << line << std::endl;
}

bool WebServ::isFirstLineValid(const std::string& request, std::string& _firstLine){
    std::istringstream requestStream(request);
    std::getline(requestStream, _firstLine);
    
    //valida http
    if(_firstLine.find("GET") == _nPos && 
        _firstLine.find("POST") == _nPos && 
         _firstLine.find("DELETE") == _nPos)
        return false;
    
    //verifica se tem espaço após o metodo
    size_t spacePos = _firstLine.find(' ');
    if(spacePos == _nPos || spacePos == _firstLine.size() - 1 )
        return false;
    
    //valida HTTP após o metodo e espaço
    std::string version = _firstLine.substr(spacePos + 1);
    if(version.find("HTTP/1.1") == _nPos)
        return false;
    
    //valida espaço apos a versão
    spacePos = version.find(' ');
    if(spacePos == _nPos || spacePos == version.size() - 1)
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

void WebServ::mainLoop(){


    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Servidor iniciado. Aguardando conexões..." << std::endl;
    std::cout << "-----------------------------------------\n" << std::endl;

    
    //deve configurar servidores
    // for (size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex){   
    //     std::cout << "---------  CHEGA AQUI [1] -----------\n" << std::endl;
    //     configSocket(serverIndex);
    // }
    
    //Imprimir detalhes de cada servidor
    for (size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex){
        std::cout << "Detalhes do servidor " << serverIndex << ":" << std::endl;
        std::cout << "Porta: " << _serverSocket[serverIndex].getPort() << std::endl;
        std::cout << "Endereço: " << _serverSocket[serverIndex].getAddress() << std::endl;
        std::cout << "-----------------------------------------\n" << std::endl;
    }
    
    //abre o loop principal. Aceita novas conexões
    for (size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex){
        while(true){
            
            
            // TODO:
            /*
            
                INCLUIR epoll() PRA QUE NAO SEJA BLOQUEANTE (por enquanto, só conseguimos nos conectar com um por vez)
                para os clients
            
            */
            
            
            struct sockaddr_in client_address = {0};
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
}

       
/*
                    size_t posIniIP = line.find_first_of("0123456789.:", posListen, + 6); // 6 para pular a palavra listen
                if(posIniIP != _nPos){
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
                if(posPort != _nPos){
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
            if(posServerName != _nPos){
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

    g++ -std=c++98 -I inc/ src/main.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp -o executavel
    ./executavel src/config.txt

*/



/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/15 17:51:59 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/WebServ.hpp"
#include "../inc/ConfigParser.hpp"
#include "../inc/RequestParser.hpp"

WebServ::WebServ(){
    _nPos = std::string::npos;
}

WebServ::~WebServ(){
    close(_epollFD);
}
                
WebServ::WebServ(std::string filename){

    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Servidor iniciado. Aguardando conexões..." << std::endl;
    std::cout << "-----------------------------------------\n" << std::endl;

    std::ifstream fileToParse;
    size_t index = -1;
    fileToParse.open(filename.c_str());
    if(fileToParse.is_open()){
        std::string line;
        bool isLocationBlock = false;
        while(getline(fileToParse, line)){
            if (line.find("server{") != _nPos){
                index++;
                std::cout << "Index: " << index << std::endl;
                if (index > 0)
                    configSocket(index - 1);
            }
            else if (line.find("listen") != _nPos){
                _configParser.processListen(line);
            }else if (line.find("server_name") != _nPos){
                _configParser.processServerName(line);
            }else if (line.find("root") != _nPos){
                _configParser.processRoot(line);
            }else if (line.find("location") != _nPos || isLocationBlock == true){
                isLocationBlock = true;
                _configParser.processLocation(line);
                if (line.find("}") != _nPos)
                    isLocationBlock = false;
            } // configSocket(index);
        }
    }else
        std::cout << "[Error] : file cannot be opened" << std::endl;
    if (index != 0)
        configSocket(index - 1);
    fileToParse.close();
    mainLoop();
}               

void WebServ::printRequest(const std::string& request){
    std::istringstream iss(request);//permite tratar a string request como um fluxo de entrada
    std::string line;
    while(std::getline(iss, line) && line != "\r")
        std::cout << line << std::endl;
}

bool WebServ::isFirstLineValid(const std::string& request, std::string& _firstLine){
    std::istringstream requestStream(request);
    std::getline(requestStream, _firstLine);
    
    //valida http
    if(_firstLine.find("GET") == _nPos && 
        _firstLine.find("POST") == _nPos && 
         _firstLine.find("DELETE") == _nPos)
        return false;
    
    //verifica se tem espaço após o metodo
    size_t spacePos = _firstLine.find(' ');
    if(spacePos == _nPos || spacePos == _firstLine.size() - 1 )
        return false;
    
    //valida HTTP após o metodo e espaço
    std::string version = _firstLine.substr(spacePos + 1);
    if(version.find("HTTP/1.1") == _nPos)
        return false;
    
    //valida espaço apos a versão
    spacePos = version.find(' ');
    if(spacePos == _nPos || spacePos == version.size() - 1)
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
        throw WebServException();
    }
    
    //habilitar o socket para aguardar conexões de entrada. ) 5 representa o tamanho máximo da fila de conexões pendentes.
    if(listen(_serverSocket[serverIndex].getWebServSocket(), 5) == -1){
        close(_serverSocket[serverIndex].getWebServSocket());
        throw WebServException();
    }
    std::cout << "--------- Socket atual colocado pra escutar..." << std::endl;
}


void WebServ::mainLoop(){
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Servidor iniciado. Aguardando conexões..." << std::endl;
    std::cout << "-----------------------------------------\n" << std::endl;

    const int maxEvents = 64;
    struct epoll_event events[maxEvents];
    
    
    //Imprimir detalhes de cada servidor
    for (size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex){
        //int server_socket = _serverSocket[serverIndex].getWebServSocket();
        std::cout << "Detalhes do servidor " << serverIndex << ":" << std::endl;
        std::cout << "Porta: " << _serverSocket[serverIndex].getPort() << std::endl;
        std::cout << "Endereço: " << _serverSocket[serverIndex].getAddress() << std::endl;
        std::cout << "-----------------------------------------\n" << std::endl;
    }
    //abre o loop proncipal e aceita novas conexões
    for (size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex){
        while(true){
            int numEvents = epoll_wait(_epollFD, events, maxEvents, -1);
            if(numEvents == -1){
                if(errno == EINTR)
                    continue;
                perror("epoll_wait");
                exit(EXIT_FAILURE);
            }
            for(int i = 0; i < numEvents; ++i){
                int fd = events[i].data.fd;
                if(fd == _serverSocket[0].getWebServSocket()){
                    //aceita nova conexão
                    struct sockaddr_in client_address = {0};
                    _clientAddressLength = sizeof(client_address);
                    _clientSocket = accept(_serverSocket[serverIndex].getWebServSocket(),(struct sockaddr *)&client_address, &_clientAddressLength);
                    if(_clientSocket == -1){
                        std::cerr << "Erro ao aceitar a conexão" << std::endl;
                        continue;
                    }
                    // configura cliente nao bloqueante
                    int flags = fcntl(_clientSocket, F_GETFL, 0);
                    if(flags == -1){
                        close(_clientSocket);
                        perror("fcntl F_GETFL");
                        continue;
                    }
                    flags |= O_NONBLOCK;
                    //add client socket to epoll
                    if (fcntl(_clientSocket, F_SETFL, flags) == -1) {
                        close(_clientSocket);
                        perror("fcntl F_SETFL");
                        continue;
                    }
                    // Add client socket to epoll
                    struct epoll_event event;
                    event.events = EPOLLIN | EPOLLET;
                    event.data.fd = _clientSocket;
                    if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, _clientSocket, &event) == -1) {
                        perror("epoll_ctl EPOLL_CTL_ADD");
                        close(_clientSocket);
                        continue;
                    }
                } else {
                    //Recebe solicitação
                    char buffer[1024];
                    ssize_t bytesRead = recv(_clientSocket, buffer, sizeof(buffer), 0); //peguei a request
                    if(bytesRead <= 0){
                        std::cerr << "Erro ao receber solicitação do client " << std::endl;
                        close(fd);//continue;
                    } else {
                        std::string request(buffer, bytesRead); //buffer contém os dados recebidos do client
                        printRequest(request);
                        std::istringstream requestStream(request); //pega a 
                        std::string _firstLine;
                        std::getline(requestStream, _firstLine);
                        bool hasError = false;
                        if(!isFirstLineValid(request, _firstLine)){
                            hasError = true;
                            close(_clientSocket);
                        }
                        if(hasError)
                            responseError();
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
                        if (bytesSent == -1)
                            std::cerr << "Erro ao enviar a resposta ao cliente" << std::endl;
                        std::cout << "\n---------------------------------------" << std::endl;
                        std::cout << "----- FECHOU A CONEXÃO COM O CLIENTE ----" << std::endl;
                        std::cout << "-----------------------------------------" << std::endl;
                        close(_clientSocket);
                    }
                } //fim do else/if
            }// fim do for depois do while
        }// fim do while
    }//fim do for antes do while
    for (size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex)
    close(_serverSocket[serverIndex].getWebServSocket());
    return;
}

/*
    g++ -std=c++98 -I inc/ src/main.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp -o executavel
    ./executavel src/config.txt

*/
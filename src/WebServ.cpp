/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/07 21:52:25 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"
#include "../inc/WebServ.hpp"
#include "../inc/Response.hpp"
#include "../inc/CGI.hpp"

WebServ::WebServ(){}

WebServ::~WebServ(){}
                
//----------------17.08.2023
WebServ::WebServ(std::string filename){

    std::ifstream fileToParse;
	size_t index = 0;
    fileToParse.open(filename.c_str());
    if(fileToParse.is_open()){
        std::string line;
        bool isLocationBlock = false;
        while(getline(fileToParse, line)){
            if (line.find("server {") != std::string::npos){
                std::cout << "Index: " << index << std::endl;
                if (index > 0)
                    configSocket(index - 1);
				index++;
            }
			else if (line.find("location") != std::string::npos || isLocationBlock == true){
                if (line.find("}") != std::string::npos){
                    isLocationBlock = false;
					continue;
				}
                isLocationBlock = true;
				std::cout << "LINHA: " << line << std::endl;
                _configParser.processLocation(line);
            } // configSocket(index);
            else if (line.find("listen") != std::string::npos){
                _configParser.processListen(line);
            }else if (line.find("server_name") != std::string::npos){
                _configParser.processServerName(line);
            }else if (line.find("root") != std::string::npos){
                _configParser.processRoot(line);
            }else if (line.find("index") != std::string::npos){
                _configParser.processIndex(line);
			}else if (line.find("ssl on") != std::string::npos){
				_configParser.processSSL(line);
			}else if (line.find("allow_methods") != std::string::npos){
				_configParser.processAllowMethods(line);
			}else if (line.find("client_max_body_size") != std::string::npos){
				_configParser.processClientMaxBodySize(line);
			}else if (line.find("return") != std::string::npos){
				_configParser.processReturn(line);
			}
        }
    }else
        std::cout << "[Error] : file cannot be opened" << std::endl;
    if (index != 0)
        configSocket(index - 1);
    fileToParse.close();
    mainLoop();
}

void WebServ::configSocket(size_t serverIndex){
    SocketS temp_socket;
    //_serverSocket[serverIndex].setPort(_configParser.getPort());
    //_serverSocket[serverIndex].setAddress(_configParser.getAddress());
    temp_socket.setPort(_configParser.getPort());
    temp_socket.setAddress(_configParser.getAddress());
	//adicionar o _locations do ConfigParser ao _locations do temp_socket
	temp_socket.setLocations(_configParser.getLocations());
	
    temp_socket.setMethods(_configParser.getMethods());
    temp_socket.setRoot(_configParser.getRoot());

    _serverSocket.push_back(temp_socket);
    std::cout << "Port: " << _serverSocket.back().getPort() << std::endl;
    std::cout << "Address: " << _serverSocket.back().getAddress() << std::endl;

	// iterar o map do Locations pra verificar os valores
	std::map<std::string, LocationDirective>::iterator it = _serverSocket.back().getLocations().begin();
	int size_locations =  _serverSocket.back().getLocations().size();
	std::cout << "---- Locations desse server ----" << std::endl;
	std::cout << "Size Locations: " << _serverSocket.back().getLocations().size() << std::endl;
	for ( int monitor_locations = 0; monitor_locations < size_locations; ++monitor_locations) {
        std::cout << "Chave: " << it->first << std::endl;
		++it;
    }
	std::cout << "--------------------------------" << std::endl;
	
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
    } //std::cout << "--------- Socket atual colocado pra escutar..." << std::endl;
}

void WebServ::printRequest(const std::string& request)
{
    std::istringstream iss(request);//permite tratar a string request como um fluxo de entrada
    std::string line;
    while(std::getline(iss, line) && line != "\r")
        std::cout << line << std::endl;
}

bool WebServ::isFirstLineValid(const std::string& request, std::string& _firstLine)
{
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

void WebServ::responseError()
{
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

bool contains(const std::vector<std::string>& vec, const std::string& content) {
    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i] == content) {
            return true;
        }
    }
    return false;
}

// divisao da mainLoop em 31.08.2023

Epoll& WebServ::getEpollS()
{
    return _epollS;
}
// fim da divisao da mainLoop em 31.08.2023  

bool WebServ::isEventFromServerSocket(struct epoll_event* events, int index)
{
    _epollS.setIsServerFdTriggered(false);
    for (size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex)
    {
        if (events[index].data.fd == _serverSocket[serverIndex].getWebServSocket())
        {
            getEpollS().setIsServerFdTriggered(true); // Retorna true se encontrar um socket do servidor
                break;
        }
    }
    return false; // Retorna false se não encontrar nenhum socket do servidor
}

void WebServ::handleRequest(int clientSocket, char* buffer, ssize_t bytesRead, std::string& requestString)
{
    Request request(requestString);
    if (!request.isFirstLineValid())
    {
        request.setHasError(true);
        close(clientSocket);
        //return;
    }
    if (request.getHasError())
    {
        responseError();
        //return;
    }
    request.validateRequest();

    Response currentResponse;
    std::cout << "--------------------********---------" << std::endl;
    std::cout << "Dominio da request: " << request.getDomainRequest() << "| Size: " << request.getDomainRequest().size() << std::endl;
    std::cout << "--------------------********---------" << std::endl;

    int selectedServer = currentResponse.selectServer(request, _serverSocket);
    if (selectedServer != -1)
    {
        std::cout << "OLHA QUE ELE CHEGOU NUM SERVER HEIN" << std::endl;
				std::cout << "------------------------------------" << std::endl;
				std::cout << "------------------------------------" << std::endl;
        currentResponse.buildResponse(request, _serverSocket[selectedServer]);
        std::string response = currentResponse.getResponse();
        std::cout << "RESPONSE DO BUILDER:  " << response << std::endl;
        ssize_t bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
        if (bytesSent == -1)
        {
            std::cerr << "Erro ao enviar a resposta ao cliente" << std::endl;
        }
    } 
    else
    {
        std::cout << "Server não encontrado!" << std::endl;
    }
    close(clientSocket);
}


void WebServ::mainLoop(){
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Servidor iniciado. Aguardando conexões..." << std::endl;
    std::cout << "-----------------------------------------\n" << std::endl;

    _epollS.addServersToEpoll(_serverSocket);
    int epollFd = _epollS.getEpollFd();
    if (epollFd == -1 || epollFd == -2)
        return;
    const int maxEvents = _epollS.getMaxEvents();
    struct epoll_event events[maxEvents];

    while(true)
    {
        _epollS.setNumberEvents(epoll_wait(epollFd, events, maxEvents, -1));
        if (_epollS.getNumberEvents() == -1) {
            perror("Error in epoll_wait");
            return;
        }
        for (int index = 0; index < _epollS.getNumberEvents(); ++index)
        {
            isEventFromServerSocket(events,index);

            if (_epollS.getIsServerFdTriggered() == true)
            {
                int result = _epollS.addNewClientToEpoll(events, index);
                if (result == -3)
                    continue;
            }
            else
            {
				std::cout << "=================================================" << std::endl;
				std::cout << "RECEBENDO REQUISICAO" << std::endl;
				std::cout << "=================================================" << std::endl;
                int clientSocket = events[index].data.fd;
                char buffer[1024];
				// ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
				std::string toBeRequest;
				ssize_t bytesRead;
				while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) { //isso nao tá dando muito certo.. tem que dar uma olhada no Kaydoo como ele fez....
        			// Adicione os dados lidos à string toBeRequest.
					std::cout << "ENTRAMOS NO LOOP DE LEITURA" << std::endl;
        			toBeRequest.append(buffer, bytesRead);
					std::cout << "Quantidade lida até agora: " << bytesRead << std::endl;
    			}
				std::cout << "Quantidade lida depois de terminar o loop: " << bytesRead << std::endl;
				// extrair o tamanho do content-length e comparar com bytesRead pra validar que ja leu tudo dessa request
				std::string requestString(toBeRequest);
				std::cout << std::endl;
				std::cout << std::endl;
				std::cout << "TAMANHO DA REQUEST GUARDADA::: " << requestString.size() << std::endl;
				if (requestString.size() <= 1204)
					std::cout << requestString << std::endl;
				else
					std::cout << requestString.substr(0, 800) << std::endl;
				std::cout << std::endl;
				std::cout << std::endl;
                printRequest(requestString);
				

                // if(bytesRead <= 0)
                // {
                //     std::cerr << "Erro ao receber solicitação do client" << std::endl;
                //     continue;
                // }
				std::cout << "DEU CERTOOOOOOOOO ------------------------>>>>>" << std::endl;
				// std::cout << "To Be Request: " << std::endl;
				// std::cout << toBeRequest << std::endl;
				// std::cout << std::endl;
				// std::cout << std::endl;
                // std::string requestString(buffer, bytesRead);
				// std::string requestString(toBeRequest);
                // printRequest(requestString);
                handleRequest(clientSocket, buffer, bytesRead, requestString);
                std::cout << "\n---------------------------------------" << std::endl;
                std::cout <<     "----- FECHOU A CONEXÃO COM O CLIENTE ----" << std::endl;
                std::cout << "-----------------------------------------" << std::endl;  
            }
        }
    }
    for (size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex)

        close(_serverSocket[serverIndex].getWebServSocket());
}


/*
    g++ -std=c++98 -I inc/ src/main.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp src/LocationDirective.cpp src/Epoll.cpp src/Request.cpp src/Response.cpp src/CGI.cpp -o executavel_com_response
    ./executavel src/config.txt

*/
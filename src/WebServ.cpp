/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/13 23:06:15 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"
#include "../inc/WebServ.hpp"
#include "../inc/Response.hpp"
#include "../inc/CGI.hpp"
#include "../inc/ConfigParser.hpp"


WebServ::WebServ(){}

WebServ::~WebServ(){}

WebServ::WebServ(std::string filename){

    std::ifstream fileToParse;
	size_t index = 0;
    fileToParse.open(filename.c_str());
    char contentFile;

    if(!(fileToParse >> contentFile)){
        fileToParse.close();
        throw ErrorException("Configuration Error: empty file!");
    }
    fileToParse.unget(); //funcao que desfaz a leitura do conteudo.
    _configParser.validateFile(fileToParse);

    if(fileToParse.is_open()){
        std::string line;
        bool isLocationBlock = false;
        bool isInsideServerBlock = false;
        while(getline(fileToParse, line)){
            _configParser.trimWhiteSpace(line); //trima espaços em branco
			size_t startPos = line.find_first_not_of(" \t");
    		if (startPos == std::string::npos) //quer dizer que ou só tem espaço ou tab, nao tem conteudo, entao pode pular pra proxima iteração
				continue;
            _configParser.removeComments(line); //remove comentarios
            size_t semicolon = line.find_last_not_of(" \t"); //remove semicolon
            if(semicolon != std::string::npos && line[semicolon] == ';')
                line = line.substr(0, semicolon);   
            if (line.find("server{") != std::string::npos){
				// verifica que o arquivo nao está tentando abrir outro server dentro de um bloco de server (ou dentro de um bloco de location)..
				if (isInsideServerBlock == true || isLocationBlock == true)
					throw ErrorException("Configuration Error: Server Block INSIDE another Server or Location block!");
                std::cout << "Index: " << index << std::endl;
                if (index > 0)
                    configSocket(index - 1);
				index++;
			    isInsideServerBlock = true; // Entramos em um bloco "server"
            }
            else if (!isInsideServerBlock && !isLocationBlock && !line.empty())
                throw ErrorException("Wrong character out of server scope: " + line);
            else if (line.find("}") != std::string::npos && isLocationBlock == false) {
                isInsideServerBlock = false; // Saímos do bloco "server"
                continue;
            }
            else if(isInsideServerBlock){
    			if (line.find("location") != std::string::npos || isLocationBlock == true){
                    if (line.find("}") != std::string::npos){
                        isLocationBlock = false;
    					continue;
    				}
                    isLocationBlock = true;
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
    			}else if (line.find("error_page") != std::string::npos){
                    _configParser.processErrorPage(line);
                }
				else
				{
					std::cout << line << std::endl;
					throw ErrorException("Configuration Error: Directive not allowed!");
				}
            }
        }
        if(index == 0){
            throw ErrorException("Configuration Error: Server Block not found!");
        }
		if (isLocationBlock == true || isInsideServerBlock == true)
			throw ErrorException("Configuration Error: Server Block or Location Block not closed!");
    }else
        throw ErrorException("File Error : file cannot be opened");
    if (index != 0)
        configSocket(index - 1);
    fileToParse.close();
	checkForDuplicates();
	mainLoop();
}

void WebServ::checkForDuplicates()
{
    for(size_t i = 0; i < _serverSocket.size() - 1; ++i){
        for(size_t j = i + 1; j < _serverSocket.size(); ++j){
            if(_serverSocket[i].getPort() == _serverSocket[j].getPort() &&
               _serverSocket[i].getAddress() == _serverSocket[j].getAddress()){
				std::vector<std::string> currentSocketServerNames = _serverSocket[i].getServerName();
				std::vector<std::string> nextSocketServerNames = _serverSocket[j].getServerName();
				for (size_t k = 0; k < currentSocketServerNames.size(); ++k){
					for (size_t l = 0; l < nextSocketServerNames.size(); ++l){
						if (currentSocketServerNames[k] == nextSocketServerNames[l])
							throw ErrorException("Configuration Error : Duplicated server_name in multiple servers");
                    }
				}
			}
        }
    }
}

void WebServ::configSocket(size_t serverIndex)
{
	// antes de passar os itens do _configParser para o socket, verificar que todas as diretivas mandatórias estão preenchidas..
	/* modelo:
		if (server.getRoot().empty())
			server.setRoot("/;");
		if (server.getHost() == 0)
			server.setHost("localhost;");
		if (server.getIndex().empty())
			server.setIndex("index.html;");
		if (ConfigFile::isFileExistAndReadable(server.getRoot(), server.getIndex()))
			throw ErrorException("Index from config file not found or unreadable");
		if (server.checkLocaitons())
			throw ErrorException("Locaition is duplicated"); // ja ta resolvido no processLocation, certo?
		if (!server.getPort())
			throw ErrorException("Port not found");
		server.setErrorPages(error_codes);
		if (!server.isValidErrorPages())
			throw ErrorException("Incorrect path for error page or number of error");
	*/
	if (_configParser.getRoot().empty())
		_configParser.setRoot("./");
	if (_configParser.getAddress().empty())
		_configParser.setAddress("localhost"); //ver se isso já não está sendo resolvendo no processListen
	//definir index, caso não tenha encontrado (e armazenar o que é encontrado no processIndex na classe)
	// verificar tambem se o arquivo desse index existe e é acessivel
	if (_configParser.getIndexFiles().size() == 0)
		_configParser.setIndexFiles("index.html");
	if (_configParser.getPort().empty())
		throw ErrorException("Configuration Error: Port not found!");
	// tem que setar as errorPages e verificar a existencia e acessibilidade delas...
	// o client_max_body_size vai ser OBRIGATÓRIO ou, se nao houver no nivel server, a gente vai definir um padrão? (ou deixar sem?)


	// passagem do conteudo do configParser pro temp_socket
    SocketS temp_socket;
    temp_socket.setPort(_configParser.getPort());
    temp_socket.setAddress(_configParser.getAddress());
	//adicionar o _locations do ConfigParser ao _locations do temp_socket
	temp_socket.setLocations(_configParser.getLocations());
    temp_socket.setMethods(_configParser.getMethods());
    temp_socket.setRoot(_configParser.getRoot());
	// nós resetamos os bools do _configParser pra false // agora reseta tudo
	_configParser.resetConfig();

	// ------------------------------------------------
    _serverSocket.push_back(temp_socket);

	// iterar o map do Locations pra verificar os valores
	std::map<std::string, LocationDirective>::iterator it = _serverSocket.back().getLocations().begin();
	int size_locations =  _serverSocket.back().getLocations().size();
    //cria socket -----> deixa isso em outra função pra daí poder avaliar se tem server escutando no msmo ip:port e só fazer bind de um e replicar o fd pro outro?
    _serverSocket[serverIndex].setWebServSocket(socket(AF_INET, SOCK_STREAM, 0));//int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(_serverSocket[serverIndex].getWebServSocket() == -1){
        throw ErrorException("Socket Error: Failed to create socket!");
    }
    //configura endereço do servidor e inicializa os campos da estrutura com 0
    struct sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET; //socket usará os ends. IPv4
    server_address.sin_port = htons(std::atoi(_configParser.getPort().c_str())); //usa a função htons para converter8080 para a ordem de bytes da rede e atribui a sin_port
    server_address.sin_addr.s_addr = INADDR_ANY; //especifica o end.IP que o socket do server será vinculado
    //chamada para o bind - vincula o socket ao endereço e porta, 0 -1 tem haver com a falha na chamada do bind
    if(bind(_serverSocket[serverIndex].getWebServSocket(),(struct sockaddr*)&server_address, sizeof(server_address)) == -1){
        close(_serverSocket[serverIndex].getWebServSocket());
          throw ErrorException("Socket Error: Bind failed!");
    }
    //habilitar o socket para aguardar conexões de entrada. ) 5 representa o tamanho máximo da fila de conexões pendentes.
    if(listen(_serverSocket[serverIndex].getWebServSocket(), 5) == -1){
        close(_serverSocket[serverIndex].getWebServSocket());
        throw ErrorException("Socket Error: Listen failed!");
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

bool WebServ::isEventFromServerSocket(struct epoll_event* events, int index) {
    _epollS.setIsServerFdTriggered(false);
    for (size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex) {
        if (events[index].data.fd == _serverSocket[serverIndex].getWebServSocket()) {
            getEpollS().setIsServerFdTriggered(true); // Retorna true se encontrar um socket do servidor
                break;
        }
    }
    return false; // Retorna false se não encontrar nenhum socket do servidor
}

void WebServ::handleRequest(int clientSocket, char* buffer, ssize_t bytesRead, std::string& requestString) {
    Request request(requestString);
    if (!request.isFirstLineValid()) {
        request.setHasError(true);
        close(clientSocket);
    }
    if (request.getHasError())
        responseError();
    request.validateRequest();
    Response currentResponse;
    std::cout << "--------------------********---------" << std::endl;
    std::cout << "Dominio da request: " << request.getDomainRequest() << "| Size: " << request.getDomainRequest().size() << std::endl;
    std::cout << "--------------------********---------" << std::endl;
    int selectedServer = currentResponse.selectServer(request, _serverSocket);
    if (selectedServer != -1) {
        currentResponse.buildResponse(request, _serverSocket[selectedServer]);
        std::string response = currentResponse.getResponse();
        ssize_t bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
        if (bytesSent == -1)
            throw ErrorException ("Erro ao enviar a resposta ao cliente");
    } 
    else
        std::cout << "Server não encontrado!" << std::endl;
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

    while(true) {
        _epollS.setNumberEvents(epoll_wait(epollFd, events, maxEvents, -1));
        if (_epollS.getNumberEvents() == -1) {
            perror("Error in epoll_wait");
            return;
        }
        for (int index = 0; index < _epollS.getNumberEvents(); ++index) {
            isEventFromServerSocket(events,index);
            if (_epollS.getIsServerFdTriggered() == true) {
                int result = _epollS.addNewClientToEpoll(events, index);
                if (result == -3)
                    continue;
            } else { //quarta: temos que ldiar com as requests pra GET e provavelmente fazer um objeto pro client - guardando informação de em que server ele está bindando... (pesquisar mais com o chat)
                int clientSocket = events[index].data.fd;
                char buffer[1024];
                ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                if(bytesRead <= 0) {
                    std::cerr << "Erro ao receber solicitação do client " << std::endl;
                    continue;
                }
                std::string requestString(buffer, bytesRead);
                printRequest(requestString);
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
    g++ -std=c++98 -I inc/ src/main.cpp src/CGI.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp src/LocationDirective.cpp src/Epoll.cpp src/Request.cpp src/Response.cpp -o executavel_com_response
    ./executavel src/config.txt
*/
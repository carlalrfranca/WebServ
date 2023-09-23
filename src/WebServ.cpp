/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/23 15:18:09 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/WebServ.hpp"

WebServ::WebServ(){}

WebServ::~WebServ(){}

WebServ::WebServ(std::string filename)
{
    std::ifstream fileToParse;
	size_t index = 0;
    fileToParse.open(filename.c_str());
    char contentFile;
	_totalBytesRead = 0;
	_contentLength = 0;

    if(!(fileToParse >> contentFile))
    {
        fileToParse.close();
        throw ErrorException("Configuration Error: empty file!");
    }
    fileToParse.unget(); //funcao que desfaz a leitura do conteudo.
    _configParser.validateFile(fileToParse);

    if(fileToParse.is_open())
    {
        std::string line;
        bool isLocationBlock = false;
        bool isInsideServerBlock = false;
        while(getline(fileToParse, line))
        {
            _utils.trimWhiteSpace(line); //trima espaços em branco
			size_t startPos = line.find_first_not_of(" \t");
    		if (startPos == std::string::npos) //quer dizer que ou só tem espaço ou tab, nao tem conteudo, entao pode pular pra proxima iteração
				continue;
            _utils.removeComments(line); //remove comentarios
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
                    if (line.find("location") != std::string::npos && isLocationBlock == true)
						throw ErrorException("Configuration Error: Can't have location block inside of another location block");
					if (line.find("}") != std::string::npos){
                        isLocationBlock = false;
    					continue;
    				}         
                    _configParser.processLocation(line);
                    isLocationBlock = true;
                } // configSocket(index);
                else if (line.find("listen") != std::string::npos){
                    _configParser.processListen(line);
                }else if (line.find("server_name") != std::string::npos){
                    _configParser.processServerName(line);
                }else if (line.find("root") != std::string::npos){
                    _configParser.processRoot(line);
                }else if (line.find("index") != std::string::npos){
                    _configParser.processIndex(line);
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
	if (_configParser.getRoot().empty())
		_configParser.setRoot("./");
	if (_configParser.getAddress().empty())
		_configParser.setAddress("localhost");
	if (_configParser.getIndexFile().empty())
		_configParser.setIndexFile("index.html");
	if (_configParser.getPort().empty())
		throw ErrorException("Configuration Error: Port not found!");
	_configParser.checkDuplicatePorts();
	// o client_max_body_size vai ser OBRIGATÓRIO ou, se nao houver no nivel server, a gente vai definir um padrão? (ou deixar sem?)

	// verificar se esse server teve mais de um listen (ip e porta)...
	// se sim, cria um socket pra cada, cada um escutando em sua respectiva
	// porta, mas com o resto das configurações semelhantes...
	// ** ATENÇÃO: a MESMA porta não pode ter sido configurada DUAS VEZES (testar
	// pra ver se escutar na porta 5005 no ip 127.0.0.1 e na mesma porta do ip 128.??? consegue bindar)

    SocketS temp_socket;
    temp_socket.setPort(_configParser.getPort());
    temp_socket.setAddress(_configParser.getAddress());
	temp_socket.setLocations(_configParser.getLocations());
    temp_socket.setMethods(_configParser.getMethods());
    temp_socket.setRoot(_configParser.getRoot());
	temp_socket.setIndexFile(_configParser.getIndexFile());
	temp_socket.setErrorPage(_configParser.getErrorPage());
	_configParser.resetConfig();

    _serverSocket.push_back(temp_socket);

	// iterar o map do Locations pra verificar os valores
	// std::map<std::string, LocationDirective>::iterator it = _serverSocket.back().getLocations().begin();
	// int size_locations =  _serverSocket.back().getLocations().size();


	// colocar esse prcesso de criação de socket num outro método
	// quando já tiver o vetor de sockets
	// e daí ir validando e criando (sem bindar o mesmo ip:port duas vezes)
	
	//cria socket -----> deixa isso em outra função pra daí poder avaliar se tem server escutando no msmo ip:port e só fazer bind de um e replicar o fd pro outro?
	// A PARTIR DAQUI: tá na função initServer() do SocketS!!!! vai ser chamado depois, na initServers() 
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

// função pra inicializar os servers depois de todos já estarem setados no vetor
// (é aqui que vamos verificar se tem algum com ip:address repetida e só dar UM bind e compartilhar o fd!)
// criar essas estruturas e fazer o bind e o listen é algo que está num método próprio do server (SocketS)
/*

void Webserv::initServers()
{
	int i = 0;
	while (i < _serverSocket.size())
	{
		// aqui vai o if testando se a porta e o address é igual -> daí 
		// replica o fd deles (tem que ser com iteradores - consultar isso)
		// e continua;
		// senão:
		_serverSocket[i].initServer();
		i++;
	}
}

*/

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

Epoll& WebServ::getEpollS()
{
    return _epollS;
}

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
        _response = currentResponse.getResponse();
        // ssize_t bytesSent = send(clientSocket, _response.c_str(), _response.length(), 0);
        // if (bytesSent == -1)
        //     throw ErrorException ("Erro ao enviar a resposta ao cliente");
    } 
    else
        std::cout << "Server não encontrado!" << std::endl;
    // close(clientSocket);
}

void WebServ::readRequest(int clientSocket)
{
	char buffer[1000];
	_epollS._clientFd = clientSocket;
	// int contentLength = 0;
	// ah, por enquanto tá funcionando porque ele toda vez processa a requestString no cgi... individualmente..
	// mas tem que tratar os chunks direito...
	// std::string requestString;
	if (_requestString.size() > 0)
		std::cout << RED << "Request JÁ TEM COISA!!!" << END << std::endl;
	ssize_t totalBytesRead = 0;
	// loop para receber e acumular os chunks da solicitação
	ssize_t bytesRead = 0;
	// while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0)
	std::cout << BLUE << "ENTERING READ REQUEST" << END << std::endl;
	while (true)
	{
		ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == 0)
		{
			std::cerr << "Client closed connection." << std::endl; //tem que tirar o client quando dá esse tipo de erro
			_epollS._event.data.fd = _epollS._clientFd;
			epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_DEL, _epollS._clientFd, &_epollS._event);
			close(_epollS._clientFd);
			return ;
		}
		else if (bytesRead == -1)
		{
			std::cerr << "Error during reading of client request: " << strerror(errno) << std::endl; //tem que tirar o client quando dá esse tipo de erro
			// _epollS._event.data.fd = _epollS._clientFd;
			// epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_DEL, _epollS._clientFd, &_epollS._event);
			// close(_epollS._clientFd);
			return ;
		}
		else if (bytesRead != 0)
		{
			_requestString.append(buffer, bytesRead);
			_totalBytesRead += bytesRead;
			std::cout << BLUE << "Total de Bytes lidos: " << _totalBytesRead << END << std::endl;
			memset(buffer, 0, sizeof(buffer));
		}
		// verificar num if se o recebimento da request tá completo
		// Agora temos a solicitação completa em requestString e podemos processá-la
		size_t headerEndPos = _requestString.find("\r\n\r\n");
		if (headerEndPos != std::string::npos) {
			std::cout << RED << "Encontrou fim do cabeçalho " << totalBytesRead << END << std::endl;
		    // O fim do cabeçalho foi encontrado.
		    // Agora você pode processar o cabeçalho separadamente do corpo da mensagem.
		    std::string header = _requestString.substr(0, headerEndPos + 4); // +4 para incluir "\r\n\r\n"
		    std::string body = _requestString.substr(headerEndPos + 4); // Restante da mensagem após o cabeçalho
			std::cout << YELLOW << "TAMANHO DA HEADER " << header.size() << END << std::endl;
		    // Faça algo com o cabeçalho e o corpo, se necessário.
			size_t contentLengthPos = header.find("Content-Length: ");
			if (contentLengthPos != std::string::npos)
			{
				std::cout << RED << "Encontrou Content-Length " << END << std::endl;
				// Agora, extraia o valor do Content-Length.
    			contentLengthPos += strlen("Content-Length: "); // Avance para o início do valor.
    			size_t lineEndPos = header.find("\r\n", contentLengthPos); // Encontre o final da linha.

    			if (lineEndPos != std::string::npos) {
    			    std::string contentLengthStr = header.substr(contentLengthPos, lineEndPos - contentLengthPos);
    			    // Agora, 'contentLengthStr' contém o valor do Content-Length como uma string.
					std::cout << RED << "Content-Length > " << contentLengthStr << END << std::endl;
    			    // Você pode converter 'contentLengthStr' para um valor numérico, se necessário.
    			    // Exemplo:
    			    try {
    			        _contentLength = std::atoi(contentLengthStr.c_str());
    			        // 'contentLength' agora contém o valor do Content-Length como um inteiro.
						
    			    } catch (const std::invalid_argument& e) {
    			        // Lida com erros de conversão, se necessário.
						std::cout << RED << "Erro de conversão -->>> " << e.what() << std::endl;
    			    }
    			}
				std::cout << BLUE << "Total lido até agora: " << _totalBytesRead << END << std::endl;
				std::cout << BLUE << "CONTENT LENGTH: " << _contentLength << END << std::endl;
				if ((_totalBytesRead - header.size())  == _contentLength)
				{
					std::cout << YELLOW << _contentLength << END << std::endl;
					std::cout << YELLOW << totalBytesRead << END << std::endl;
        			printRequest(_requestString);
					handleRequest(clientSocket, buffer, _requestString.length(), _requestString);
					_epollS._event.events = EPOLLOUT;
					epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_MOD, clientSocket, &_epollS._event);
					_requestString = "";
					_contentLength = 0;
					_totalBytesRead = 0;
				}
			}
			else {
				printRequest(_requestString);
				handleRequest(clientSocket, buffer, _requestString.length(), _requestString);
				_epollS._event.events = EPOLLOUT;
				epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_MOD, clientSocket, &_epollS._event);
				_requestString = "";
				_contentLength = 0;
				_totalBytesRead = 0;
				return ;
			}
		}
	}
}

void WebServ::mainLoop()
{
    std::cout << BLUE << "-----------------------------------------" << END << std::endl;
    std::cout << BLUE << "Servidor iniciado. Aguardando conexões..." << END << std::endl;
    std::cout << BLUE << "-----------------------------------------\n" << END << std::endl;
    _epollS.addServersToEpoll(_serverSocket);
    int epollFd = _epollS.getEpollFd();
    if (epollFd == -1 || epollFd == -2)
        return;
    const int maxEvents = _epollS.getMaxEvents();
    struct epoll_event events[maxEvents];

    while(true)
    {
        _epollS.setNumberEvents(epoll_wait(epollFd, events, maxEvents, -1));
        if (_epollS.getNumberEvents() == -1)
        {
            perror("Error in epoll_wait");
            return;
        }
        for (int index = 0; index < _epollS.getNumberEvents(); ++index)
        {
			int current_fd = events[index].data.fd;
			isEventFromServerSocket(events,index);
			if (events[index].events & EPOLLIN)
			{
            	if (_epollS.getIsServerFdTriggered() == true)
            	{
            	    int result = _epollS.addNewClientToEpoll(events, index);
            	    if (result == -3)
            	        continue;
            	} else { //quarta: temos que ldiar com as requests pra GET e provavelmente fazer um objeto pro client - guardando informação de em que server ele está bindando... (pesquisar mais com o chat)
            	    int clientSocket = events[index].data.fd;
					readRequest(clientSocket);
					/*
					char buffer[150];
					_epollS._clientFd = clientSocket;
					std::string requestString;
					ssize_t totalBytesRead = 0;
					// loop para receber e acumular os chunks da solicitação
					ssize_t bytesRead = 0;
					// while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0)
					while (true)
					{
						ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            	        if (bytesRead == 0)
						{
							std::cerr << "Client closed connection." << std::endl; //tem que tirar o client quando dá esse tipo de erro
							_epollS._event.data.fd = _epollS._clientFd;
							epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_DEL, _epollS._clientFd, &_epollS._event);
							close(_epollS._clientFd);
							break;
						}
						else if (bytesRead == -1)
						{
							std::cerr << "Error during reading of client request: " << strerror(errno) << std::endl; //tem que tirar o client quando dá esse tipo de erro
							// _epollS._event.data.fd = _epollS._clientFd;
							// epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_DEL, _epollS._clientFd, &_epollS._event);
							// close(_epollS._clientFd);
							break;
						}
						requestString.append(buffer, bytesRead);
						totalBytesRead += bytesRead;
						std::cout << BLUE << "Total de Bytes lidos: " << totalBytesRead << END << std::endl;
					}
					// Agora temos a solicitação completa em requestString e podemos processá-la
					printRequest(requestString);
					handleRequest(clientSocket, buffer, requestString.length(), requestString);

					_epollS._event.events = EPOLLOUT;
					epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_MOD, clientSocket, &_epollS._event);
					*/
					// handleRequest(clientSocket, buffer, bytesRead, requestString);
            	    // std::cout << BLUE << "\n---------------------------------------" << END << std::endl;
            	    // std::cout << BLUE <<     "----- FECHOU A CONEXÃO COM O CLIENTE ----" << END << std::endl;
            	    // std::cout << BLUE << "-----------------------------------------" << END << std::endl;
            	}
			}
			else if (events[index].events & EPOLLOUT)
			{
				ssize_t bytesSent = send(_epollS._clientFd, _response.c_str(), _response.length(), 0);
        		if (bytesSent == -1)
            		throw ErrorException ("Erro ao enviar a resposta ao cliente");
				_epollS._event.data.fd = _epollS._clientFd;
				epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_DEL, _epollS._clientFd, &_epollS._event);
				close(_epollS._clientFd);
				std::cout << BLUE << "\n---------------------------------------" << END << std::endl;
            	std::cout << BLUE <<     "----- FECHOU A CONEXÃO COM O CLIENTE ----" << END << std::endl;
            	std::cout << BLUE << "-----------------------------------------" << END << std::endl;
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
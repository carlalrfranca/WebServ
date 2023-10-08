/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/07 21:48:34 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/WebServ.hpp"

WebServ::WebServ()
{}

WebServ::~WebServ()
{

    // Libere quaisquer outros recursos alocados dinamicamente ou que exijam limpeza especial
    // Certifique-se de revisar todos os membros da classe e chamar os destrutores apropriados
    // ou realizar outras ações de limpeza, se necessário.

    // Exemplo: Chame o destrutor da ConfigParser, se necessário
    // _configParser.~ConfigParser();

    // Exemplo: Limpe o vetor _serverSocket, se necessário
    _serverSocket.clear();
}

void WebServ::setResponse(const std::string& response)
{
    _response = response;
}

std::string WebServ::getResponse()const 
{
    return _response;
}

WebServ::WebServ(std::string filename)
{
    std::ifstream fileToParse;
	size_t index = 0;
    fileToParse.open(filename.c_str());
    char contentFile;
	std::cout << BLUE << "Qtdade servers inicio: " << _serverSocket.size() << END << std::endl;
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
                    configSocket();
				index++;
			    isInsideServerBlock = true; // Entramos em um bloco "server"
            }
            else if(!isInsideServerBlock && !isLocationBlock && !line.empty())
                throw ErrorException("Wrong character out of server scope: " + line);
            else if(line.find("}") != std::string::npos && isLocationBlock == false)
            {
                isInsideServerBlock = false; // Saímos do bloco "server"
                continue;
            }
            else if(isInsideServerBlock)
            {
    			if(line.find("location") != std::string::npos || isLocationBlock == true)
    			{
                    if(line.find("location") != std::string::npos && isLocationBlock == true)
						throw ErrorException("Configuration Error: Can't have location block inside of another location block");
					if(line.find("}") != std::string::npos)
					{
                        isLocationBlock = false;
    					continue;
    				}         
                    _configParser.processLocation(line);
                    isLocationBlock = true;
                } // configSocket(index);
                else if(line.find("listen") != std::string::npos)
                    _configParser.processListen(line);
                else if(line.find("server_name") != std::string::npos)
                    _configParser.processServerName(line);
                else if(line.find("root") != std::string::npos)
                    _configParser.processRoot(line);
                else if(line.find("index") != std::string::npos)
                    _configParser.processIndex(line);
    			else if(line.find("allow_methods") != std::string::npos)
    				_configParser.processAllowMethods(line);
    			else if(line.find("client_max_body_size") != std::string::npos)
    				_configParser.processClientMaxBodySize(line);
    			else if(line.find("return") != std::string::npos)
    				_configParser.processReturn(line);
    			else if(line.find("error_page") != std::string::npos)
                    _configParser.processErrorPage(line);
                else {
					std::cout << line << std::endl;
					throw ErrorException("Configuration Error: Directive not allowed!");
				}
            }
        }
        if(index == 0)
            throw ErrorException("Configuration Error: Server Block not found!");
		if(isLocationBlock == true || isInsideServerBlock == true)
			throw ErrorException("Configuration Error: Server Block or Location Block not closed!");
    } else
        throw ErrorException("File Error : file cannot be opened");
    if(index != 0)
        configSocket();
    fileToParse.close();
	// checkForDuplicates();
	initServers();
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
			else if (_serverSocket[i].getPort() == _serverSocket[j].getPort() &&
	               _serverSocket[i].getAddress() != _serverSocket[j].getAddress()){
					throw ErrorException("Configuration Error : Duplicated Port in multiple servers");
			}
        }
    }
}

void WebServ::configSocket()
{
	// antes de passar os itens do _configParser para o socket,
	// verificar que todas as diretivas mandatórias estão preenchidas..
	if(_configParser.getRoot().empty())
		_configParser.setRoot("./");
	if(_configParser.getAddress().empty())
		_configParser.setAddress("127.0.0.1");
	// if (_configParser.getIndexFile().empty())
		// _configParser.setIndexFile("index.html");
	if(_configParser.getAllPorts().size() == 0)
		throw ErrorException("Configuration Error: [MANDATORY DIRECTIVE] Port not found!");
	if (_configParser.getMethods().size() == 0)
		throw ErrorException("Configuration Error: [MANDATORY DIRECTIVE] It is necessary at least ONE allowed method at server level.");
	// o client_max_body_size vai ser OBRIGATÓRIO ou, se nao houver no nivel server,
	// a gente vai definir um padrão? (ou deixar sem?)
	std::vector<std::string> tmpPorts = _configParser.getAllPorts();
	std::vector<std::string> tmpAddress = _configParser.getAllIps();
	size_t totalPorts = tmpPorts.size();
	for(size_t i = 0; i < totalPorts; i++)
	{
    	SocketS temp_socket;
		temp_socket.setPort(tmpPorts[i]);
		temp_socket.setAddress(tmpAddress[i]);
	    // temp_socket.setPort(_configParser.getPort());
	    // temp_socket.setAddress(_configParser.getAddress());
		temp_socket.setLocations(_configParser.getLocations());
	    temp_socket.setMethods(_configParser.getMethods());
	    temp_socket.setRoot(_configParser.getRoot());
		temp_socket.setIndexFile(_configParser.getIndexFile());
		temp_socket.setErrorPage(_configParser.getErrorPage());
		temp_socket.setServerName(_configParser.getDomains());
		temp_socket.setMaxBodySize(_configParser.getMaxBodySize());
	    _serverSocket.push_back(temp_socket);
	}
	_configParser.resetConfig(); // O PROBLEMA DESSA MERDA É AQUI
	// iterar o map do Locations pra verificar os valores
	// std::map<std::string, LocationDirective>::iterator it = _serverSocket.back().getLocations().begin();
	// int size_locations =  _serverSocket.back().getLocations().size();
	
}

// função pra inicializar os servers depois de todos já estarem setados no vetor
// (é aqui que vamos verificar se tem algum com ip:address repetida e só dar UM bind e compartilhar o fd!)
// criar essas estruturas e fazer o bind e o listen é algo que está num método próprio do server (SocketS)


void WebServ::initServers()
{
	checkForDuplicates();
	bool isDuplicated = false;
	for (std::vector<SocketS>::iterator it = _serverSocket.begin(); it != _serverSocket.end(); ++it)
	{
		for (std::vector<SocketS>::iterator it2 = _serverSocket.begin(); it != it2; ++it2)
		{
			if (it2->getAddress() == it->getAddress() && it2->getPort() == it->getPort())
			{
				it->setWebServSocket(it2->getWebServSocket());
				isDuplicated = true;
			}
		}
		if (!isDuplicated)
			it->initServer();
	}
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

    if(_firstLine.find("GET") == std::string::npos && 
        _firstLine.find("POST") == std::string::npos && 
        _firstLine.find("DELETE") == std::string::npos)
        return false;
    size_t spacePos = _firstLine.find(' ');
    if(spacePos == std::string::npos || spacePos == _firstLine.size() - 1 )
        return false;
    std::string version = _firstLine.substr(spacePos + 1);
    if(version.find("HTTP/1.1") == std::string::npos)
        return false;
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
    else
        response = "HTTP/1.1 Error\r\n\r\n";
    ssize_t bytesSent = send(_clientSocket, response.c_str(), response.size(), 0);
    if(bytesSent < 0)
        std::cerr << "[Error] sending error response." << std::endl;
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

void WebServ::handleRequest(std::string& requestString)
{
    Request request(requestString);
    Response currentResponse;
	std::cout << BLUE << request.getMethod() << END << std::endl;
	int resultCode = request.validateRequest();
    if(resultCode != 0)
	{
		request.setHasError(true);
		_response = request.errorCodeHtml(resultCode);
		return ;
	}
    if(request.getHasError())
        responseError(); //criar um método pra errorCodeHtml aqui
    int selectedServer = UtilsResponse::selectServer(request, _serverSocket);
    if(selectedServer != -1)
	{
        currentResponse.buildResponse(request, _serverSocket[selectedServer]);
        _response = currentResponse.getResponse();
    } 
    else
        std::cout << "Server não encontrado!" << std::endl;
}

int	WebServ::convertContentLength(std::string& header, size_t contentLengthPos)
{
	contentLengthPos += strlen("Content-Length: "); // Avance para o início do valor.
    size_t lineEndPos = header.find("\r\n", contentLengthPos); // Encontre o final da linha.

    if(lineEndPos != std::string::npos)
	{
        std::string contentLengthStr = header.substr(contentLengthPos, lineEndPos - contentLengthPos);
        // Agora, 'contentLengthStr' contém o valor do Content-Length como uma string.
		// std::cout << RED << "Content-Length > " << contentLengthStr << END << std::endl;
        // Você pode converter 'contentLengthStr' para um valor numérico, se necessário.
        // Exemplo:
        try
		{
            _contentLength = std::atoi(contentLengthStr.c_str());
        } catch (const std::invalid_argument& e)
		{
            // Lida com erros de conversão, se necessário. O que fazer aqui? Erro de cabeçalho? Tem isso?
			std::cout << RED << "Erro de conversão -->>> " << e.what() << std::endl;
        }
    }
	return 0;
}

bool WebServ::hasBodyContent(const std::string& header)
{
	// retorna se há "content-length", ou seja, se a request tem um body sendo enviado além dos cabeçalhos
	size_t contentLengthPos = header.find("Content-Length: ");
	if(contentLengthPos != std::string::npos)
		return true;
	return false;
}

/*
	epollS._event.data.fd = epollS._clientFd;
	epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_DEL, epollS._clientFd, &epollS._event);
	close(epollS._clientFd);
*/

void WebServ::removeClientFromEpoll(Epoll& epollS)
{
	struct epoll_event event;
	
	event = epollS.getEvent();
	event.data.fd = epollS.getClientFd();
	epollS.setEvent(event);
	
	epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_DEL, epollS.getClientFd(), &event);
	close(epollS.getClientFd());
}

int WebServ::readRequest(int clientSocket)
{
	char buffer[1000];
	_epollS.setClientFd(clientSocket);
	std::string header;
	std::string body;
	int header_size = 0;
	// ah, por enquanto tá funcionando porque ele toda vez processa a requestString no cgi... individualmente..
	// mas tem que tratar os chunks direito...
	if(_requestString.size() > 0)
		std::cout << RED << "Request JÁ TEM COISA!!!" << END << std::endl;
	// loop para receber e acumular os chunks da solicitação
	//ssize_t bytesRead = 0;
	std::cout << BLUE << "ENTERING READ REQUEST" << END << std::endl;
	while(!shouldExit)
	{
		ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if(bytesRead == 0)
		{
			std::cerr << "Client closed connection." << std::endl; //tem que tirar o client quando dá esse tipo de erro
			removeClientFromEpoll(_epollS);
			return 0;
		}
		else if(bytesRead == -1)
		{
			// _requestString.append(buffer, bytesRead);
			// _totalBytesRead += bytesRead;
			// memset(buffer, 0, sizeof(buffer));
			std::cerr << "Error during reading of client request. " << std::endl; //tem que tirar o client quando dá esse tipo de erro
			removeClientFromEpoll(_epollS);
			std::cout << BLUE << _contentLength << END << std::endl;
			std::cout << BLUE << (_totalBytesRead - header_size) << END << std::endl;
			if (header_size != 0 && (_totalBytesRead - header_size) == _contentLength)
				std::cout << BLUE << "TERMINOU A REQUEST! Content-Length: " << _contentLength << " e total Lido: " << (_totalBytesRead - header_size )<< END << std::endl;
			// _epollS._event.data.fd = _epollS._clientFd;
			// epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_DEL, _epollS._clientFd, &_epollS._event);
			// close(_epollS._clientFd);
			return -1;
		}
		else if (bytesRead != 0)
		{
			std::cout << BLUE << "Bytes Read: " << bytesRead << END << std::endl;
			_requestString.append(buffer, bytesRead);
			_totalBytesRead += bytesRead;
			memset(buffer, 0, sizeof(buffer));
		}
		// Agora temos a solicitação completa em requestString e podemos processá-la
		size_t headerEndPos = _requestString.find("\r\n\r\n");
		if(headerEndPos != std::string::npos) {
			if(header.size() == 0)
			{
			    header = _requestString.substr(0, headerEndPos + 4); // +4 para incluir "\r\n\r\n"
				std::cout << YELLOW << header << END << std::endl;
			}
			if(_contentLength == 0 && hasBodyContent(header) == true)
			{
				size_t contentLengthPos = header.find("Content-Length: ");
				// Agora, extraia o valor do Content-Length.
				convertContentLength(header, contentLengthPos);
			}
			else if(hasBodyContent(header) == false)
				_contentLength = -1;
			struct epoll_event event;
			event = _epollS.getEvent();
			header_size = header.size();
			if((_totalBytesRead - header.size()) == _contentLength)
			{
				std::cout << YELLOW << _contentLength << END << std::endl;
				std::cout << YELLOW << _totalBytesRead << END << std::endl;
        		// printRequest(_requestString);
				handleRequest(_requestString);
				event.events = EPOLLOUT;
				epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_MOD, clientSocket, &event);
				_requestString = "";
				_contentLength = 0;
				_totalBytesRead = 0;
				return 0;
			}
			else if(_contentLength == (size_t)-1)
			{
				// printRequest(_requestString);
				handleRequest(_requestString);
				event.events = EPOLLOUT;
				epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_MOD, clientSocket, &event);
				_requestString = "";
				_contentLength = 0;
				_totalBytesRead = 0;
				return 0;
			}
			return 0;
		}
	}
	return 0;
}

void WebServ::mainLoop()
{
    std::cout << BLUE << "-----------------------------------------" << END << std::endl;
    std::cout << BLUE << "Servidor iniciado. Aguardando conexões..." << END << std::endl;
    std::cout << BLUE << "-----------------------------------------\n" << END << std::endl;
    _epollS.addServersToEpoll(_serverSocket);
    int epollFd = _epollS.getEpollFd();
    if(epollFd == -1 || epollFd == -2)
		return;
    const int maxEvents = _epollS.getMaxEvents();
    struct epoll_event events[maxEvents];
    while(!shouldExit)
    {
        _epollS.setNumberEvents(epoll_wait(epollFd, events, maxEvents, -1));
        if(_epollS.getNumberEvents() == -1)
        {
            perror("Error in epoll_wait");
            return;
        }
        for(int index = 0; index < _epollS.getNumberEvents(); ++index)
        {
			std::cout << RED << "Tem um NOVO EVENTO" << END << std::endl;
			isEventFromServerSocket(events,index);
			if(events[index].events & EPOLLIN)
			{
				std::cout << RED << "Onde entra o ultimo - 1" << END << std::endl;
            	if(_epollS.getIsServerFdTriggered() == true)
            	{
            	    int result = _epollS.addNewClientToEpoll(events, index);
            	    if (result == -3)
            	        continue;
            	} else {
					std::cout << RED << "Onde entra o ultimo - 2" << END << std::endl;
					//quarta: temos que ldiar com as requests pra GET e provavelmente fazer um objeto pro client
					// - guardando informação de em que server ele está bindando... (pesquisar mais com o chat)
            	    int clientSocket = events[index].data.fd;
					if (readRequest(clientSocket) == -1)
					{
						std::cout << RED << "Chegou ao final" << END << std::endl;
						std::cout << RED << _totalBytesRead << " | " << _contentLength << END << std::endl;
					}
            	}
			}
			else if(events[index].events & EPOLLOUT)
			{
				std::cout << "Comassim" << std::endl;
				ssize_t bytesSent = send(_epollS.getClientFd(), _response.c_str(), _response.length(), 0);
        		if(bytesSent == -1)
            		throw ErrorException ("Erro ao enviar a resposta ao cliente");
				removeClientFromEpoll(_epollS);
				std::cout << BLUE << "\n---------------------------------------" << END << std::endl;
            	std::cout << BLUE <<     "----- FECHOU A CONEXÃO COM O CLIENTE ----" << END << std::endl;
            	std::cout << BLUE << "-----------------------------------------" << END << std::endl;
			}
        }
    }
    for(size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex)
        close(_serverSocket[serverIndex].getWebServSocket());
}

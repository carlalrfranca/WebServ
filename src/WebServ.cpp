/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/24 16:48:31 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"

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
            if (line.find("server{") != std::string::npos){
                std::cout << "Index: " << index << std::endl;
                if (index > 0)
                    configSocket(index - 1);
				index++;
            }
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
			}else if (line.find("location") != std::string::npos || isLocationBlock == true){
                if (line.find("}") != std::string::npos){
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

std::string WebServ::executeScriptAndTakeItsOutPut(int *pipefd)
{
	pid_t childPid = fork();
	
	if (childPid == -1)
	{
		std::cerr << "ERROR creating CHILD PROCESS" << std::endl;
		return NULL;
	}
	else if (childPid == 0) //é processo filho
	{
		// por estarmos no processo filho nesse bloco, vamos então modificar o valor
		// do STDOUT pra poder redirecionar a saída do script pra cá
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]); //não vamos usar o pipe de leitura, então fechamos ele por boa convenção
		
		// Executamos agora o script de exemplo
		execl("./process_form.sh", "./process_form.sh", static_cast<char*>(0));
		// Se chegou aqui, houve um erro no execl
		std::cerr << "ERROR executing SCRIPT" << std::endl;
		return NULL;
	} else {
		// processo pai
		close(pipefd[1]); //nao vamos usar o fd de escrita, então o fechamos por boa convenção
		
		// Ler a saída do script CGI do pipe e armazená-la numa string
		std::string scriptOutPut;
		char buffer[1024]; //a saída crua terá que vir primeiro para um buffer
		while (true)
		{
			ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer)); //lê 1024 bytes do pipefd[0] pro buffer
			if (bytesRead <= 0)
				break;
			scriptOutPut.append(buffer, bytesRead);
		}
		close(pipefd[0]); //terminamos de ler da saída do script, então podemos fechar esse pipe
		
		// É importante colocarmos esse processo (pai) pra aguardar o término do processo filho
		int status;
		waitpid(childPid, &status, 0);
		// Agora a saída do script CGI está armazenada em 'scriptOutPut'
		std::cout << "------ SAÍDA DO SCRIPT --------\n" << scriptOutPut << std::endl;
		return scriptOutPut;
	}
}

std::string WebServ::handleCGIRequest(std::string& request)
{
	// primeiro criamos a header da response:
	std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

	/* 
		aqui vamos:
			- confirmar que o método é o POST (depois, transferir isso para um outro método, antes de
			chamar essa função aqui)
				> Se for, vamos resgatar o conteúdo enviado pelo formulário e passar para uma
				  função que cuidará disso (parseFormData) -> irá criar um map com os valores dos inputs do formulario
				> Vamos inserir esses valores na variavel de ambiente QUERY_STRING com setenv() - (para que o script possa acessar essas informações)
				> vamos abrir um pipe_fd pra leitura e escrita - e poder manipular o STDIN e STDOUT pra poder
				  "pegar" aqui no programa o que o script, por padrão, só pode escrever no terminal (STDOUT)
				> vamos fazer um fork() - criar um processo filho que irá modificar o STDOUT com dup e executará o script
				> vamos resgatar a saída do script e construir uma response pra retornar pra mainLoop()
	*/

	if (request.find("POST") != std::string::npos)
	{
		std::size_t data_init_pos = request.find("\r\n\r\n");
		if (data_init_pos != std::string::npos)
		{
			std::string inputData = request.substr(data_init_pos + 4);
			
			// setamos a env QUERY_STRING com esses valores do form
			setenv("QUERY_STRING", inputData.c_str(), 1);

			// criar o pipe pra redirecionar a saída do script pra poder resgatar pra cá
			int pipefd[2];
			if (pipe(pipefd) == -1)
			{
				std::cerr << "ERROR creating PIPE" << std::endl;
				return NULL;
			}
			std::cout << "----------- CRIOU O PIPE! -----------" << std::endl;
			std::string scriptOutPut;

			scriptOutPut = executeScriptAndTakeItsOutPut(pipefd);
			if (scriptOutPut.empty())
			{
				return NULL;
			}
			response += scriptOutPut;
			return response;
		}
	}
	else
	{
		// ou seja, não foi um 'POST'
		// retorna uma reponse só pra teste
		response += "<html><body><h1>Simple Form</h1><form method=\"post\">";
        response += "Name: <input type=\"text\" name=\"name\"><br>Email: <input type=\"text\" name=\"email\"><br>";
        response += "<input type=\"submit\" value=\"Submit\"></form></body></html>";
	}
	return response;
}

int WebServ::addServerToEpoll()
{
	int epollFd = epoll_create1(0);

	if (epollFd == -1)
	{
		perror("Error creating epoll");
		return -1;
	}
	// 

    //Imprimir detalhes de cada servidor
	struct epoll_event event;
    for (size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex){
        std::cout << "Detalhes do servidor " << serverIndex << ":" << std::endl;
        std::cout << "Porta: " << _serverSocket[serverIndex].getPort() << std::endl;
        std::cout << "Endereço: " << _serverSocket[serverIndex].getAddress() << std::endl;
		std::cout << "WebServ SOCKET FD: " <<  _serverSocket[serverIndex].getWebServSocket() << std::endl;
        std::cout << "-----------------------------------------\n" << std::endl;
		// try epoll
		event.data.u64 = 0;
        event.data.fd = _serverSocket[serverIndex].getWebServSocket();
        event.events = EPOLLIN | EPOLLET;
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, event.data.fd, &event) == -1) {
            perror("Error adding socket to epoll");
            return -2;
        }
    }
	return epollFd;
}

int WebServ::addNewClientToEpoll(struct epoll_event *event_ptr, int i, int epollFd)
{
	struct sockaddr_in clientAddress = {0};
	socklen_t clientAddressLength = sizeof(clientAddress);
	int clientSocket = accept(event_ptr[i].data.fd, (struct sockaddr*)&clientAddress, &clientAddressLength);
	
	if (clientSocket == -1) {
	    perror("Error accepting client connection");
	    return -3; // Move to the next event
	}
	
	// Set the client socket to non-blocking mode
	int flags = fcntl(clientSocket, F_GETFL, 0);
	fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);
	struct epoll_event event;
	event.data.u64 = 0;
	event.data.fd = clientSocket;
	event.events = EPOLLIN | EPOLLET; // Listen for read events in edge-triggered mode
	
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &event) == -1) {
	    perror("Error adding client socket to epoll");
	    close(clientSocket); // Close the socket on error
	}
	return 0;
}

void WebServ::mainLoop(){

        std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Servidor iniciado. Aguardando conexões..." << std::endl;
    std::cout << "-----------------------------------------\n" << std::endl;

	int epollFd = addServerToEpoll();
	if (epollFd == -1 || epollFd == -2)
	{
		return; //dar uma exceção de erro
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
			bool isServerFdTriggered = false;
			for (size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex)
			{
				if (events[i].data.fd == _serverSocket[serverIndex].getWebServSocket())
				{
					isServerFdTriggered = true;
					break;
				}
			}
				if (isServerFdTriggered == true)
				{
					int result = addNewClientToEpoll(events, i, epollFd);
					if (result == -3)
						continue;
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
            		printRequest(request);
            		std::istringstream requestStream(request); //pega a 
            		std::string _firstLine;
            		std::getline(requestStream, _firstLine);
            		bool hasError = false;
            		if(!isFirstLineValid(request, _firstLine)){
            		    hasError = true;
            		    close(clientSocket);
            		}
            		if(hasError)
            		    responseError();
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

					// para decidir que reponse vamos mandar, precisamos ver
					// que recurso a solicitação/request está pedindo
					size_t found = request.find("process_data.cgi");
					if (found != std::string::npos)
					{
						// lida com o cgi
						std::cout << "Recebeu solicitação para >> RECURSO CGI" << std::endl;
						std::string response = handleCGIRequest(request);
						send(clientSocket, response.c_str(), response.length(), 0);
					}
					else
					{
	            		//por enquanto, um response GENERICO só pra satisfazer o client
	            		const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><head><link rel='stylesheet' href='style.css'></head><body><h1>Hello World</h1></body></html>";
						ssize_t bytesSent = send(clientSocket, response, strlen(response), 0); //começa sempre pelo metodo: envia a reponse para o clienteSocket e retorna a quantidade de bytes.
						if (bytesSent == -1){
							std::cerr << "Erro ao enviar a resposta ao cliente" << std::endl;
						}
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
    g++ -std=c++98 -I inc/ src/main.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp src/LocationDirective.cpp -o executavel
    ./executavel src/config.txt

*/
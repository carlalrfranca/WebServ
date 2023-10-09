
#include <sys/socket.h>
#include <cerrno>


const int MAX_CLIENTS = 10;
const int MAX_BUFFER_size = 1024;

//p/config. socket nao-bloq
void acceptConnection()
{

}


//p/aceitar nova conexão
void setNonBlocking(int FD)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void WebServ::mainLoop()
{
    std::cout << BLUE << "-----------------------------------------" << END << std::endl;
    std::cout << BLUE << "Servidor iniciado. Aguardando conexões..." << END << std::endl;
    std::cout << BLUE << "-----------------------------------------\n" << END << std::endl;
    
    _epollS.addServersToEpoll(_serverSocket);
    int serverSocket = _serverSocket.getWebServerSocket();


    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(serverSocket, &readSet);
    int maxFD = serverSocket;
    while(!shouldExist)
    {
        int numReady = select(maxFD + 1, &readSet, NULL, NULL, NULL);
        if(numReady == -1)
        {
            perror("erro no select parsa!");
            return;
        }
        if(FD_ISSET(serverSocket, &readSet))
        {
            char buffer[1024];
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            close (clientSocket);
            it = _epollS.getClientSockets().erase(it);
        }else{


        }
    }


    int epollFd = _epollS.getEpollFd();
    if(epollFd == -1 || epollFd == -2)
		return;
    const int maxEvents = _epollS.getMaxEvents();
    struct epoll_event events[maxEvents];
    while(!shouldExit)
    {
        _epollS.setNumberEvents(epoll_wait(epollFd, events, maxEvents, -1));
		std::cout << YELLOW << "VAMOS PEGAR A QUANTIDADE DE EVENTOS EM ESPERA :: " << _epollS.getNumberEvents() << END << std::endl;
        if(_epollS.getNumberEvents() == -1)
        {
            perror("Error in epoll_wait");
            return;
        }
        for(int index = 0; index < _epollS.getNumberEvents(); ++index)
        {
			// std::cout << RED << "Tem um NOVO EVENTO" << END << std::endl;
			isEventFromServerSocket(events,index);
			if(events[index].events & EPOLLIN)
			{
				std::cout << RED << "Onde entra o ultimo - 1" << END << std::endl;
            	if(_epollS.getIsServerFdTriggered() == true)
            	{
					std::cout << BLUE << "Adicionando OUTRO CLIENT" << END << std::endl;
            	    int result = _epollS.addNewClientToEpoll(events, index);
            	    if (result == -3)
            	        continue;
            	} else {
					// std::cout << RED << "Onde entra o ultimo - 2" << END << std::endl;
					//quarta: temos que ldiar com as requests pra GET e provavelmente fazer um objeto pro client
					// - guardando informação de em que server ele está bindando... (pesquisar mais com o chat)
					int clientSocket = events[index].data.fd;
            	    std::cout << BLUE << "LER REQUEST PARA O CLIENT: " << clientSocket << END << std::endl;
					readRequest(clientSocket);
            	}
			}
			else if(events[index].events & EPOLLOUT)
			{
				std::cout << "FD QUE CAIU: " << events[index].data.fd << std::endl;
				std::cout << "FD _EPOLLS: " << _epollS.getClientFd() << std::endl;
				// if (isEventFromServerSocket(events,index))
				// 	std::cout << "O evento é de SOCKET SERVIDOR" << std::endl;
				// else
				// 	std::cout << "O evento é NÃO é de socket servidor" << std::endl;
				// ssize_t bytesSent = send(_epollS.getClientFd(), _response.c_str(), _response.length(), 0);
        		// if(bytesSent == -1)
            	// 	throw ErrorException ("Erro ao enviar a resposta ao cliente");
				// removeClientFromEpoll(_epollS);
				// std::cout << BLUE << "\n---------------------------------------" << END << std::endl;
            	// std::cout << BLUE <<     "----- FECHOU A CONEXÃO COM O CLIENTE ----" << END << std::endl;
            	// std::cout << BLUE << "-----------------------------------------" << END << std::endl;

				if (_epollS.getClientFd() == events[index].data.fd) {
                    // O soquete do cliente está pronto para escrita, pode usar send
                    ssize_t bytesSent = send(_epollS.getClientFd(), _response.c_str(), _response.length(), 0);
                    if (bytesSent == -1) {
    					    // Outro erro ocorreu, trate conforme necessário
    					    perror("Erro ao enviar a resposta ao cliente [send]");
							std::cerr << "Index do evento: " << index << std::endl;
							std::cerr << "Erro no descritor de arquivo do cliente: " << _epollS.getClientFd() << " | Numero de eventos: " << _epollS.getNumberEvents() << std::endl;
    					}
						// removeClientFromEpoll(_epollS);
						epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_DEL, _epollS.getClientFd(), &_epollS._event);
						std::cout << YELLOW << "[REMOVER O FD DO CLIENT] > " << _epollS.getClientFd() << END << std::endl;
						close(_epollS.getClientFd());
						std::cout << YELLOW << "[FD REMOVIDO] > " << _epollS.getClientFd() << END << std::endl;
						std::cout << RED << "\n---------------------------------------" << END << std::endl;
                        std::cout << RED << "----- FECHOU A CONEXÃO COM O CLIENTE ----" << END << std::endl;
                        std::cout << RED << "-----------------------------------------" << END << std::endl;
                    } else {
                        // Envio bem-sucedido
                        // removeClientFromEpoll(_epollS);
						epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_DEL, _epollS.getClientFd(), &_epollS._event);
						std::cout << YELLOW << "[REMOVER O FD DO CLIENT] > " << _epollS.getClientFd() << END << std::endl;
						close(_epollS.getClientFd());
                        std::cout << BLUE << "\n---------------------------------------" << END << std::endl;
                        std::cout << BLUE << "----- FECHOU A CONEXÃO COM O CLIENTE ----" << END << std::endl;
                        std::cout << BLUE << "-----------------------------------------" << END << std::endl;
                    }
                }
			}
        }
    for(size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex)
        close(_serverSocket[serverIndex].getWebServSocket());
}
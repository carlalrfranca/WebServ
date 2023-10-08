/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/24 20:22:28 by lfranca-          #+#    #+#             */
/*   Updated: 2023/10/08 12:11:37 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"
#include "../inc/Epoll.hpp"

Epoll::Epoll() : _epollFd(0), _maxEvents(25), _isServerFdInPoolTriggered(false), _numberEvents(0)
{}

Epoll::~Epoll()
{}

const int& Epoll::getEpollFd() const
{
	return _epollFd;
}

const int& Epoll::getNumberEvents() const
{
	return _numberEvents;	
}

const bool& Epoll::getIsServerFdTriggered() const
{
	return _isServerFdInPoolTriggered;
}

const int& Epoll::getMaxEvents() const
{
	return _maxEvents;
}

const struct epoll_event& Epoll::getEvent()const
{
	return _event;
}

const int& Epoll::getClientFd()const
{
	return _clientFd;
}

void Epoll::setClientFd(int clientFd)
{
	_clientFd = clientFd;
}

void Epoll::setEvent(struct epoll_event event)
{
	_event = event;
}

void Epoll::setEpollFd(int epollFd)
{
	_epollFd = epollFd;
}

void Epoll::setNumberEvents(int numEvents)
{
	_numberEvents = numEvents;
}

void Epoll::setIsServerFdTriggered(bool isServerTriggered)
{
	_isServerFdInPoolTriggered = isServerTriggered;
}

int Epoll::addServersToEpoll(std::vector<SocketS>& servers)
{
	_epollFd = epoll_create1(0);
	if(_epollFd == -1)
	{
		perror("Error creating epoll");
		return -1;
	}
    //Imprimir detalhes de cada servidor
	// struct epoll_event event;
	bool isDuplicated = false;
    for (std::vector<SocketS>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        for (std::vector<SocketS>::iterator it2 = servers.begin(); it2 != it; ++it2)
        {
            if (it2->getWebServSocket() == it->getWebServSocket())
            {
				isDuplicated = true;
            }
        }
		if (!isDuplicated)
		{
			// add server to epoll
        	std::cout << "Porta: " << it->getPort() << std::endl;
        	std::cout << "Endereço: " << it->getAddress() << std::endl;
			std::cout << "WebServ SOCKET FD: " <<  it->getWebServSocket() << std::endl;
        	std::cout << "-----------------------------------------\n" << std::endl;
			fcntl(it->getWebServSocket(), F_SETFL, O_NONBLOCK, FD_CLOEXEC);
			_event.data.u64 = 0;
        	_event.data.fd = it->getWebServSocket();
        	_event.events = EPOLLIN;
        	if(epoll_ctl(_epollFd, EPOLL_CTL_ADD, _event.data.fd, &_event) == -1)
        	{
        	    perror("Error adding socket to epoll");
        	    return -2;
        	}
		}
	}
	return 0;
}

int Epoll::addNewClientToEpoll(struct epoll_event *event_ptr, int i)
{
	// NOTA:  aqui criamos e adicionamos um novo objeto Client (ainda não foi incorporado) ao vetor [ou map?] de clients no Webserv. 
	//struct sockaddr_in clientAddress = {0};
	struct sockaddr_in clientAddress = {0, AF_INET, 0, INADDR_ANY};
	socklen_t clientAddressLength = sizeof(clientAddress);
	int clientSocket = accept(event_ptr[i].data.fd, (struct sockaddr*)&clientAddress, &clientAddressLength);
	if(clientSocket == -1)
	{
	    perror("Error accepting client connection");
	    return -3; // Move to the next event
	}
	fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	_event.data.u64 = 0;
	_event.data.fd = clientSocket;
	_event.events = EPOLLIN;
	
	if(epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientSocket, &_event) == -1)
	{
	    perror("Error adding client socket to epoll");
	    close(clientSocket); // Close the socket on error
	}
	std::cout << RED << "Conexão aceita com client FD: " << clientSocket << END << std::endl;
	return 0;
}

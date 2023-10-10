/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/24 20:22:28 by lfranca-          #+#    #+#             */
/*   Updated: 2023/10/10 11:08:31 by lfranca-         ###   ########.fr       */
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
		std::cout << RED << "Error: Creating epoll" << END << std::endl;
		return -1;
	}
	bool isDuplicated = false;

    for(std::vector<SocketS>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        for (std::vector<SocketS>::iterator it2 = servers.begin(); it2 != it; ++it2)
        {
            if (it2->getWebServSocket() == it->getWebServSocket())
            {
				isDuplicated = true;
            }
        }
		if(!isDuplicated)
		{
			fcntl(it->getWebServSocket(), F_SETFL, O_NONBLOCK, FD_CLOEXEC);
			_event.data.u64 = 0;
        	_event.data.fd = it->getWebServSocket();
        	_event.events = EPOLLIN;

        	if(epoll_ctl(_epollFd, EPOLL_CTL_ADD, _event.data.fd, &_event) == -1)
        	{
        	    std::cout << RED << "Error: Adding server socket to epoll" << END << std::endl;
        	    return -2;
        	}
		}
	}
	return 0;
}

int Epoll::addNewClientToEpoll(struct epoll_event *event_ptr, int i)
{
	struct sockaddr_in clientAddress = {0, AF_INET, 0, INADDR_ANY};
	socklen_t clientAddressLength = sizeof(clientAddress);
	int clientSocket = accept(event_ptr[i].data.fd, (struct sockaddr*)&clientAddress, &clientAddressLength);
	
	if(clientSocket == -1)
	{
	    std::cout << RED << "Error: Accepting client connection" << END << std::endl;
	    return -3;
	}
	fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	_event.data.u64 = 0;
	_event.data.fd = clientSocket;
	_event.events = EPOLLIN;
	
	if(epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientSocket, &_event) == -1)
	{
	    std::cout << RED << "Error: Adding client socket to epoll" << END << std::endl;
	    close(clientSocket);
	}
	std::cout << BLUE << "Accepted connection with client: " << clientSocket << END << std::endl;
	return 0;
}

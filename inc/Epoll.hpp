/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/24 19:59:24 by lfranca-          #+#    #+#             */
/*   Updated: 2023/10/08 09:51:28 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _EPOLL_HPP_
#define _EPOLL_HPP_

#include "HeadersLibs.hpp"
#include "SocketS.hpp"

class Epoll
{
	public:

		Epoll();
		~Epoll();

		int addServersToEpoll(std::vector<SocketS>& servers);
		int addNewClientToEpoll(struct epoll_event *event_ptr, int i);
		
		// getters and setters
		void setEpollFd(int epollFd);
		void setNumberEvents(int numEvents);
		void setIsServerFdTriggered(bool isServerTriggered);
		
		void setClientFd(int clientFd);
		void setEvent(struct epoll_event event);
		
		const struct epoll_event& getEvent()const;
		const int& getClientFd()const;

		const int& getEpollFd() const;
		const int& getMaxEvents() const;
		const int& getNumberEvents() const;
		const bool& getIsServerFdTriggered() const;
		
		struct epoll_event  _event;
	
	private:

		int			_epollFd;
		const int	_maxEvents;
		bool		_isServerFdInPoolTriggered;
		int			_numberEvents;
		
		
		
		
		int					_clientFd;
};

#endif
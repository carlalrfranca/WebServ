/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/24 19:59:24 by lfranca-          #+#    #+#             */
/*   Updated: 2023/08/24 21:32:36 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _EPOLL_HPP_
#define _EPOLL_HPP_

#include "HeadersLibs.hpp"
#include "SocketS.hpp"

class Epoll {

	public:
		Epoll(void);
		~Epoll(void);

		int addServersToEpoll(std::vector<SocketS>& servers);
		int addNewClientToEpoll(struct epoll_event *event_ptr, int i);
		
		// getters and setters
		const int& getEpollFd(void) const;
		void setEpollFd(int epollFd);
		const int& getNumberEvents(void) const;
		const int& getMaxEvents(void) const;
		void setNumberEvents(int numEvents);
		const bool& getIsServerFdTriggered(void) const;
		void setIsServerFdTriggered(bool isServerTriggered);

	private:
		int _epollFd;
		const int _maxEvents;
		bool _isServerFdInPoolTriggered;
		int _numberEvents;
};

#endif
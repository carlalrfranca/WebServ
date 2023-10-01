/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 22:40:19 by lfranca-          #+#    #+#             */
/*   Updated: 2023/09/29 21:41:06 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client.hpp"

Client::Client()
{}

Client::~Client()
{}

void Client::setFd(size_t clientFd)
{
    _clientFd = clientFd;
}

void Client::setServerToRespondeFd(size_t serverfd)
{
    _serverToRespondFd = serverfd;
}

void Client::setResponse(const Response& response)
{
    _response = response;
}

size_t Client::getFd()const
{
    return _clientFd;
}

size_t Client::getServerToRespondFd()const
{
    return _serverToRespondFd;
}

Response Client::getResponse()const
{
    return _response;
}

void Client::clearClient()
{}

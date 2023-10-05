/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 22:32:29 by lfranca-          #+#    #+#             */
/*   Updated: 2023/10/04 15:16:20 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include "./HeadersLibs.hpp"
#include "../inc/CGI.hpp"
#include "./Response.hpp"

// pra usar na hora de adaptar pra multiplos servidores
// tambem precisa ver como adaptar o checkTimeOut()
class Client
{
	public:

		Client();
		~Client();

		void clearClient();
		void setFd(size_t clientFd);
		void setResponse(const Response& response);
		void setServerToRespondeFd(size_t serverfd);
		
		size_t getFd()const;
		size_t getServerToRespondFd()const;
		Response getResponse()const;

	private:

		size_t 		_serverToRespondFd;
		size_t 		_clientFd; //faz sentido armazenar aqui?
		Response	_response;
};

#endif
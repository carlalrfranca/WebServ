/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 22:32:29 by lfranca-          #+#    #+#             */
/*   Updated: 2023/10/10 11:51:51 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include "./HeadersLibs.hpp"
#include "../inc/CGI.hpp"
#include "./Response.hpp"

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
		size_t 		_clientFd;
		Response	_response;
};

#endif
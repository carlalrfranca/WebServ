/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 22:32:29 by lfranca-          #+#    #+#             */
/*   Updated: 2023/09/13 22:43:19 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include "./HeadersLibs.hpp"
#include "./Response.hpp"

// pra usar na hora de adaptar pra multiplos servidores
// tambem precisa ver como adaptar o checkTimeOut()
class Client {

	public:
		Client();
		~Client();

		size_t getFd(void) const;
		void setFd(size_t clientFd);
		size_t getServerToRespondFd(void) const;
		void setServerToRespondeFd(size_t serverfd);
		Response getResponse(void) const;
		void setResponse(const Response& response);

		void clearClient();

	private:
		size_t serverToRespondFd;
		size_t clientFd; //faz sentido armazenar aqui?
		Response response;
};

#endif
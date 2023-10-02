/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusMessages.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/24 18:33:38 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/02 16:32:42 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/StatusMessages.hpp"

StatusMessages::StatusMessages()
{
    statusMessages[200] = "OK";
	statusMessages[301] = "Moved Permanently";
	statusMessages[302] = "Temporary Redirect";
	statusMessages[307] = "Internal Redirect";
	statusMessages[308] = "Internal Redirect";
    statusMessages[400] = "Bad Request";
    statusMessages[401] = "Unauthorized";
    statusMessages[403] = "Forbidden";
    statusMessages[404] = "Not Found";
	statusMessages[405] = "Method Not Allowed";
    statusMessages[413] = "Payload Too Large";
    statusMessages[500] = "Internal Server Error";
	statusMessages[501] = "Not Implemented";
    statusMessages[503] = "Service Unavailable";
	statusMessages[504] = "Gateway Timeout";
    statusMessages[505] = "HTTP Version Not Supported";
    // Adicione mais códigos e mensagens conforme necessário
}

std::string StatusMessages::getMessage(int statusCode)
{
    std::map<int, std::string>::iterator it = statusMessages.find(statusCode);
    if (it != statusMessages.end())
        return it->second;
    return "Unknown Status";
}


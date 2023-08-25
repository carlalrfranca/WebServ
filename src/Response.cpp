/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/18 18:00:34 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/24 18:27:37 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"

#ifndef _RESPONSE_HPP_
#define _RESPONSE_HPP_

#include <string>
#include <iostream>

//acho que ele puxa por chave e valor entao perguntar depois se vamos usar map mesmo


class Server;
class Request;


class Response {

    public:
        Response(void);
        ~Response(void);
        Response(Request *req, Server *_server); //precisa desses dois
        
        
        //getters and setters
        size_t getIdClient()const;
        const std::string& getHeader(const std::string& header) const;
        
        //response
        void status();
        void text();
        void reset();
        // ..to be continue
    
    
    private:
    
};

std::ostream& operator<<(std::ostream& out, Response const& rhs);

#endif //_                                                                          */

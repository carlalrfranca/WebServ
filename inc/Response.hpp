/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/18 18:00:34 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/24 18:48:04 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _RESPONSE_HPP_
#define _RESPONSE_HPP_
#include "HeadersLibs.hpp"

//acho que ele puxa por chave e valor entao perguntar depois se vamos usar map mesmo

class Server;
class Request;

class Response
{

    public:
    
        Response(void);
        ~Response(void);
        Response(Request *req, Server *_server); //precisa desses dois
        
        //getters and setters
        size_t getIdClient()const;
        const std::string& getHeader(const std::string& header) const;
        
        //response
        void createResponse(std::string requestedContent);
        void readRequestedFile(std::string requestedContent);

        //void status();
        //void text();
        //void reset();
        // ..to be continue
        class ResponseException: public std::exception{
        public:
            virtual const char* what() const throw(){
                return "\nError: Inserir uma mensagem no Response Exception\n";
            }
        };
    
    
    private:
        
        const char *response;
        ssize_t _bytesSent;
    
};

std::ostream& operator<<(std::ostream& out, Response const& rhs);

#endif                                                                       */

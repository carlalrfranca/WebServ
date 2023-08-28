/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/18 18:00:34 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/27 19:37:42 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _RESPONSE_HPP_
#define _RESPONSE_HPP_
#include "HeadersLibs.hpp"

//acho que ele puxa por chave e valor entao perguntar depois se vamos usar map mesmo

class SocketS;
class Request;

class Response
{

    public:
    
        Response(void);
        ~Response(void);
        Response(Request *req, SocketS *_server); //precisa desses dois
        
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

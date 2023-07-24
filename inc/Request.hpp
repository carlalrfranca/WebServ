/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 16:43:49 by cleticia          #+#    #+#             */
/*   Updated: 2023/07/23 14:06:16 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _RESQUEST_HPP_
#define _RESQUEST_HPP_

#include <iostream>
#include "Server.hpp"
#include "Server.hpp"
#include <vector>

//inserir macro buffer
class Request {

    public:
        Request();
       ~Request(); 
        
    private:
        char _buffer[1024];
        std::string _request; //talvez fique na request class
        size_t _foundRequest; //talvez fique na request class
        ssize_t _bytesRead;

        void printRequestHeaders();    
        class RequestException: public std::exception{
            public:
                virtual const char* what() const throw(){
                return "\nError: Inserir uma mensagem no Request Exception\n";
            }
                
        };        
};
//esse aqui está mais para depuração e apresentação das informações
std::ostream& operator<<(std::ostream& output, const Request& rhs);
#endif

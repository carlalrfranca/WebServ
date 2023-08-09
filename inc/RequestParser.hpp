/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/08 16:45:29 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/08 17:28:25 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _REQUESTPARSER_HPP_
#define _REQUESTPARSER_HPP_

#include <iostream>
#include <unistd.h>


class RequestParser {

    public:
    
        RequestParser();
        ~RequestParser();
        RequestParser(std::string& request);
        
        void extractPath(); //extrai o caminho do recurso da 1 pimeira linha
        void extractMethod(); //extrai o metodo HTTP da 1 linha da solicitação
        void verifyHeaders(); //analisa os cabeçalhos  da solicitação
        void verifyBody(); //lida com o parseamento do corpo da solicitação
    
        bool isParsed();
    private:
        std::string _request;
        std::string _method;
        std::string _path;
        std::string _httpVersion;
        
        
    
    class RequestParserException: public std::exception{
    public:
        virtual const char* what() const throw(){
            return "\nError: No Parseamento da Request Parsa, dá seu jeito!\n";
        }
    };

};

#endif
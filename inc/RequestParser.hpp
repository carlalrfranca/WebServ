/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/08 16:45:29 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/09 21:18:14 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _REQUESTPARSER_HPP_
#define _REQUESTPARSER_HPP_

#include <iostream>
#include <unistd.h>
#include <vector>

class RequestParser {

    public:
    
        RequestParser();
        ~RequestParser();
        RequestParser(std::string& request);
        

    private:

        std::vector<std::string>    _tokensFirstLine;
        std::string                 _method;
        std::string                 _firstLine;
        std::string                 _token;
        


    class RequestParserException: public std::exception{
    public:
        virtual const char* what() const throw(){
            return "\nError: No Parseamento da Request Parsa, dá seu jeito!\n";
        }
    };
};

#endif


/*




*/

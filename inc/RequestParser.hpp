/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/08 16:45:29 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/11 15:18:06 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _REQUESTPARSER_HPP_
#define _REQUESTPARSER_HPP_
#include "HeadersLibs.hpp"

class RequestParser
{

    public:
    
        RequestParser();
        ~RequestParser();
        RequestParser(std::string& request);

    private:

        std::vector<std::string>    _tokensFirstLine;
        std::string                 _method;
        std::string                 _firstLine;
        std::string                 _token;

    class RequestParserException: public std::exception
    {
        public:
            virtual const char* what() const throw()
            {
                return "\nError: In Request Parsing!\n";
            }
    };
};
#endif
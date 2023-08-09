/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/08 16:45:18 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/09 16:13:40 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/RequestParser.hpp"

RequestParser::RequestParser()
{}

RequestParser::~RequestParser()
{}

RequestParser::RequestParser(std::string& request) : _request(request)
{}
        
void RequestParser::extractPath()//extrai o caminho do recurso da 1 pimeira linha
{}

void RequestParser::extractMethod()//extrai o metodo HTTP da 1 linha da solicitação
{}

void RequestParser::verifyHeaders()//analisa os cabeçalhos  da solicitação
{}

void RequestParser::verifyBody()//lida com o parseamento do corpo da solicitação
{}

bool RequestParser::isParsed()
{
    
    return true;
}

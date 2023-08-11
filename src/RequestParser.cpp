/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/08 16:45:18 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/09 18:56:16 by cleticia         ###   ########.fr       */
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


/*

tenho que pegar a string da request
extrair o conteudo ate a primeira quebra de linha
armazeno esse conteudo em um atributo
depois pego o atributo
splitar a linha com base em espáço
armazeno em um verto de string
depois valido o que for get separo
deixo 


metodo
caminho
valido o protocolo e salvo em atributo o protocolo



a segunda linha eu nao salvo o host
só identifica e salva o valor que vier neste caso o localhost:8080


*/

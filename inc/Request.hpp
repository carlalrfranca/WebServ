/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 16:43:49 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/24 18:47:26 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _RESQUEST_HPP_
#define _RESQUEST_HPP_
#include "HeadersLibs.hpp"

//inserir macro buffer
class Request
{

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


// solicitacao da request as linhas


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

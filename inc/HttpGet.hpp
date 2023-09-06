/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpGet.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:43:28 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/05 20:50:14 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _HTTP_GET_HPP_
#define _HTTP_GET_HPP_

#include "HeadersLibs.hpp"


/*
    Criação de classe para validacao de GET. No metodo da response
    faz um bloco de try catch. Dentro do try insere a resposta e fora a excecao de erro.

    criar classe de VALIDAÇÃO DE GET e incorporar na função que lida com 
    o GET (tem que ter exceção preparada caso a validação tenha algo errado)
*/

class ValidGet
{

    public:
    
        ValidGet();
        ValidGet(const std::string& root);
        ~ValidGet();
        void setUri(const std::string& uri);
        std::string handlerGetRequest();
        
        void setRoot(const std::string& root);
        
    private:
        
        std::string _root; //raiz do servidor
        std::string _uri;
        std::string _fullPath;
        
        bool fileNotFound(const std::string& fullPath); //auxiliar para o caso do arquivo não ter sido encontrado
        class HttpGetException: public std::exception{
    
            public:
                virtual const char* what() const throw(){
                    return "\nHttpGetException: Error processing GET request.\n";
                }
        };
};
#endif
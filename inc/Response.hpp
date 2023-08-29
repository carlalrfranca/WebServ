/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/18 18:00:34 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/28 22:49:52 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _RESPONSE_HPP_
#define _RESPONSE_HPP_
#include "HeadersLibs.hpp"

class Server;
class Request;

class Response
{

    public:
    
        Response(void);
        ~Response(void);
        Response(Request *req, Server *_server); //precisa desses dois
        
        
        //getters and setters
        size_t getIdClient(void)const;
        const std::string& getHeader(const std::string& header)const; // busca pelo campo de cabeçalho na lista de cabeçalhos
        
        //response methods
        void status(int statusCode, const std::string& header)const; // status da resposta com base no código e no texto fornecidos
        void text(void); //configuração do corpo da resposta como texto
        void reset(void); //implementa a redefinição de resposta, limpando cabeçalhos e corpo
        
        //response methods
        void processResponse(void); //processa a solicitacao, verifica método, URI, etc. e constrói a resposta de acordo com os requisitos
        void createResponse(std::string requestedContent);
        void readRequestedFile(std::string requestedContent);
    
    private:
    
        std::map<std::string, std::string>  _headers;
        std::string                         _body;
        size_t                              _idClient;
        // LETICIA DEIXOU ANOTADO:::::
        // criar uma string response que, será todo esse cabeçalho + body (ver exemplos no chat)
        // PRA DEIFINIR O BODY (um arquivo html que voce precisa "pegar" do diretorio que temos, por exemplo):
            // 1. Identificar QUAL é o método sendo pedido pela request e verificar que esse socket ACEITA ESSA MÉTODO (get, por exemplo)
            //    ---> se NÃO aceitar, construir uma RESPOSTA DE ERRO (com o statusCode adequado) e ARMAZENAR isso na response da classe e retornar (pra interromper o metodo)
            //  2. SE ACEITAR, daí IDENTIFICAR qual é o RECURSO (URI) pedido pela request e verificar se
            //     esse recurso está entre os locations (map) desse SERVER SOCKET (qualquer duvida, dar uma olhada no hpp do SocketS)
            //    ---> Se esse recurso NÃO ESTIVER DENTRE AS LOCATIONS, tem que construir uma RESPOSTA DE ERRO (404?) e armazenar na resposne da classe e retornar (pra interromper o metodo)
            //  3. SE ESTIVER PRESENTE, acessar o valor (LocationsDirective) desse location no map e indeitificar o arquivo INDEX pedido (está em algum lugar do diretorio) e
            //    criar um método pra abrir esse arquivo e armazenar seu conteúdo numa string (a string body, no caso)            
        // precisa devolver essa string de alguma forma (get?) pra mainLoop
        // pra que ela possa fazer send() dessa response pro clientSocket
        
        class ResponseException: public std::exception
        {
            public:
                virtual const char* what() const throw()
                {
                    return "\nError: An error occurred while processing the response.\n";
                }
        };
    
};
std::ostream& operator<<(std::ostream& out, Response const& rhs);

#endif 


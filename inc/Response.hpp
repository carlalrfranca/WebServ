/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/18 18:00:34 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/15 21:46:28 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _RESPONSE_HPP_
#define _RESPONSE_HPP_
#include "HeadersLibs.hpp"
#include "SocketS.hpp"
#include "CGI.hpp"
#include "StatusMessages.hpp"

class SocketS;
class Request;
class ValidGet;
class Response;
typedef std::string (*Funcao)(Request &request, SocketS &server, Response *this_response);

class Response{

    public:
    
        Response();
        ~Response();
        Response(Request request); //precisa desses dois
        //Response &operator=(Response const &other);
        
        //getters and setters
        const std::string& getHeader(const std::string& header)const; // busca pelo campo de cabeçalho na lista de cabeçalhos
        const std::string getPath()const;
        std::string getResponse()const;//foi criado outro metodo, precisa ajustar .cpp
		std::string getDate()const;
		
        void setPath(const std::string& allPath); 
        void setStatusCode(const std::string& statusCode);
        void setContentType(const std::string& contentType);
        void setDateAndTime();
        // void setContentLength(size_t length);
        void setResponse(const std::string& response);
        int  selectServer(Request& stringDomain, std::vector<SocketS> serverSocket);
        
        //std::string httpPost(Request &request, SocketS &server);
        //std::string httpDelete(Request &request, SocketS &server);
        
        //response methods
        std::string readHtmlFile(const std::string& filePath);
        std::string buildResponse(Request &request, SocketS &server);
        bool contains(const std::vector<std::string>& vec, const std::string& content);

        void reset(); //implementa a redefinição de resposta, limpando cabeçalhos e corpo
        void httpError(std::string errorCode, const std::string &errorMessage);
		
		//tá eu inicalizei dentro so .hpp, sei lá se isso pod o nao, mas se jogasse no construor or algum motivo
		//acho ue dariamuito errado por ser uma const...se bem que eu pderia apenas tirar isso
		
        void errorPage(Request &response, std::string &root);
        
        
		std::string errorCodeHtml(int statusCode, SocketS &server);

		// method for each method
		static std::string deleteMethod(Request &request, SocketS &server, Response *this_response);
		// static std::string postMethod(Request &request, SocketS &server);
		static std::string postMethod(Request &request, SocketS &server, Response *this_response);
		// static std::string getMethod(Request &request, SocketS &server, Response *this_response);
        static std::string httpGet(Request &request, SocketS &server, Response *this_response);
        
    private:
    
        std::map<std::string, std::string>  _headers;
        std::string                         _body;
        std::string                         _response; //criar uma string response que, será todo esse cabeçalho + body (ver exemplos no chat)
        std::string                         _code;
        std::string                         _path;
        SocketS                             *_chosenSocket;
        StatusMessages                      _statusMessages;
		std::map<std::string, Funcao>		_methodsFunctions;

        class ErrorException: public std::exception
        {
            public:
        
                ErrorException(const char* errorMessage) : _msg(errorMessage){}
                virtual const char* what()const throw(){
                
                    return _msg;
                }
            
            private:
            
                const char* _msg;
        };
};

#endif 

// std::ostream& operator<<(std::ostream& out, Response const& rhs);
        /*
            ------------------------------------------- LETICIA DEIXOU ANOTADO ----------------------------------------------
            criar uma string response que, será todo esse cabeçalho + body (ver exemplos no chat)
            PRA DEIFINIR O BODY (um arquivo html que voce precisa "pegar" do diretorio que temos, por exemplo):

            1. Identificar QUAL é o método sendo pedido pela request e verificar que esse socket ACEITA ESSA MÉTODO (get, por exemplo)
                ---> se NÃO aceitar, construir uma RESPOSTA DE ERRO (com o statusCode adequado) e ARMAZENAR isso na response da classe e retornar (pra interromper o metodo)
            2. SE ACEITAR, daí IDENTIFICAR qual é o RECURSO (URI) pedido pela request e verificar se
                 esse recurso está entre os locations (map) desse SERVER SOCKET (qualquer duvida, dar uma olhada no hpp do SocketS)
                ---> Se esse recurso NÃO ESTIVER DENTRE AS LOCATIONS, tem que construir uma RESPOSTA DE ERRO (404?) e armazenar na resposne da classe e retornar (pra interromper o metodo)
            3. SE ESTIVER PRESENTE, acessar o valor (LocationsDirective) desse location no map e indeitificar o arquivo INDEX pedido (está em algum lugar do diretorio) e
                criar um método pra abrir esse arquivo e armazenar seu conteúdo numa string (a string body, no caso)
         precisa devolver essa string de alguma forma (get?) pra mainLoop
         pra que ela possa fazer send() dessa response pro clientSocket
        */

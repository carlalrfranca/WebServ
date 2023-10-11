/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/18 18:00:34 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/10 23:57:22 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _RESPONSE_HPP_
#define _RESPONSE_HPP_

#include "StatusMessages.hpp"
#include "LocationDirective.hpp"
#include "UtilsResponse.hpp"
#include "HeadersLibs.hpp"
#include "SocketS.hpp"
#include "Request.hpp"
#include "HttpGet.hpp"
#include "Utils.hpp"
#include "CGI.hpp"

#define PATH_DELETE_RESET "web/cgi-bin/delete_upload_image.py"
#define	PATH_POST_INFO "process_data_store_form.cgi"
#define PATH_INFO "web/cgi-bin/process_data_store_form.py"
#define PATH_GET_INFO "delete_upload_image.cgi"
#define	DEFAULT_UPLOAD_STORE "web/images/" //usar depois pro valor default do 'uploadStore' (só procurar onde tem essa verificação no código na Response.cpp)
#define	FILE_FORM_STORE "form_data.txt"

class SocketS;
class Request;
class ValidGet;
class Response;

typedef std::string (*Funcao)(Request &request, SocketS &server, Response *this_response);

class Response
{
    public:
    
        Response();
        ~Response();
    
        //getters and setters
		std::string getUri()const;
		std::string getDate()const;
		std::string getMethod()const;
        std::string getResponse()const;//foi criado outro metodo, precisa ajustar .cpp
        std::string getParameter(std::string query, std::string parameter)const;
        const std::string getPath()const;
        const std::map<std::string, std::string>& getHeader()const;
		const std::vector<std::string>& getAllowedMethods()const;

		void setUri(const std::string& uri);
        void setPath(const std::string& allPath); 
        void setResponse(const std::string& response);
        void setStatusCode(const std::string& statusCode);
        void setContentType(const std::string& contentType);
        void setDateAndTime();
		void setAllowedMethods(const std::vector<std::string> allowedMethods);
        
        //response methods
        void reset();
        void buildResponse(Request &request, SocketS &server);
        void generateResponse(int statusCode, const Request& request);
        void httpError(std::string errorCode, const std::string &errorMessage);
        void errorPage(Request &response, std::string &root);
		void errorCodeHtml(int statusCode, SocketS &server);
        void generateHtmlFromFiles(const std::vector<std::string>& fileList, std::map<std::string, LocationDirective>::iterator& it);
        int  selectServer(Request& stringDomain, std::vector<SocketS> serverSocket);
        bool contains(const std::vector<std::string>& vec, const std::string& content);
        bool listFilesAndGenerateHtml(std::map<std::string, LocationDirective>::iterator& it);
		bool buildPathToResource(std::string root, Request &request, SocketS &server, std::map<std::string, std::vector< std::string > >& locationDirectives, std::map<std::string, LocationDirective>::iterator& it);
		bool isResponseADirectoryListingOrErrorPage(std::string path, SocketS &server, std::map<std::string, std::vector< std::string > >& locationDirectives, std::map<std::string, LocationDirective>::iterator& it, std::string indexPage);
		bool isDirectory(const std::string& path);
		bool isThisMethodAllowed(std::map<std::string, LocationDirective>& serverLocations, Request &request, SocketS &server, std::string& requestMethod);
        std::string extractScriptName(std::string& uri);
        std::string generateHeaders(int statusCode, const Request& request);
		std::string extractUriAndBuildPathToResource(std::string root, std::vector<std::string>& parts_uri, std::string& uri, std::map<std::string, LocationDirective>::iterator& it);
        std::string readHtmlFile(const std::string& filePath);
		std::map<std::string, LocationDirective>::iterator findRequestedLocation(Request &request, std::map<std::string, LocationDirective>& serverLocations);
		// method for each method
		std::string postMethodTryCGI(Request &request, SocketS &server, std::string uploadStoreFolder, CGI script);
        static std::string httpGet(Request &request, SocketS &server, Response *this_response);
		static std::string deleteMethod(Request &request, SocketS &server, Response *this_response);
		static std::string postMethod(Request &request, SocketS &server, Response *this_response);
		static std::string buildHeaderReturn(std::string statusCode, std::string resource, Response *this_response);
		static std::map<std::string, std::string> initContentTypes();
        std::string getContentTypeFromExtension(const std::string& extension);

        std::string generateHeaders(int statusCode, const Request& request, std::string content);

    private:
    
        std::map<std::string, std::string>  _headers;
        std::string                         _body;
        std::string                         _response;
        std::string                         _code;
        std::string                         _path;
		std::string                         _uri;
        Utils                               _utils;
        SocketS                             *_chosenSocket;
        UtilsResponse                       _utilsResponse;                
        StatusMessages                      _statusMessages;
		std::map<std::string, Funcao>		_methodsFunctions;
		std::map<std::string, std::string>  _extensionToContentType;
		std::vector<std::string>			_allowedMethods;

    class ErrorException: public std::exception
    {
        public:
            ErrorException(const char* errorMessage) : _msg(errorMessage){}
            virtual const char* what()const throw()
            {
                return _msg;
            }
        
        private:
            const char* _msg;
    };
};

#endif 

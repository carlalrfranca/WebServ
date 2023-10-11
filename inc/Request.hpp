/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 16:43:49 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/11 15:16:12 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _REQUEST_HPP_
#define _REQUEST_HPP_

#include "HeadersLibs.hpp"
#include "Utils.hpp"
#include "UtilsResponse.hpp"
#include "StatusMessages.hpp"
#define SUCCESS 0

class Request
{
    
    public:

        Request(const std::string& request);
        ~Request();
        
        int validateRequest();
        int isFirstLineValid();
        void printRequest();
        void processBodyRequest();
        void processHeaderRequest();
        std::string errorCodeHtml(int statusCode);
        
        void setHasError(bool hasError);
        void setRequest(const std::string& request);
		void setMethod(const std::string& method);
        void setHeader(const std::string& header); 
        void setBody(const std::string& body);
        void setContentLength(const std::string& contentLength);
        void setContentType(const std::string& contentType);
        void setBoundary(const std::string& boundary);
        void setFilename(const std::string& filename);
		void setAllowedMethods(const std::vector<std::string> allowedMethods);
        
        bool getHasError()const;
		bool getIsDeleteMaskedAsPost();
        std::string getBoundary()const;
        std::string getFilename()const;
        std::string getContentLength()const;        
        std::string getContentType()const;
        std::string getDateAndTime()const;
        std::string getHeader()const;
        std::string getBody()const;
        std::string getDomainRequest()const;
		std::string getRequest() const;
        std::string getPortRequest()const;
		std::string getFileFormat()const;
        const std::string& getMethod()const;
        const std::string& getURI()const;
        const std::string& getVersion()const;
		const std::string getErrorPage(int statusCode)const;
		const std::vector<std::string>& getAllowedMethods()const;
    
    private:
    
        std::string							_body;
        std::string							_boundary;
		std::string							_contentLength;
		std::string							_contentType;
        std::string							_domainContent;
        std::string							_filename;
		std::string							_fileFormat;
        std::string							_firstLine; 
        std::string							_header;
        std::string							_hostLine;
        std::string							_hostContent;
        std::string							_method;
        std::string							_portRequest;
        std::string							_request;
        std::string							_uri;
        std::string							_version;
        std::istringstream					_requestStream;
        bool								_hasError;
		bool								_isDeleteMaskedAsPost;
        Utils								_utils;
		std::map<std::string, std::string>	_errorPage;
		StatusMessages						_statusMessages;
		std::vector<std::string>			_allowedMethods;
        
    
    class ErrorException: public std::exception
    {
        public:
    
            virtual const char* what() const throw()
            {
                return "\nError: Invalid request detected.\n";
            }
        
    };
};

std::ostream& operator<<(std::ostream& output, const Request& rhs);
#endif
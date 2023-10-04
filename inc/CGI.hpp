/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 19:53:37 by lfranca-          #+#    #+#             */
/*   Updated: 2023/10/03 22:42:34 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CGI_HPP_
#define _CGI_HPP_

#include "./HeadersLibs.hpp"
#include "./Request.hpp"

#define PATH_FORM_DATA "./form_data.txt"

class CGI
{
	public:

		CGI();
		CGI(const std::string& root, std::vector<std::string> commands, std::vector<std::string> extensions, std::string scriptName);
		~CGI();

		int executeScript(int *pipefd, std::string fileName);
		int handleCGIRequest(Request &request);
		int uploadImage(Request &request, std::string request_content, size_t data_init_pos);
		int uploadImageCGI(Request &request);
		std::string generateHeadersSucessCGI(int statusCode);
		std::string setDateAndTime();
		// void handleCGIRequest(std::string& request);
		int storeFormInput( std::size_t data_init_pos, const std::string& request_content);
		int CGIForGetRequest(std::string requestedFilePath);
		int executeScriptForGET(int *pipefd, std::string requestedFilePath);

		void setRoot(const std::string& root);
		void setCommands(std::vector<std::string> commands);
		void setExtensions(std::vector<std::string> extensions);
		void setPathToScript(std::string scriptName); //tem que setar o path pro script e completar com as extensões
		void setUploadStoreFolder(std::string uploadStore);

		const std::string& getRoot() const;
		const std::string& getUploadStore() const;
		std::string getResponse() const;
		std::vector<std::string> getCommands() const;
		std::vector<std::string> getExtensions() const;
		const std::string& getPathToScript(void) const;

		 class ErrorException : public std::exception
    	{
    	    public:
	
    	        ErrorException(const std::string& message) throw() : _errorMessage(message) {}
    			virtual ~ErrorException() throw() {}
    			virtual const char* what() const throw()
    	        {
    	            return  _errorMessage.c_str();
    	        }
	
    	    private:
	
    	        std::string _errorMessage;
    	};

	private:

		std::vector<std::string> _scriptCommands;
		std::vector<std::string> _scriptExtensions;
		std::string				_response;
		std::string				_method;
		std::string				_inputFormData;
		std::string				_scriptOutput;
		std::string				_rootToScripts;
		std::string				_scriptName;
		std::string				_uploadStore;
		
};
#endif
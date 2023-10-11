/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 19:53:37 by lfranca-          #+#    #+#             */
/*   Updated: 2023/10/11 16:20:53 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CGI_HPP_
#define _CGI_HPP_

#include "./HeadersLibs.hpp"
#include "./Request.hpp"
#define PATH_FORM_DATA "form_data.txt"
#define	UPLOAD_STORE_LOC "/images/"

class CGI
{
	public:

		CGI();
		~CGI();
		CGI(const std::string& root, std::vector<std::string> commands, std::vector<std::string> extensions, std::string scriptName);

		int executeScript(int *pipefd, std::string fileName);
		int executeProcessParent(int *pipefd, unsigned int timeoutSeconds, pid_t childPi, int *stdoutPipe);
		int handleCGIRequest(Request &request);
		int uploadImage(Request &request, std::string request_content, size_t data_init_pos);
		int uploadImageCGI(Request &request);
		int storeFormInput(std::size_t data_init_pos, const std::string& request_content);
		int CGIForGetRequest(std::string requestedFilePath);
		int executeScriptForGET(int *pipefd, std::string requestedFilePath);
		void executeProcessChild(int *pipefd, std::string fileName, int *stdoutPipe);
		std::string generateHeadersSucessCGI(int statusCode, Request &request);

		std::string setDateAndTime();
		void setRoot(const std::string& root);
		void setCommands(std::vector<std::string> commands);
		void setExtensions(std::vector<std::string> extensions);
		void setPathToScript(std::string scriptName);
		void setScriptNameDirectly(std::string scriptName);
		void setUploadStoreFolder(std::string uploadStore);
		void setLocation(std::string location);
		std::string getLocation()const;

		std::vector<std::string> getCommands()const;
		std::vector<std::string> getExtensions()const;
		std::string getResponse()const;
		const std::string& getRoot()const;
		const std::string& getUploadStore()const;
		const std::string& getPathToScript()const;

		 class ErrorException : public std::exception
    	{
    	    public:
	
    	        ErrorException(const std::string& message) throw() : _errorMessage(message){}
    			virtual ~ErrorException() throw() {}
    			virtual const char* what() const throw()
    	        {
    	            return  _errorMessage.c_str();
    	        }
	
    	    private:
	
    	        std::string _errorMessage;
    	};

	private:

		std::vector<std::string>	_scriptCommands;
		std::vector<std::string>	_scriptExtensions;
		std::string					_response;
		std::string					_method;
		std::string					_inputFormData;
		std::string					_scriptOutput;
		std::string					_rootToScripts;
		std::string					_scriptName;
		std::string					_uploadStore;
		std::string					_scriptPathWithCommand;
		std::string					_locationUri;
};
#endif
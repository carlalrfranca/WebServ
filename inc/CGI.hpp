/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 19:53:37 by lfranca-          #+#    #+#             */
/*   Updated: 2023/09/23 19:38:43 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CGI_HPP_
#define _CGI_HPP_

#include "./HeadersLibs.hpp"
#include "./Request.hpp"

class CGI
{
	public:

		CGI();
		~CGI();

		int executeScript(int *pipefd);
		int handleCGIRequest(Request &request);
		int uploadImage(std::string request_content, size_t data_init_pos);
		// void handleCGIRequest(std::string& request);
		int storeFormInput( std::size_t data_init_pos, const std::string& request_content);
	
		void setRoot(const std::string& root);
		void setCommands(std::vector<std::string> commands);
		void setExtensions(std::vector<std::string> extensions);
		void setPathToScript(std::string scriptName); //tem que setar o path pro script e completar com as extensões
		
		const std::string& getRoot() const;
		std::string getResponse() const;
		std::vector<std::string> getCommands() const;
		std::vector<std::string> getExtensions() const;
		const std::string& getPathToScript(void) const;


	private:

		std::string _response;
		std::string _method;
		std::string _inputFormData;
		std::string _scriptOutput;

		// variaveis pra pegar na Response (metodo post)
		std::string _rootToScripts;
		std::string _scriptName;
		std::vector<std::string> _scriptCommands;
		std::vector<std::string> _scriptExtensions;
		
		
};
#endif
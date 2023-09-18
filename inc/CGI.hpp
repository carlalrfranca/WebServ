/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 19:53:37 by lfranca-          #+#    #+#             */
/*   Updated: 2023/09/17 21:49:38 by lfranca-         ###   ########.fr       */
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

		void executeScript(int *pipefd);
		void handleCGIRequest(Request &request);
		// void handleCGIRequest(std::string& request);
	
		void setRoot(const std::string& root);
		void setCommands(std::vector<std::string> commands);
		void setExtensions(std::vector<std::string> extensions);
		
		const std::string& getRoot() const;
		std::string getResponse() const;
		std::vector<std::string> getCommands() const;
		std::vector<std::string> getExtensions() const;

	private:

		std::string _response;
		std::string _method;
		std::string _inputFormData;
		std::string _scriptOutput;

		// variaveis pra pegar na Response (metodo post)
		std::string _rootToScripts;
		std::vector<std::string> _scriptCommands;
		std::vector<std::string> _scriptExtensions;
		
};
#endif
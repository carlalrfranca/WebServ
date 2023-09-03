/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 19:53:37 by lfranca-          #+#    #+#             */
/*   Updated: 2023/09/02 23:28:49 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CGI_H
#define _CGI_H

#include "./HeadersLibs.hpp"

class CGI {

	public:
		CGI();
		~CGI();
		void executeScript(int *pipefd);
		void handleCGIRequest(std::string& request);
		std::string getResponse(void) const;
	
		// setters and getters for scripts
		void setRoot(const std::string& root);
		void setCommands(std::vector<std::string> commands);
		void setExtensions(std::vector<std::string> extensions);
		std::vector<std::string> getCommands(void) const;
		std::vector<std::string> getExtensions(void) const;
		const std::string& getRoot(void) const;

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
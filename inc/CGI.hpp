/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 19:53:37 by lfranca-          #+#    #+#             */
/*   Updated: 2023/08/29 20:33:41 by lfranca-         ###   ########.fr       */
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
	
	private:
		std::string _response;
		std::string _method;
		std::string _inputFormData;
		std::string _scriptOutput;

};

#endif
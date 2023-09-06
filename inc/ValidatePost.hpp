/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ValidatePost.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/03 21:07:55 by lfranca-          #+#    #+#             */
/*   Updated: 2023/09/03 21:18:09 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _VALIDATE_POST_H_
#define _VALIDATE_POST_H_

#include "HeadersLibs.hpp"

class ValidatePost {

	public:
		ValidatePost(void);
		~ValidatePost(void);
		bool runValidations(std::map<std::string, std::vector< std::string > > &locationDirectives);
		
		class ValidatePostException: public std::exception{
        public:
            virtual const char* what() const throw(){
                return "\nError: Configuration for POST - CGI - isn't defined correctly\n";
            }
        };

	private:
		bool hasCommandsToScript(std::map<std::string, std::vector< std::string > > &locationDirectives);
		bool hasExtensionsToScript(std::map<std::string, std::vector< std::string > > &locationDirectives);
		bool hasAcceptableContentSize(std::map<std::string, std::vector< std::string > > &locationDirectives);
};

#endif
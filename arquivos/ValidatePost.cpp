/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ValidatePost.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/03 21:08:04 by lfranca-          #+#    #+#             */
/*   Updated: 2023/09/29 21:55:41 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ValidatePost.hpp"

ValidatePost::ValidatePost()
{}

ValidatePost::~ValidatePost()
{}

bool ValidatePost::runValidations(std::map<std::string, std::vector< std::string > > &locationDirectives)
{
	if (hasAcceptableContentSize(locationDirectives) == false
		|| hasCommandsToScript(locationDirectives) == false
		|| hasExtensionsToScript(locationDirectives) == false)
			throw ValidatePostException();
	else
		return true;
}

bool ValidatePost::hasCommandsToScript(std::map<std::string, std::vector< std::string > > &locationDirectives)
{
	std::map<std::string, std::vector< std::string > >::iterator commandOfCGI = locationDirectives.find("cgi_path");
	if (commandOfCGI == locationDirectives.end())
		return false;
	return true;
}

bool ValidatePost::hasExtensionsToScript(std::map<std::string, std::vector< std::string > > &locationDirectives)
{
	std::map<std::string, std::vector< std::string > >::iterator CGIExtension = locationDirectives.find("cgi_ext");
	if (CGIExtension == locationDirectives.end())
		return false;
	return true;
}

bool ValidatePost::hasAcceptableContentSize(std::map<std::string, std::vector< std::string > > &locationDirectives)
{
	// tem que passar o content-size aceito pelo arquivo de configuração tambem
}


/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationDirective.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 21:13:30 by lfranca-          #+#    #+#             */
/*   Updated: 2023/08/30 20:19:26 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"
#include "../inc/LocationDirective.hpp"

LocationDirective::LocationDirective(void)
{ }

LocationDirective::~LocationDirective(void)
{ }

void LocationDirective::addDirective(std::string& directiveName, std::string& value)
{
	std::cout << "Diretivas: " << directiveName << std::endl;
	std::cout << "Valor: " << value << std::endl;
	_directives[directiveName].push_back(value);
}

std::map<std::string, std::vector<std::string > > LocationDirective::getDirectives(void) const
{
	return _directives;
}


std::vector<std::string> LocationDirective::getSpecificDirective(std::string directiveName) const
{
	std::map<std::string, std::vector<std::string> >::const_iterator it = _directives.find(directiveName);

    if (it != _directives.end()) {
        std::cout << "Key found! Value: " << it->second[0] << std::endl;
        return it->second;
    } else {
        std::cout << "Key not found!" << std::endl;
    }
}
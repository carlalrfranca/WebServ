/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationDirective.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 21:13:30 by lfranca-          #+#    #+#             */
/*   Updated: 2023/10/01 16:47:47:12 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"
#include "../inc/LocationDirective.hpp"

LocationDirective::LocationDirective()
{}

LocationDirective::~LocationDirective()
{
	for (std::map<std::string, std::vector<std::string> >::iterator it = _directives.begin(); it != _directives.end(); ++it) {
        it->second.clear();
    }
    _directives.clear();
}

void LocationDirective::addDirective(std::string& directiveName, std::string& value)
{
	_directives[directiveName].push_back(value);
}

std::map<std::string, std::vector<std::string > > LocationDirective::getDirectives(void) const
{
	return _directives;
}

std::vector<std::string> LocationDirective::getSpecificDirective(std::string directiveName)const
{
	std::map<std::string, std::vector<std::string> >::const_iterator it = _directives.find(directiveName);
    
    if(it != _directives.end())
        return it->second;
    else
        std::cout << RED << "Key not found!" << END << std::endl;
    return std::vector<std::string>();

}

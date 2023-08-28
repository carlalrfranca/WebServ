/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationDirective.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 21:13:30 by lfranca-          #+#    #+#             */
/*   Updated: 2023/08/28 12:56:10 by lfranca-         ###   ########.fr       */
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
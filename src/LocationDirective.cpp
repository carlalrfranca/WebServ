/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationDirective.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 21:13:30 by lfranca-          #+#    #+#             */
/*   Updated: 2023/08/27 19:21:39 by lfranca-         ###   ########.fr       */
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
	_directives[directiveName].push_back(value);
}
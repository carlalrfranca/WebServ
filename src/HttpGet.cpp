/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpGet.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:43:28 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/29 21:46:53 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"
#include "../inc/HttpGet.hpp"

ValidGet::ValidGet()
{}

ValidGet::~ValidGet()
{}

ValidGet::ValidGet(const std::string& root)
{
    _root = root;
    _fullPath = "";
}

void ValidGet::setUri(const std::string& uri)
{
    _uri = uri;
}

bool ValidGet::fileNotFound(const std::string& fullPath)
{
    return true;
}

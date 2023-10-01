/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpGet.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:43:28 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/01 13:46:46 by lfranca-         ###   ########.fr       */
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

//bool ValidGet::fileNotFound(const std::string& fullPath)
//{
//    return true;
//}

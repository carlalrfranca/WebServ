/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UtilsResponse.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/04 17:40:05 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _UTILS_RESPONSE_HPP_
#define _UTILS_RESPONSE_HPP_

#include "LocationDirective.hpp"
#include "HeadersLibs.hpp"
#include "SocketS.hpp"
#include "Request.hpp"

class Request;
class SocketS;
class LocationDirective;

class UtilsResponse
{
    public:
    
        UtilsResponse();
        ~UtilsResponse();

        bool isDirectory(const std::string& path);
        static std::string readHtmlFile(const std::string& filePath);
        static int selectServer(Request& request, std::vector<SocketS> serverSocket);
        static std::string readFileToString(const std::string& filename);
        static size_t strToSizeT(const std::string& str);
        std::map<std::string, LocationDirective>::iterator findRequestedLocation(Request &request, std::map<std::string, LocationDirective>& serverLocations);
        bool isThisMethodAllowed(std::map<std::string, LocationDirective>& serverLocations, Request &request, SocketS &server, std::string& requestMethod);
};

#endif
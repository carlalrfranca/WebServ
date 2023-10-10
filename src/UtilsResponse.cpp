/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UtilsResponse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/10 12:02:04 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/UtilsResponse.hpp"

UtilsResponse::UtilsResponse()
{}

UtilsResponse::~UtilsResponse()
{}

std::string UtilsResponse::readHtmlFile(const std::string& filePath)
{
    std::ifstream file(filePath.c_str());

    if(!file.is_open())
        return"";
    std::string content;
    char character;
    while(file.get(character))
        content += character;
    file.close();
    return content;
}

int UtilsResponse::selectServer(Request& request, std::vector<SocketS> serverSocket)
{
    int indexChosenSocket = -1;
    std::string domainRequest = request.getDomainRequest();
    std::string portRequest = request.getPortRequest();

	for(size_t serverIndex = 0; serverIndex < serverSocket.size(); ++serverIndex)
	{
		if(domainRequest == serverSocket[serverIndex].getAddress())
		{
			if(portRequest == serverSocket[serverIndex].getPort())
			{
				indexChosenSocket = serverIndex;
				return indexChosenSocket;
			}
		}
		else
		{
			std::vector<std::string> tempServerName = serverSocket[serverIndex].getServerName();
			for(size_t nameIndex = 0; nameIndex < tempServerName.size(); ++nameIndex)
			{
				if(domainRequest == tempServerName[nameIndex])
				{
					indexChosenSocket = serverIndex;
					return indexChosenSocket;
				}
			}
		}
	}
    return indexChosenSocket;
}

std::string UtilsResponse::readFileToString(const std::string& filename)
{
    std::ifstream file(filename.c_str());

    if(!file.is_open())
        return "";
    std::string content;
    std::string line;
    while(std::getline(file, line))
        content += line + "\n";
    file.close();
    return content;
}

bool UtilsResponse::isDirectory(const std::string& path)
{
    struct stat info;

    if(stat(path.c_str(), &info) != 0)
		return false;
    return(info.st_mode & S_IFDIR) != 0;
}

std::map<std::string, LocationDirective>::iterator UtilsResponse::findRequestedLocation(Request &request, std::map<std::string, LocationDirective>& serverLocations)
{
    std::map<std::string, LocationDirective>::iterator it = serverLocations.find(request.getURI());
	std::string uri = request.getURI();

	if(it == serverLocations.end())
	{
		bool canBeRootLoc = false;
		for(std::map<std::string, LocationDirective>::iterator locFind = serverLocations.begin(); locFind != serverLocations.end(); ++locFind)
		{
			if(locFind->first == "/")
				canBeRootLoc = true;
			if(locFind->first != "/")
			{
		        if(uri == locFind->first || uri.find(locFind->first + "/") != std::string::npos || uri.find(locFind->first) != std::string::npos)
				{
	                it = locFind;
	                return it;
	            }
			}
	    }
		if(it == serverLocations.end() && canBeRootLoc == true)
			it = serverLocations.find("/");
	}
	return it;
}

bool UtilsResponse::isThisMethodAllowed(std::map<std::string, LocationDirective>& serverLocations, Request &request, SocketS &server, std::string& requestMethod)
{
	std::vector<std::string> allowed_methods = server.getMethods();
	std::map<std::string, LocationDirective>::iterator itLocation = findRequestedLocation(request, serverLocations);
	if(itLocation != serverLocations.end())
	{
		std::map<std::string, std::vector< std::string > > locationDirectives;
		locationDirectives = itLocation->second.getDirectives();
		std::map<std::string, std::vector< std::string > >::iterator itLocationAllowMethods = locationDirectives.find("allow_methods");
		if(itLocationAllowMethods != locationDirectives.end())
		{
			std::vector<std::string> locationAllowedMethods = itLocationAllowMethods->second;
			for(std::vector<std::string>::iterator it = locationAllowedMethods.begin(); it != locationAllowedMethods.end(); ++it)
			{
				if(strcmp(it->c_str(), requestMethod.c_str()) == 0)
				{
					request.setAllowedMethods(locationAllowedMethods);
					return true;
				}
			}
		} else {
			for(std::vector<std::string>::iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it)
			{
				if(strcmp(it->c_str(), requestMethod.c_str()) == 0)
				{
					request.setAllowedMethods(allowed_methods);
					return true;
				}
			}
		}
	}
	return false;
}

size_t UtilsResponse::strToSizeT(const std::string& str)
{
	size_t sizeValue = 0;
    std::istringstream ss(str);
    ss >> sizeValue;
	return sizeValue;
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketS.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 17:46:53 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/10 13:21:21 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"
#include "../inc/SocketS.hpp"

SocketS::SocketS()
{
    _portNumber = "";
    _ipAddress = "";
    _response = "";
    _webServSocket = 0;
    _root = "";
    _statusCode = "0";
	_indexFile = "";
	_maxBodySize = 1024;
}


SocketS::~SocketS()
{
	for (std::map<std::string, LocationDirective>::iterator it = _locations.begin(); it != _locations.end(); ++it)
	{
		it->second.~LocationDirective();
    }
}

std::map<std::string, LocationDirective> SocketS::getLocations() const
{
	return _locations;
}

void SocketS::setLocations(std::map<std::string, LocationDirective> locations)
{
	_locations = locations;
} 

const int& SocketS::getFD()const
{
    return _webServSocket;
}

void SocketS::loadConfiguration()
{}

void SocketS::setPort(std::string portNumber)
{
    unsigned int port = 0;

    for(size_t i = 0; i < portNumber.length(); ++i){
        if(!std::isdigit(portNumber[i]))
            throw SocketSException("Error: Wrong Syntax: port");
    }
	port = atoi(portNumber.c_str());
    if(port < 1 || port > 65636)
        throw SocketSException("Error: Wrong Syntax: port");
    this->_portNumber = portNumber;
}

void SocketS::setAddress(std::string ipAddress)
{
    this->_ipAddress = ipAddress;
}

void SocketS::setWebServSocket(int webServSocket)
{
    this->_webServSocket = webServSocket;
}

void SocketS::setMaxBodySize(size_t maxBodySize)
{
    this->_maxBodySize = maxBodySize;
}

size_t SocketS::getMaxBodySize()const
{   
    return _maxBodySize;
}

const std::string& SocketS::getPort()const
{
    return _portNumber;
}

const std::string& SocketS::getAddress()const
{
    return _ipAddress;
}

const std::string SocketS::getErrorPage(int statusCode)const
{
	std::stringstream toConvertToStr;
    toConvertToStr << statusCode;
    std::string statusCodeStr = toConvertToStr.str();
	std::map<std::string, std::string>::const_iterator it = _errorPage.find(statusCodeStr);
	if(it != _errorPage.end())
		return it->second;
	return "";
}

const int& SocketS::getWebServSocket()const
{
    return _webServSocket;
}

void SocketS::setServerName(std::vector<std::string> serverName)
{
     this->_serverName = serverName;
}

std::vector<std::string> SocketS::getServerName()const
{
    return _serverName;
}
 
void SocketS::setMethods(std::vector<std::string> methods)
{
    _methods = methods;
}

std::vector<std::string> SocketS::getMethods()const
{
    return _methods;
}

void SocketS::setRoot(std::string root)
{
    _root = root;
}

const std::string& SocketS::getRoot()const
{
    return _root;
}

const std::string& SocketS::getIndexFile()const
{
	return _indexFile;
}

void SocketS::setIndexFile(std::string index)
{
	_indexFile = index;
}

void SocketS::setErrorPage(std::map<std::string, std::string> errorPages)
{
	_errorPage = errorPages;
}

void SocketS::addAliasToHostsFile(const std::string& alias)
{
    std::string command = "echo '127.0.0.1 " + alias + "' >> /etc/hosts";
    int result = system(command.c_str());
    if(result != 0)
        throw SocketSException("Error while adding the alias to the file /etc/hosts.");
    result = system("systemctl restart systemd-resolved");
    if (result == -1)
        std::cout << "Server_Name Error: Failed to flush DNS cache." << std::endl;
}

void SocketS::initServer()
{
	setWebServSocket(socket(AF_INET, SOCK_STREAM, 0));
	if(getWebServSocket() == -1)
	{
        throw SocketSException("Socket Error: Failed to create socket!");
    }
    struct sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(std::atoi(getPort().c_str()));
    server_address.sin_addr.s_addr = INADDR_ANY;

    if(bind(getWebServSocket(),(struct sockaddr*)&server_address, sizeof(server_address)) == -1)
	{
        close(getWebServSocket());
        throw SocketSException("Socket Error: Bind failed!");
    }
    if(listen(getWebServSocket(), 5) == -1)
	{
        close(getWebServSocket());
        throw SocketSException("Socket Error: Listen failed!");
	}
	std::string serverName;
	if(!_serverName.empty())
	{
		for(std::vector<std::string>::const_iterator it = _serverName.begin(); it != _serverName.end(); ++it)
		{
			serverName = *it;
			addAliasToHostsFile(serverName);
		}
	}
}

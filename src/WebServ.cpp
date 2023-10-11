/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/11 18:14:52 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/WebServ.hpp"

WebServ::WebServ()
{}

WebServ::~WebServ()
{
    _serverSocket.clear();
}

void WebServ::setResponse(const std::string& response)
{
    _response = response;
}

std::string WebServ::getResponse()const 
{
    return _response;
}

WebServ::WebServ(std::string filename)
{
    std::ifstream fileToParse;
	size_t index = 0;
    fileToParse.open(filename.c_str());
    char contentFile;
	_totalBytesRead = 0;
	_contentLength = 0;

    if(!(fileToParse >> contentFile))
    {
        fileToParse.close();
        throw ErrorException("Configuration Error: empty file!");
    }
    fileToParse.unget();
    _configParser.validateFile(fileToParse);

    if(fileToParse.is_open())
    {
        std::string line;
        bool isLocationBlock = false;
        bool isInsideServerBlock = false;
        while(getline(fileToParse, line))
        {
            _utils.trimWhiteSpace(line);
			size_t startPos = line.find_first_not_of(" \t");
    		if(startPos == std::string::npos)
				continue;
            _utils.removeComments(line);
            size_t semicolon = line.find_last_not_of(" \t");
            if(semicolon != std::string::npos && line[semicolon] == ';')
                line = line.substr(0, semicolon);   
            if(line.find("server{") != std::string::npos)
			{
				if(isInsideServerBlock == true || isLocationBlock == true)
					throw ErrorException("Configuration Error: Server Block INSIDE another Server or Location block!");
                if(index > 0)
                    configSocket();
				index++;
			    isInsideServerBlock = true;
            }
            else if(!isInsideServerBlock && !isLocationBlock && !line.empty())
                throw ErrorException("Wrong character out of server scope: " + line);
            else if(line.find("}") != std::string::npos && isLocationBlock == false)
            {
                isInsideServerBlock = false;
                continue;
            }
            else if(isInsideServerBlock)
            {
    			if(line.find("location") != std::string::npos || isLocationBlock == true)
    			{
                    if(line.find("location") != std::string::npos && isLocationBlock == true)
						throw ErrorException("Configuration Error: Can't have location block inside of another location block");
					if(line.find("}") != std::string::npos)
					{
                        isLocationBlock = false;
    					continue;
    				}
                    _configParser.processLocation(line);
                    isLocationBlock = true;
                }
                else if(line.find("listen") != std::string::npos)
                    _configParser.processListen(line);
                else if(line.find("server_name") != std::string::npos)
                    _configParser.processServerName(line);
                else if(line.find("root") != std::string::npos)
                    _configParser.processRoot(line);
                else if(line.find("index") != std::string::npos)
                    _configParser.processIndex(line);
    			else if(line.find("allow_methods") != std::string::npos)
    				_configParser.processAllowMethods(line);
    			else if(line.find("client_max_body_size") != std::string::npos)
    				_configParser.processClientMaxBodySize(line);
    			else if(line.find("return") != std::string::npos)
    				_configParser.processReturn(line);
    			else if(line.find("error_page") != std::string::npos)
                    _configParser.processErrorPage(line);
                else {
					std::cout << line << std::endl;
					throw ErrorException("Configuration Error: Directive not allowed!");
				}
            }
        }
        if(index == 0)
            throw ErrorException("Configuration Error: Server Block not found!");
		if(isLocationBlock == true || isInsideServerBlock == true)
			throw ErrorException("Configuration Error: Server Block or Location Block not closed!");
    } else
        throw ErrorException("File Error : file cannot be opened");
    if(index != 0)
        configSocket();
    fileToParse.close();
	initServers();
	mainLoop();
}

void WebServ::checkForDuplicates()
{
    for(size_t i = 0; i < _serverSocket.size() - 1; ++i)
	{
        for(size_t j = i + 1; j < _serverSocket.size(); ++j)
		{
            if(_serverSocket[i].getPort() == _serverSocket[j].getPort() && _serverSocket[i].getAddress() == _serverSocket[j].getAddress())
			{
				std::vector<std::string> currentSocketServerNames = _serverSocket[i].getServerName();
				std::vector<std::string> nextSocketServerNames = _serverSocket[j].getServerName();
				for(size_t k = 0; k < currentSocketServerNames.size(); ++k)
				{
					for(size_t l = 0; l < nextSocketServerNames.size(); ++l)
					{
						if(currentSocketServerNames[k] == nextSocketServerNames[l])
							throw ErrorException("Configuration Error : Duplicated server_name in multiple servers");
                    }
				}
			}
			else if(_serverSocket[i].getPort() == _serverSocket[j].getPort() && _serverSocket[i].getAddress() != _serverSocket[j].getAddress())
			{
					throw ErrorException("Configuration Error : Duplicated Port in multiple servers");
			}
        }
    }
}

void WebServ::configSocket()
{
	if(_configParser.getRoot().empty())
		_configParser.setRoot("./");
	if(_configParser.getAddress().empty())
		_configParser.setAddress("127.0.0.1");
	if(_configParser.getAllPorts().size() == 0)
		throw ErrorException("Configuration Error: [MANDATORY DIRECTIVE] Port not found!");
	if(_configParser.getMethods().size() == 0)
		throw ErrorException("Configuration Error: [MANDATORY DIRECTIVE] It is necessary at least ONE allowed method at server level.");
	std::vector<std::string> tmpPorts = _configParser.getAllPorts();
	std::vector<std::string> tmpAddress = _configParser.getAllIps();
	size_t totalPorts = tmpPorts.size();
	
	for(size_t i = 0; i < totalPorts; i++)
	{
    	SocketS temp_socket;
		temp_socket.setPort(tmpPorts[i]);
		temp_socket.setAddress(tmpAddress[i]);
		temp_socket.setLocations(_configParser.getLocations());
	    temp_socket.setMethods(_configParser.getMethods());
	    temp_socket.setRoot(_configParser.getRoot());
		temp_socket.setIndexFile(_configParser.getIndexFile());
		temp_socket.setErrorPage(_configParser.getErrorPage());
		temp_socket.setServerName(_configParser.getDomains());
		temp_socket.setMaxBodySize(_configParser.getMaxBodySize());
	    _serverSocket.push_back(temp_socket);
	}
	_configParser.resetConfig();
}

void WebServ::initServers()
{
	checkForDuplicates();
	bool isDuplicated = false;

	for(std::vector<SocketS>::iterator it = _serverSocket.begin(); it != _serverSocket.end(); ++it)
	{
		for(std::vector<SocketS>::iterator it2 = _serverSocket.begin(); it != it2; ++it2)
		{
			if(it2->getAddress() == it->getAddress() && it2->getPort() == it->getPort())
			{
				it->setWebServSocket(it2->getWebServSocket());
				isDuplicated = true;
			}
		}
		if(!isDuplicated)
			it->initServer();
	}
}

void WebServ::printRequest(const std::string& request)
{
    std::istringstream iss(request);
    std::string line;

    while(std::getline(iss, line) && line != "\r")
        std::cout << line << std::endl;
}

bool WebServ::isFirstLineValid(const std::string& request, std::string& _firstLine)
{
    std::istringstream requestStream(request);
    std::getline(requestStream, _firstLine);

    if(_firstLine.find("GET") == std::string::npos && 
        _firstLine.find("POST") == std::string::npos && 
        _firstLine.find("DELETE") == std::string::npos)
        return false;
    size_t spacePos = _firstLine.find(' ');
    if(spacePos == std::string::npos || spacePos == _firstLine.size() - 1 )
        return false;
    std::string version = _firstLine.substr(spacePos + 1);
    if(version.find("HTTP/1.1") == std::string::npos)
        return false;
    spacePos = version.find(' ');
    if(spacePos == std::string::npos || spacePos == version.size() - 1)
        return false;
    return true;
}

Epoll& WebServ::getEpollS()
{
    return _epollS;
}

bool WebServ::isEventFromServerSocket(struct epoll_event* events, int index)
{
    _epollS.setIsServerFdTriggered(false);

    for(size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex)
    {
        if(events[index].data.fd == _serverSocket[serverIndex].getWebServSocket())
        {
            getEpollS().setIsServerFdTriggered(true);
            break;
        }
    }
    return false;
}

void WebServ::handleRequest(std::string& requestString)
{
    Request request(requestString);
    Response currentResponse;
	std::cout << request.getMethod() << std::endl;
	int resultCode = request.validateRequest();

    if(resultCode != 0)
	{
		request.setHasError(true);
		_response = request.errorCodeHtml(resultCode);
		return ;
	}
    int selectedServer = UtilsResponse::selectServer(request, _serverSocket);

    if(selectedServer != -1)
	{
		std::cout << YELLOW << "CONNECTION WITH SERVER ---- " << _serverSocket[selectedServer].getAddress() << ":" << _serverSocket[selectedServer].getPort() << END << std::endl;
        currentResponse.buildResponse(request, _serverSocket[selectedServer]);
        _response = currentResponse.getResponse();
    } 
    else
        std::cout << "Error: Server not found!" << std::endl;
}

int	WebServ::convertContentLength(std::string& header, size_t contentLengthPos)
{
	contentLengthPos += strlen("Content-Length: ");
    size_t lineEndPos = header.find("\r\n", contentLengthPos);

    if(lineEndPos != std::string::npos)
	{
		std::string contentLengthStr = header.substr(contentLengthPos, lineEndPos - contentLengthPos);
		try
		{
			_contentLength = std::atoi(contentLengthStr.c_str());
        } catch (const std::invalid_argument& e)
		{
			std::cout << "Error of conversion." << e.what() << std::endl;
        }
    }
	return 0;
}

bool WebServ::hasBodyContent(const std::string& header)
{
	size_t contentLengthPos = header.find("Content-Length: ");
	if(contentLengthPos != std::string::npos)
		return true;
	return false;
}

void WebServ::removeClientFromEpoll(Epoll& epollS)
{
	struct epoll_event event;
	
	event = epollS.getEvent();
	event.data.fd = epollS.getClientFd();
	
	epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_DEL, epollS.getClientFd(), &event);
	epollS.setEvent(event);
	close(epollS.getClientFd());
}

int WebServ::readRequest(int clientSocket)
{
	char buffer[1000];
	_epollS.setClientFd(clientSocket);
	std::string header;
	std::string body;
	while(!shouldExit)
	{
		ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if(bytesRead == 0)
		{
			std::cerr << "Client closed connection." << std::endl;
			_epollS._event.data.fd = clientSocket;
			_epollS._event.events = EPOLLOUT;
			epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_DEL, clientSocket, &_epollS._event);
			close(_epollS.getClientFd());
			_requestString = "";
			_contentLength = 0;
			_totalBytesRead = 0;
			return 0;
		}
		else if(bytesRead == -1)
		{
			std::cerr << "Error during reading of client request." << std::endl;
			_epollS._event.data.fd = clientSocket;
			_epollS._event.events = EPOLLOUT;
			epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_DEL, clientSocket, &_epollS._event);
			close(_epollS.getClientFd());
			_requestString = "";
			_contentLength = 0;
			_totalBytesRead = 0;
			return -1;
		}
		else if(bytesRead != 0)
		{
			_requestString.append(buffer, bytesRead);
			_totalBytesRead += bytesRead;
			memset(buffer, 0, sizeof(buffer));
		}
		size_t headerEndPos = _requestString.find("\r\n\r\n");
		if(headerEndPos != std::string::npos) {
			if(header.size() == 0)
			{
			    header = _requestString.substr(0, headerEndPos + 4);
			}
			if(_contentLength == 0 && hasBodyContent(header) == true)
			{
				size_t contentLengthPos = header.find("Content-Length: ");
				convertContentLength(header, contentLengthPos);
			}
			else if(hasBodyContent(header) == false)
				_contentLength = -1;
			if((_totalBytesRead - header.size()) == _contentLength)
			{
				handleRequest(_requestString);
				_epollS._event.data.fd = clientSocket;
				_epollS._event.events = EPOLLOUT;
				epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_MOD, clientSocket, &_epollS._event);
				_requestString = "";
				_contentLength = 0;
				_totalBytesRead = 0;
				return 0;
			}
			else if(_contentLength == (size_t)-1)
			{
				handleRequest(_requestString);
				_epollS._event.data.fd = clientSocket;
				_epollS._event.events = EPOLLOUT;
				epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_MOD, clientSocket, &_epollS._event);
				_requestString = "";
				_contentLength = 0;
				_totalBytesRead = 0;
				return 0;
			}
			return 0;
		}
	}
	return 0;
}

void WebServ::mainLoop()
{
    std::cout << YELLOW << "-----------------------------------------" << END << std::endl;
    std::cout << YELLOW << "SERVER INITIALIZED. WAITING FOR CONNECTIONS..." << END << std::endl;
    std::cout << YELLOW << "-----------------------------------------\n" << END << std::endl;
    
	_epollS.addServersToEpoll(_serverSocket);
    int epollFd = _epollS.getEpollFd();

    if(epollFd == -1 || epollFd == -2)
		return;
    const int maxEvents = _epollS.getMaxEvents();
    struct epoll_event events[maxEvents];
    
	while(!shouldExit)
    {
        _epollS.setNumberEvents(epoll_wait(epollFd, events, maxEvents, -1));
        if(_epollS.getNumberEvents() == -1)
        {
			for(size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex)
				close(_serverSocket[serverIndex].getWebServSocket());
			close(_epollS.getEpollFd());
            return;
        }
        for(int index = 0; index < _epollS.getNumberEvents(); ++index)
        {
			isEventFromServerSocket(events,index);
			if(events[index].events & EPOLLIN)
			{
            	if(_epollS.getIsServerFdTriggered() == true)
            	{
            	    int result = _epollS.addNewClientToEpoll(events, index);
            	    if(result == -3)
            	        continue;
            	} else {
					int clientSocket = events[index].data.fd;
					readRequest(clientSocket);
            	}
			}
			else if(events[index].events & EPOLLOUT)
			{
				if(_epollS.getClientFd() == events[index].data.fd)
				{
                    ssize_t bytesSent = send(_epollS.getClientFd(), _response.c_str(), _response.length(), 0);
                    if(bytesSent == -1 || bytesSent == 0)
					{
						_epollS._event.data.fd = _epollS.getClientFd();
						_epollS._event.events = EPOLLOUT;
						epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_DEL, _epollS.getClientFd(), &_epollS._event);
						close(_epollS.getClientFd());
					} else {
                        std::cout << YELLOW << "----- SENT DATA AND CLOSED CONNECTION WITH CLIENT ----" << END << std::endl;
						_epollS._event.data.fd = _epollS.getClientFd();
						_epollS._event.events = EPOLLOUT;
						epoll_ctl(_epollS.getEpollFd(), EPOLL_CTL_DEL, _epollS.getClientFd(), &_epollS._event);
						close(_epollS.getClientFd());
                    }
                }
			}
        }
	}
	for(size_t serverIndex = 0; serverIndex < _serverSocket.size(); ++serverIndex)
		close(_serverSocket[serverIndex].getWebServSocket());
	close(_epollS.getEpollFd());
}

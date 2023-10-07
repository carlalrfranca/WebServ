/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 21:24:02 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/05 21:44:40 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"
#include "../inc/ConfigParser.hpp"
#include "../inc/Utils.hpp"

ConfigParser::ConfigParser()
{
    //_line = ""; // talvez, mas acho que nao precisa mesmo
    // _domains = ""; // server_name
    // _rules = "";    // location
    _directive = 0;
    _portNumber = "";
    _ipAddress = "";
    _path = "";
    _root = "";
    _hasRoot = true;
    _hasDirListen = false;
    _hasDirServerName = false;
    _hasDirIndex = false;
    _hasDirAllowMethods = false;
    _hasDirMaxBodySize = false;
    _hasDirReturn = false;
	_indexFile = "";
	_maxBodySize = 1024;

	// configurando por padrão os paths das paginas de erro (se o arquivo personalizar alguma, vai sobrescrever na hora em que estiver processando a diretiva de error_page)
	_errorPage["404"] = "./web/error/Error404.html";
	struct stat info;
		if (stat(_errorPage["404"].c_str(), &info) != 0)
			std::cout << "This error page does not exist" << std::endl;
		else
			std::cout << "This error page exist" << std::endl;
	_errorPage["400"] = "./web/error/Error400.html";
	_errorPage["401"] = "./web/error/Error401.html";
	_errorPage["403"] = "./web/error/Error403.html";
	_errorPage["405"] = "./web/error/Error405.html";
	_errorPage["413"] = "./web/error/Error413.html";
	_errorPage["500"] = "./web/error/Error500.html";
	_errorPage["501"] = "./web/error/Error501.html";
	_errorPage["503"] = "./web/error/Error503.html";
	_errorPage["504"] = "./web/error/Error504.html";
	_errorPage["505"] = "./web/error/Error505.html";
}

ConfigParser::~ConfigParser()
{}

void ConfigParser::resetConfig()
{
	_hasDirAllowMethods = false;
	_hasDirIndex = false;
	_hasDirListen = false;
	_hasDirLocation = false;
	_hasDirMaxBodySize = false;
	_hasDirReturn = false;
	_hasDirServerName = false;
	_maxBodySize = 1024;
    _directive = 0;
    // _portNumber = "";
    // _ipAddress = "";
    // _httpAddress = "";
    _path = "";
    _root = "";
    _hasRoot = true;
	_indexFile = "";

	// _indexFiles.clear();
	_locationsMap.clear();
	_locations.clear();
	_allIps.clear();
	_allPorts.clear();
	_methods.clear();
	_domains.clear();

	// resetar as paginas de erro padrão
	_errorPage["400"] = "./web/error/Error400.html";
	_errorPage["401"] = "./web/error/Error401.html";
	_errorPage["403"] = "./web/error/Error403.html";
	_errorPage["405"] = "./web/error/Error405.html";
	_errorPage["413"] = "./web/error/Error413.html";
	_errorPage["500"] = "./web/error/Error500.html";
	_errorPage["501"] = "./web/error/Error501.html";
	_errorPage["503"] = "./web/error/Error503.html";
	_errorPage["504"] = "./web/error/Error504.html";
	_errorPage["505"] = "./web/error/Error505.html";
}

std::map<std::string, LocationDirective> ConfigParser::getLocations() const
{
	return _locations;
}

void ConfigParser::setLocations(std::map<std::string, LocationDirective>& locations)
{
	_locations = locations;
}

void ConfigParser::setPort(int portNumber)
{
    this->_portNumber = portNumber;
}

void ConfigParser::setAddress(std::string ipAddress)
{
    this->_ipAddress = ipAddress;
}

const std::string& ConfigParser::getPort()const
{
    return _portNumber;
}

const std::string& ConfigParser::getAddress()const
{
    return _ipAddress;
}

void ConfigParser::setRoot(std::string root)
{
    _root = root;
}

const std::string& ConfigParser::getRoot()const
{
    return _root;
}

std::map<std::string, std::string> ConfigParser::getErrorPage()const
{
    return _errorPage;
}

const std::string& ConfigParser::getIndexFile()const
{
	return _indexFile;
}

void ConfigParser::setIndexFile(std::string index)
{
	_indexFile = index;
}

void ConfigParser::setMaxBodySize(size_t maxBodySize)
{
	_maxBodySize = maxBodySize;
}

size_t ConfigParser::getMaxBodySize()const
{
	return _maxBodySize;
}

const std::vector<std::string>& ConfigParser::getMethods()const
{
    return _methods;
}

const std::vector<std::string>& ConfigParser::getDomains()const
{
	return _domains;
}

bool ConfigParser::getAutoIndex()const
{
	return _autoIndexOn;
}

const std::vector<std::string>& ConfigParser::getAllPorts()const
{
	return _allPorts;
}

const std::vector<std::string>& ConfigParser::getAllIps()const
{
	return _allIps;
}

bool ConfigParser::duplicatedPort(std::string& portNumber)
{
	for (std::vector<std::string>::iterator it = _allPorts.begin(); it != _allPorts.end(); ++it)
	{
    	if (*it == portNumber)
    	    return true;
	}
	return false;
}

bool ConfigParser::isValidIPAddress(const std::string &ipAddress)
{
	for(size_t i = 0; i < ipAddress.length(); ++i)
	{
		if(!(ipAddress[i] == '.' || (ipAddress[i] >= '0' && ipAddress[i] <= '9')))
			return false;
	}
	std::istringstream splitOctets(ipAddress);
	std::string octet;
	int count = 0;
	while(std::getline(splitOctets, octet, '.'))
	{
		count++;
		if(count > 4)
			return false;
	}
	if (ipAddress != "127.0.0.1" && ipAddress != "0.0.0.0") //tem mais algum tipo de IP que pode?
		return false;
	int value;
    std::istringstream octetStream(octet);
    octetStream >> value;
    if (octetStream.fail() || value < 0 || value > 255)
        return false;
    return count == 4;
}

bool ConfigParser::isValidPort(const std::string &port)
{
	int portNumber;
	std::istringstream portStream(port);
	portStream >> portNumber;
	if(!portStream.fail() && portNumber >= 1 && portNumber <= 65535)
		return true;
	return false;
}

void ConfigParser::processListen(std::string &line)
{
	size_t	posInit;

	if (line[line.length() - 1] == ';')
		line = line.substr(0,  line.length() - 1);
	std::istringstream listen_values(line);
	std::string word;
    std::vector<std::string> tokens;
	while(listen_values >> word)
		tokens.push_back(word);
	if(tokens.size() != 2)
		throw ErrorException("Syntax Error: Only ONE value by listen directive.");
	if(tokens[0] != "listen")
		throw ErrorException("Syntax Error: Format of line: [directive] [value] ...");
	posInit = tokens[1].find("https");
	if(posInit != std::string::npos)
		throw ErrorException("Configuration Error: Can't configure https protocol.");
	posInit = tokens[1].find("://");
	if(posInit != std::string::npos)
	{
		posInit += 3;
		tokens[1] = tokens[1].substr(posInit);
		std::cout << "Valor da listen sem '://' >> " << tokens[1] << std::endl;
	}
	posInit = tokens[1].find(":");
	if(posInit != std::string::npos)
	{
		_ipAddress = tokens[1].substr(0, posInit);
		_portNumber = tokens[1].substr(posInit + 1);
		if(_ipAddress == "localhost")
			_ipAddress = "127.0.0.1";
		if(!isValidIPAddress(_ipAddress))
			throw ErrorException("Configuration Error: Invalid IP Address!");
		if(!isValidPort(_portNumber))
			throw ErrorException("Configuration Error: Invalid port number!");
		if(duplicatedPort(_portNumber))
			throw ErrorException("Configuration Error: Cannot have repeated port in server!");
		_allPorts.push_back(_portNumber);
		_allIps.push_back(_ipAddress);
		if(!_utils.containsOnlyNumbers(_portNumber))
			throw ErrorException("Syntax Error: Port must be ONLY numbers.");
	} else {
		if(_utils.containsOnlyNumbers(tokens[1]))
		{
			_portNumber = tokens[1];
			_ipAddress = "127.0.0.1";
			if(duplicatedPort(_portNumber))
				throw ErrorException("Configuration Error: Cannot have repeated port in server.");
			_allPorts.push_back(_portNumber);
			_allIps.push_back(_ipAddress);
		}
		else
			throw ErrorException("Configuration Error: You must configure a PORT NUMBER!");
	}
}

void ConfigParser::processServerName(std::string &line)
{
    if(_hasDirServerName == true)
        throw ErrorException("Error: The Directive Server_Name has been duplicated.");
    if(line.find("server_name") != std::string::npos)
	{
		if(line[line.length() - 1] == ';')
			line = line.substr(0, line.length() - 1);
		// vamos splitar essa linha
		std::istringstream server_name_values(line);
		std::string word;
    	std::vector<std::string> tokens;
	
		while(server_name_values >> word)
		{
			if(word.find_first_not_of("abcdefghijklmnopqrstuvwxyz._") != std::string::npos)
				throw ErrorException("Syntax Error: Server_name can only be written with [letters and dot]. You CANNOT use regex or wildcards, only LITERAL names.");
        	tokens.push_back(word);
    	}
		// o problema é que no vetor "tokens", ele gravou o nome da diretiva (server_name), temos que extrai-la do vetor
		tokens.erase(tokens.begin());
		for(size_t i = 0; i < tokens.size(); ++i)
		{
			std::cout << BLUE << "Dominio: " << tokens[i] << END << std::endl;
        	_domains.push_back(tokens[i]);
		}
    }
    _hasDirServerName = true;
}

bool ConfigParser::processRoot(std::string &line)
{
	std::istringstream iss(line);
	std::vector<std::string> partes;
	std::string word;
	if(_root.empty() == false)
		throw ErrorException("Error: The Directive Root has been duplicated.");
	while (iss >> word)
		partes.push_back(word);
	if(partes[0] != "root" || partes.size() != 2)
		throw ErrorException("Syntax Error: Directive Root");
	_root = partes[1];
	struct stat info;
	if(stat(_root.c_str(), &info) != 0)
		throw ErrorException("Error: The path apointed by 'root' doesn't exist.");
	if((info.st_mode & S_IFDIR) == 0)
		throw ErrorException("Error: The path apointed by 'root' ISN'T A DIRECTORY.");
	return _hasRoot;
}

void ConfigParser::hasprohibitedDirectiveInLocation(std::string &directive)
{
	std::vector<std::string> allowedDirectives;
	allowedDirectives.push_back("allow_methods");
    allowedDirectives.push_back("index");
    allowedDirectives.push_back("auto_index");
    allowedDirectives.push_back("root");
    allowedDirectives.push_back("client_max_body_size");
    allowedDirectives.push_back("cgi_path");
    allowedDirectives.push_back("cgi_ext");
    allowedDirectives.push_back("upload_store");
	allowedDirectives.push_back("return");

	std::vector<std::string>::iterator itAllowedDirective;
    for(itAllowedDirective = allowedDirectives.begin(); itAllowedDirective != allowedDirectives.end(); ++itAllowedDirective)
	{
        if(*itAllowedDirective == directive)
            break;
    }
	if(itAllowedDirective == allowedDirectives.end())
		throw ErrorException("Configuration Error: Prohibited directive in location block");
}

void ConfigParser::storeCurrentLocationDirectives(std::string &directiveLine)
{
	std::string currentLocation = _currentLocationPathOnMap;
	std::map<std::string, LocationDirective>::iterator it = _locations.find(currentLocation);
	if(it != _locations.end())
	{
		// acessa o método atraves do iterador second.método()
		// mas antes tem que extrair o nome da diretiva e o valor (splitar e colocar num vetor?)
		// tambem contar quantos itens resultam do split porque isso quer dizer que a diretiva tem mais de um valor...
		// primeiro, splitamos a linha da diretiva para poder separar o nome da diretiva de seus valores
		std::istringstream iss(directiveLine);
		std::vector<std::string> splittedLine;
		std::string singleValue;
		while(iss >> singleValue)
			splittedLine.push_back(singleValue);
		if(splittedLine.size() < 2)
			throw ErrorException("Syntax Error: Missing values to directive in location block");
		hasprohibitedDirectiveInLocation(splittedLine[0]);
		std::map<std::string, std::vector<std::string> >::iterator directiveIt = it->second.getDirectives().find(splittedLine[0]);
		if(directiveIt != it->second.getDirectives().end())
			throw ErrorException("Syntax Error: Duplicated directive in location block");
		// com os argumentos splitados e num vetor, vamos inserir a key no objeto
		it->second.addDirective(splittedLine[0], splittedLine[1]);
		// diretiva (se há mais itens) // tem que limitar
		/*
			Diretivas que PODEM TER MAIS DE UM VALOR:
				- allow_methods
				- return

			Diretivas que NÃO PODEM TER MAIS DE UM VALOR:
		*/
		if (splittedLine.size() > 2 && splittedLine[0] != "allow_methods" && splittedLine[0] != "return")
			throw ErrorException("Configuration Error: [LOCATION] Directive of one value with multiple values.");
		if(splittedLine.size() > 2)
		{
			size_t totalValues = splittedLine.size();
			size_t currentIndex = 2;
			while(currentIndex < totalValues)
			{
				it->second.addDirective(splittedLine[0], splittedLine[currentIndex]);
				std::cout << currentIndex << std::endl;
				currentIndex++;
			}
		}
	}
}

void ConfigParser::validateFile(std::ifstream& fileToParse)
{
    if(!fileToParse.is_open())
        throw ErrorException("File is not accessible");
    fileToParse.seekg(0, std::ios::end);
    std::streampos fileSize = fileToParse.tellg();
    fileToParse.seekg(0, std::ios::beg);
    if(fileSize <= 0)
        throw ErrorException("File is empty");
}

void ConfigParser::processLocation(std::string &line)
{
    std::vector<std::string> filesForPath1;
    if(line.find("location")!= std::string::npos)
    {
		if(line[line.size() - 1] != '{')
			throw ErrorException("Syntax Error: Missing '{' to open LOCATION BLOCK");
        std::istringstream iss(line);
        std::vector<std::string> parts;
    
        std::string part;
        while(iss >> part)
            parts.push_back(part);
        _currentLocationPathOnMap = parts[1];
		std::map<std::string, std::vector<std::string> >::iterator it = _locationsMap.find(_currentLocationPathOnMap);
		if(it != _locationsMap.end())
			throw ErrorException("Syntax Error: DUPLICATED location");
        _locationsMap[_currentLocationPathOnMap];
		_locations[_currentLocationPathOnMap];
    }
    else
        storeCurrentLocationDirectives(line);
}

void ConfigParser::processIndex(std::string &line)
{
	std::vector<std::string>	_indexFiles;
    if(_hasDirIndex == true)
        throw ErrorException("Error: The Directive Index has been duplicated.");
    if(line.find("index") != std::string::npos)
    {
        std::istringstream indicesStream(line);
        std::string index;
        while(indicesStream >> index)
            _indexFiles.push_back(index);
		if (_indexFiles.size() != 2)
			throw ErrorException("Configuration Error: You must point ONE index file.");
		if (_indexFiles[0] != "index")
			throw ErrorException("Syntax Error: Correct Format: [directive] [value]");
		_indexFile = _indexFiles[1];
    }
    _hasDirIndex = true;
}

void ConfigParser::processErrorPage(std::string &line)
{
    if(line.find("error_page") != std::string::npos)
    {
        std::istringstream iss(line);
        std::vector<std::string> errorCode;
        std::string token;
        while(iss >> token)
        {
            std::cout << "error_page: " << token << std::endl;
            errorCode.push_back(token);   
        }
        if(errorCode.size() != 3 || errorCode[0] != "error_page")
            throw ErrorException("Syntax Error: Found inconsistence in error_page directive");
		std::string statusCode;
        statusCode = errorCode[1];
        for(size_t i = 0; i < statusCode.size(); ++i)
		{
            if(!isdigit(statusCode[i]))
                throw ErrorException("Syntax Error: Found inconsistence in status code apointed by error_page directive");
        }
        std::string pathError;
        pathError = errorCode[2];
		struct stat info;
		if(stat(pathError.c_str(), &info) != 0)
			throw ErrorException("Configuration Error: Error page file doesn't exist!");
        _errorPage[statusCode] = pathError; // Armazena a página de erro no mapa
    }
}

void ConfigParser::processAllowMethods(std::string &line)
{
    if(_hasDirAllowMethods == true)
        throw ErrorException("Error: The Directive Allow Methods has been duplicated.");
    std::vector<std::string> methods;
    std::istringstream iss(line);
    std::string method;
    while(iss >> method)
        methods.push_back(method);
    if(methods.size() < 2 || methods[0] != "allow_methods")
		throw ErrorException("Syntax Error: The Directive Allow Methods isn't properly configured.");
	_methods = methods;
	_methods.erase(_methods.begin());
	for(size_t i = 0; i < _methods.size(); ++i)
	{
		if(_methods[i] != "GET" && _methods[i] != "POST" && _methods[i] != "DELETE")
			throw ErrorException("Configuration Error: Only GET | POST | DELETE methods are accepted.");
	}
    _hasDirAllowMethods = true;
}

size_t ConfigParser::convertToKB(std::string &sizeStr)
{
	int inBytes = 1;

	char suffix = sizeStr[sizeStr.size() - 1];
	if(suffix == 'K' || suffix == 'k')
		inBytes = 1024;
	else if(suffix == 'M' || suffix == 'm')
		inBytes = 1024 * 1024;
	else if(suffix == 'G' || suffix == 'g')
		inBytes = 1024 * 1024 * 1024;

	// remove o sufixo e converte p int	
	std::string withoutSuffix = sizeStr.substr(0, sizeStr.size() -1);
	size_t inKB = std::atoi(withoutSuffix.c_str()) * inBytes;
	
	return inKB;
}


bool ConfigParser::containsInvalidCaractersForMaxBodySize(const std::string& str) {
	char a = str[str.length() - 1];
	if(a != 'K' && a != 'M' && a != 'G' && a != 'm' && a != 'k' && a != 'g')
		return true;
    for(size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        // Verifica se o caractere não é um número (0-9) ou não é um dos caracteres permitidos (K, M, G).
        if(!isdigit(c) && c != 'K' && c != 'M' && c != 'G' && c != 'k' && c != 'm' && c != 'k')
            return true; // Encontrou um caractere inválido.
		if(i != (str.length() - 1) && !isdigit(c))
			return true;
    }
    return false; // Não foram encontrados caracteres inválidos.
}

void ConfigParser::processClientMaxBodySize(std::string &line)
{
	if(line == "client_max_body_size")
		throw ErrorException("Syntax Error: The Directive Client_Max_Body_Size needs a value.");
    if(_hasDirMaxBodySize == true)
        throw ErrorException("Configuration Error: The Directive Client_Max_Body_Size has been duplicated.");
    std::string directive = "client_max_body_size";
    std::size_t pos = line.find(directive);
    if(pos != std::string::npos)
    {
		std::cout << YELLOW << "Client_Max_Body_Size no Config File: " << line << END << std::endl;
		std::vector<std::string> maxBodyLine;
		std::istringstream iss(line);
		std::string value;
		while(iss >> value)
			maxBodyLine.push_back(value);
		if(maxBodyLine.size() != 2)
			throw ErrorException("Syntax Error: Client_Max_Body_Size directive must have ONE VALUE.");
		if(maxBodyLine[0] != "client_max_body_size")
			throw ErrorException("Syntax Error: Client_Max_Body_Size directive must COME FIRST IN LINE.");
		if(containsInvalidCaractersForMaxBodySize(maxBodyLine[1]))
			throw ErrorException("Syntax Error: Client_Max_Body_Size value must contain only numbers finalized by K/M/G/k/m/g caracters.");
		std::cout << YELLOW << "client_max_body_size: " << maxBodyLine[1] << END << std::endl;
		if(maxBodyLine[1][maxBodyLine[1].size() - 1] != 'K' && maxBodyLine[1][maxBodyLine[1].size() - 1] != 'k')
			_maxBodySize = convertToKB(maxBodyLine[1]);
		else
			_maxBodySize = atoi(maxBodyLine[1].c_str());
		std::cout << BLUE << "Client_Max_Body_Size em KB: " << _maxBodySize << END << std::endl;
    }
    _hasDirMaxBodySize = true;
}

void ConfigParser::processAutoIndex(std::string &line)
{
	if(_hasAutoIndex)
		throw ErrorException("Configuration Error: AutoIndex directive duplicated.");
	std::vector<std::string> autoIndexLine;
	std::istringstream iss(line);
	std::string value;
	while(iss >> value)
		autoIndexLine.push_back(value);
	if(autoIndexLine.size() != 2)
		throw ErrorException("Syntax Error: AutoIndex directive must have ONE VALUE.");
	if(autoIndexLine[0] != "auto_index")
		throw ErrorException("Syntax Error: AutoIndex directive.");
	if(autoIndexLine[1] == "on")
		_autoIndexOn = true;
	else if(autoIndexLine[1] == "off")
		_autoIndexOn = false;
	else
		throw ErrorException("Syntax Error: AutoIndex directive has wrong value. Value must be only ON | OFF");
	_hasAutoIndex = true;
}

void ConfigParser::processReturn(std::string &line)
{
    std::string directive = "return";
    std::size_t pos = line.find(directive);
    if(_hasDirReturn == true)
        throw ErrorException("Error: The Directive Return has been duplicated.");
    if(pos != std::string::npos)
    {
        pos = line.find_first_of("0123456789", pos + directive.size());
        if(pos != std::string::npos)
        {
            std::size_t status_pos = pos;
            std::size_t end_status_pos = line.find_first_not_of("0123456789", pos);
            if(end_status_pos != std::string::npos)
            {
                std::string status_str = line.substr(status_pos, end_status_pos - status_pos);
                int status_code = atoi(status_str.c_str());
                std::size_t message_pos = line.find_first_of('"', end_status_pos);
                if(message_pos != std::string::npos)
                {
                    std::size_t end_message_pos = line.find('"', message_pos + 1);
                    if(end_message_pos != std::string::npos)
                    {
                        std::string message = line.substr(message_pos + 1, end_message_pos - message_pos - 1);
                        std::cout << "return status code: " << status_code << ", message: " << message << std::endl;
                    }
                }
            }
        }
    }
    _hasDirReturn = true;
}

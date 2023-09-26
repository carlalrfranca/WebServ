/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 21:24:02 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/26 18:33:04 by cleticia         ###   ########.fr       */
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
			std::cout << "Essa pagina de erro nao existe." << std::endl;
		else
			std::cout << "Essa pagina de erro existe." << std::endl;
	_errorPage["503"] = "./web/error/Error503.html";
	_errorPage["400"] = "./web/error/Error400.html";
	_errorPage["403"] = "./web/error/Error403.html";
	_errorPage["405"] = "./web/error/Error405.html";
	_errorPage["505"] = "./web/error/Error505.html";
	_errorPage["504"] = "./web/error/Error504.html";
	_errorPage["500"] = "./web/error/Error500.html";
}

ConfigParser::~ConfigParser(){}

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

	//
	// tinha algo aqui dando problema antes.... ver onde essas var comentadas estao sendo usadas e avaliar
    // _rules = "";    // location
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

	// resetar as paginas de erro padrão
	_errorPage["503"] = "./web/error/Error503.html";
	_errorPage["400"] = "./web/error/Error400.html";
	_errorPage["403"] = "./web/error/Error403.html";
	_errorPage["405"] = "./web/error/Error405.html";
	_errorPage["505"] = "./web/error/Error505.html";
	_errorPage["500"] = "./web/error/Error500.html";
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

std::map<std::string, std::string> ConfigParser::getErrorPage(void)const
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

const std::vector<std::string>& ConfigParser::getMethods()const
{
    return _methods;
}

bool ConfigParser::getAutoIndex()const
{
	return _autoIndexOn;
}

// foi para utils
//bool ConfigParser::contemApenasLetras(const std::string& str)
//{
//    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) 
//	{
//        if (!std::isalpha(*it))
//            return false;
//    }
//    return true;
//}

// bool ConfigParser::contemApenasNumeros(const std::string& str)
// {
//     for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
// 	{
//         if (!std::isdigit(*it))
//             return false;
//     }
//     return true;
// }

/*
void ConfigParser::processListen(std::string &line)
{
	size_t	posInit;
	size_t	_semicolonIndex;

	// esse if impede a duplicação (tirar depois?)
    if(_hasDirListen == true)
        throw ErrorException("Error: The Directive Listen has been duplicated.");
	// tem que extrair o ;
	if (line[line.length() - 1] == ';')
		line = line.substr(0, line.length() - 1);
	// vamos splitar essa linha
	std::istringstream listen_values(line);
	std::string palavra;
    std::vector<std::string> palavras;

	while (listen_values >> palavra)
       	palavras.push_back(palavra);
	if (palavras.size() != 2)
		throw ErrorException("Syntax Error: Only ONE value by listen directive.");
	if (palavras[0] != "listen")
		throw ErrorException("Syntax Error: Format of line: [directive] [value] ...");
	posInit = palavras[1].find("https");
	if (posInit != std::string::npos)
		throw ErrorException("Configuration Error: Can't configure https protocol.");
	posInit = palavras[1].find("://");
	if (posInit != std::string::npos)
	{
		posInit += 3;
		palavras[1] = palavras[1].substr(posInit);
		std::cout << "Valor da listen sem '://' >> " << palavras[1] << std::endl;
	}
	// agora vemos se tem ':'
	posInit = palavras[1].find(":");
	if (posInit != std::string::npos)
	{
		// quer dizer que é combo ip + porta
		_ipAddress = palavras[1].substr(0, posInit);
		_portNumber = palavras[1].substr(posInit + 1);
		// **** tem que verificar que a porta SÃO APENAS NUMEROS
		// **** e verificar o FORMATO DO IPADDRESS
		if (!_utils.containsOnlyNumbers(_portNumber)) //ontemApenasNumeros
			throw ErrorException("Syntax Error: Port must be ONLY numbers.");
		// std::cout << "Combo IP + Porta: " << _ipAddress << " | " << _portNumber << std::endl;
	}
	else
	{
		// quer dizer que ou é só ipAddress ou é só porta
		// se for só ipAddress -> não aceitamos -> pode ter apenas porta, mas não apenas o ipAddress..
		if (_utils.containsOnlyNumbers(palavras[1])) //ontemApenasNumeros
		{
			_portNumber = palavras[1];
			_ipAddress = "localhost";
			// std::cout << "Combo IP + Porta: " << _ipAddress << " | " << _portNumber << std::endl;
		}
		else
			throw ErrorException("Configuration Error: You must configure a PORT NUMBER!");
	}
	_hasDirListen = true;
}
*/


void ConfigParser::processListen(const std::vector<std::string> &lines)
{
	for (std::vector<std::string>::const_iterator it = lines.begin(); it != lines.end(); ++it)
	{
		size_t	posInit;
		size_t	_semicolonIndex;
	
		// esse if impede a duplicação (tirar depois?)
        if(_hasDirListen == true)
            throw ErrorException("Error: The Directive Listen has been duplicated.");
            
		// tem que extrair o ;
		std::string listenLine = *it;
		if ( listenLine[listenLine.length() - 1] == ';')
			listenLine =  listenLine.substr(0,  listenLine.length() - 1);
		// vamos splitar essa linha
		std::istringstream listen_values(listenLine);
		std::string palavra;
        std::vector<std::string> palavras;
	
		while (listen_values >> palavra)
           	palavras.push_back(palavra);
		if (palavras.size() != 2)
			throw ErrorException("Syntax Error: Only ONE value by listen directive.");
		if (palavras[0] != "listen")
			throw ErrorException("Syntax Error: Format of line: [directive] [value] ...");
		posInit = palavras[1].find("https");
		if (posInit != std::string::npos)
			throw ErrorException("Configuration Error: Can't configure https protocol.");
		posInit = palavras[1].find("://");
		if (posInit != std::string::npos)
		{
			posInit += 3;
			palavras[1] = palavras[1].substr(posInit);
			std::cout << "Valor da listen sem '://' >> " << palavras[1] << std::endl;
		}
		// agora vemos se tem ':'
		posInit = palavras[1].find(":");
		if (posInit != std::string::npos)
		{
			// quer dizer que é combo ip + porta
			_ipAddress = palavras[1].substr(0, posInit);
			_portNumber = palavras[1].substr(posInit + 1);
			// **** tem que verificar que a porta SÃO APENAS NUMEROS
			// **** e verificar o FORMATO DO IPADDRESS
			if (!_utils.containsOnlyNumbers(_portNumber)) //ontemApenasNumeros
				throw ErrorException("Syntax Error: Port must be ONLY numbers.");
			// std::cout << "Combo IP + Porta: " << _ipAddress << " | " << _portNumber << std::endl;
		}
		else
		{
			// quer dizer que ou é só ipAddress ou é só porta
			// se for só ipAddress -> não aceitamos -> pode ter apenas porta, mas não apenas o ipAddress..
			if (_utils.containsOnlyNumbers(palavras[1])) //ontemApenasNumeros
			{
				_portNumber = palavras[1];
				_ipAddress = "localhost";
				// std::cout << "Combo IP + Porta: " << _ipAddress << " | " << _portNumber << std::endl;
			}
			else
				throw ErrorException("Configuration Error: You must configure a PORT NUMBER!");
		}
		_hasDirListen = true;
	}
}

void ConfigParser::processServerName(std::string &line)
{
    if(_hasDirServerName == true)
        throw ErrorException("Error: The Directive Server_Name has been duplicated.");
    if(line.find("server_name") != std::string::npos)
	{// tem que extrair o ;
		if (line[line.length() - 1] == ';')
			line = line.substr(0, line.length() - 1);
		// vamos splitar essa linha
		std::istringstream server_name_values(line);
		std::string palavra;
    	std::vector<std::string> palavras;
	
		while (server_name_values >> palavra)
		{
			if (palavra.find_first_not_of("abcdefghijklmnopqrstuvwxyz._") != std::string::npos)
				throw ErrorException("Syntax Error: Server_name can only be written with [letters and dot]. You CANNOT use regex or wildcards, only LITERAL names.");
        	palavras.push_back(palavra);
    	}
		// o problema é que no vetor "palavras", ele gravou o nome da diretiva (server_name), temos que extrai-la do vetor
		palavras.erase(palavras.begin());
		for (size_t i = 0; i < palavras.size(); ++i)
        	_domains.push_back(palavras[i]);
    }
    _hasDirServerName = true;
}

bool ConfigParser::processRoot(std::string &line){
	std::istringstream iss(line);
    std::vector<std::string> partes;

    std::string palavra;
	if (_root.empty() == false)
		throw ErrorException("Error: The Directive Root has been duplicated.");
    while (iss >> palavra)
        partes.push_back(palavra);
	if (partes[0] != "root" || partes.size() != 2)
		throw ErrorException("Syntax Error: Directive Root");
	_root = partes[1];
	struct stat info;
    if (stat(_root.c_str(), &info) != 0)
        throw ErrorException("Error: The path apointed by 'root' doesn't exist.");
	if ((info.st_mode & S_IFDIR) == 0)
		throw ErrorException("Error: The path apointed by 'root' ISN'T A DIRECTORY.");
	// std::cout << "Root: " << _root << std::endl;
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
    for (itAllowedDirective = allowedDirectives.begin(); itAllowedDirective != allowedDirectives.end(); ++itAllowedDirective) {
        if (*itAllowedDirective == directive)
            break;
    }
	if (itAllowedDirective == allowedDirectives.end())
		throw ErrorException("Configuration Error: Prohibited directive in location block");
}

void ConfigParser::storeCurrentLocationDirectives(std::string &directiveLine){
	std::string currentLocation = _currentLocationPathOnMap;

	std::map<std::string, LocationDirective>::iterator it = _locations.find(currentLocation);
	if (it != _locations.end())
	{
		// acessa o método atraves do iterador second.método()
		// mas antes tem que extrair o nome da diretiva e o valor (splitar e colocar num vetor?)
		// tambem contar quantos itens resultam do split porque isso quer dizer que a diretiva tem mais de um valor...
		// primeiro, splitamos a linha da diretiva para poder separar o nome da diretiva de seus valores
		std::istringstream iss(directiveLine);
		std::vector<std::string> splittedLine;
		std::string singleValue;
		while (iss >> singleValue)
			splittedLine.push_back(singleValue);
		
		if (splittedLine.size() < 2)
			throw ErrorException("Syntax Error: Missing values to directive in location block");
		hasprohibitedDirectiveInLocation(splittedLine[0]);
		std::map<std::string, std::vector<std::string> >::iterator directiveIt = it->second.getDirectives().find(splittedLine[0]);
		if (directiveIt != it->second.getDirectives().end())
			throw ErrorException("Syntax Error: Duplicated directive in location block"); 
		// com os argumentos splitados e num vetor, vamos inserir a key no objeto
		it->second.addDirective(splittedLine[0], splittedLine[1]);
		// diretiva (se há mais itens)
		if (splittedLine.size() > 2)
		{
			size_t totalValues = splittedLine.size();
			size_t currentIndex = 2;
			while (currentIndex < totalValues)
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
 
    if (!fileToParse.is_open())
        throw ErrorException("File is not accessible");
    fileToParse.seekg(0, std::ios::end);
    std::streampos fileSize = fileToParse.tellg();
    fileToParse.seekg(0, std::ios::beg);
    if(fileSize <= 0)
        throw ErrorException("File is empty");
}

/*
void ConfigParser::hasMandatoryParameters(){

    if(!getHasDirListen()){
        throw ErrorException("");
    }
    if(!getHasDirServerName()){

    }


    
    listen 
    server_name 
    se nao tiver nada no error pages fazemos atribuição e 
    validar que as paginas estao presentes
    root [ja tem um default]

}*/

void ConfigParser::processLocation(std::string &line)
{
    std::vector<std::string> filesForPath1;

    if (line.find("location")!= std::string::npos)
    {
		if (line[line.size() - 1] != '{')
			throw ErrorException("Syntax Error: Missing '{' to open LOCATION BLOCK");
        std::istringstream iss(line);
        std::vector<std::string> parts;
    
        std::string part;
        while (iss >> part)
            parts.push_back(part);
        _currentLocationPathOnMap = parts[1];
		std::map<std::string, std::vector<std::string> >::iterator it = _locationsMap.find(_currentLocationPathOnMap);
		if (it != _locationsMap.end())
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
        while(iss >> token){
            std::cout << "error_page: " << token << std::endl;
            errorCode.push_back(token);   
        }
        if(errorCode.size() != 3 || errorCode[0] != "error_page")
            throw ErrorException("Syntax Error: Found inconsistence in error_page directive");
		std::string statusCode;
        statusCode = errorCode[1];
        for(size_t i = 0; i < statusCode.size(); ++i){
            if(!isdigit(statusCode[i]))
                throw ErrorException("Syntax Error: Found inconsistence in status code apointed by error_page directive");
        }
        std::string pathError;
        pathError = errorCode[2];
		struct stat info;
		if (stat(pathError.c_str(), &info) != 0)
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
    while (iss >> method)
        methods.push_back(method);
    if (methods.size() < 2 || methods[0] != "allow_methods")
		throw ErrorException("Syntax Error: The Directive Allow Methods isn't properly configured.");
	_methods = methods;
	_methods.erase(_methods.begin());
	for (size_t i = 0; i < _methods.size(); ++i)
	{
		if (_methods[i] != "GET" && _methods[i] != "POST" && _methods[i] != "DELETE")
			throw ErrorException("Configuration Error: Only GET | POST | DELETE methods are accepted.");
	}
    _hasDirAllowMethods = true;
}

void ConfigParser::processClientMaxBodySize(std::string &line)
{
	if (line == "client_max_body_size")
		throw ErrorException("Syntax Error: The Directive Client Max Body Size need a value.");
    if(_hasDirMaxBodySize == true)
        throw ErrorException("Configuration Error: The Directive Client Max Body Size has been duplicated.");
    std::string directive = "client_max_body_size";
    std::size_t pos = line.find(directive);
    if (pos != std::string::npos)
    {
        pos = line.find_first_of("0123456789", pos + directive.size());
        if (pos != std::string::npos) {
            std::size_t end_pos = line.find_first_not_of("0123456789KMG", pos);
            if (end_pos != std::string::npos) {
                std::string size_str = line.substr(pos, end_pos - pos);
                std::cout << "client_max_body_size: " << size_str << std::endl;
				_maxBodySize = atoi(size_str.c_str());
            }
        }
    }
    _hasDirMaxBodySize = true;
}

void ConfigParser::processAutoIndex(std::string &line)
{
	if (_hasAutoIndex)
		throw ErrorException("Configuration Error: AutoIndex directive duplicated.");
	std::vector<std::string> autoIndexLine;
    std::istringstream iss(line);
    std::string value;
    while (iss >> value)
        autoIndexLine.push_back(value);
	if (autoIndexLine.size() != 2)
		throw ErrorException("Syntax Error: AutoIndex directive must have ONE VALUE.");
	if (autoIndexLine[0] != "auto_index")
		throw ErrorException("Syntax Error: AutoIndex directive.");
	if (autoIndexLine[1] == "on")
		_autoIndexOn = true;
	else if (autoIndexLine[1] == "off")
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
    if (pos != std::string::npos)
    {
        pos = line.find_first_of("0123456789", pos + directive.size());
        if (pos != std::string::npos)
        {
            std::size_t status_pos = pos;
            std::size_t end_status_pos = line.find_first_not_of("0123456789", pos);
            if (end_status_pos != std::string::npos)
            {
                std::string status_str = line.substr(status_pos, end_status_pos - status_pos);
                int status_code = atoi(status_str.c_str());
                std::size_t message_pos = line.find_first_of('"', end_status_pos);
                if (message_pos != std::string::npos)
                {
                    std::size_t end_message_pos = line.find('"', message_pos + 1);
                    if (end_message_pos != std::string::npos)
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




// #include "../inc/HeadersLibs.hpp"
// #include "../inc/ConfigParser.hpp"
// #include "../inc/Utils.hpp"

// ConfigParser::ConfigParser()
// {
//     // _line = ""; talvez, mas acho que nao precisa mesmo
//     // _domains = ""; server_name
//     // _rules = ""; location
//     _directive = 0;
//     _portNumber = "";
//     _ipAddress = "";
//     _path = "";
//     _root = "";
//     _hasRoot = true;
//     _hasDirListen = false;
//     _hasDirServerName = false;
//     _hasDirIndex = false;
//     _hasDirAllowMethods = false;
//     _hasDirMaxBodySize = false;
//     _hasDirReturn = false;
// 	_indexFile = "";
// 	_maxBodySize = 1024;

// 	// configurando por padrão os paths das paginas de erro (se o arquivo personalizar alguma, 
// 	// vai sobrescrever na hora em que estiver processando a diretiva de error_page).
	
// 	_errorPage["404"] = "./web/error/Error404.html";
// 	struct stat info;
// 		if (stat(_errorPage["404"].c_str(), &info) != 0)
// 			std::cout << "Essa pagina de erro nao existe." << std::endl;
// 		else
// 			std::cout << "Essa pagina de erro existe." << std::endl;
// 	_errorPage["503"] = "./web/error/Error503.html";
// 	_errorPage["400"] = "./web/error/Error400.html";
// 	_errorPage["403"] = "./web/error/Error403.html";
// 	_errorPage["405"] = "./web/error/Error405.html";
// 	_errorPage["505"] = "./web/error/Error505.html";
// 	_errorPage["500"] = "./web/error/Error500.html";

// }

// ConfigParser::~ConfigParser(){}

// void ConfigParser::resetConfig()
// {
// 	_hasDirAllowMethods = false;
// 	_hasDirIndex = false;
// 	_hasDirListen = false;
// 	_hasDirLocation = false;
// 	_hasDirMaxBodySize = false;
// 	_hasDirReturn = false;
// 	_hasDirServerName = false;
// 	_maxBodySize = 1024;

// 	// tinha algo aqui dando problema antes.... ver onde essas var comentadas estao sendo usadas e avaliar
//     // _rules = "";    // location
//     _directive = 0;
//     // _portNumber = "";
//     // _ipAddress = "";
//     // _httpAddress = "";
//     _path = "";
//     _root = "";
//     _hasRoot = true;
// 	_indexFile = "";
// 	/*
// 		No método que faz o reset do _configParser
// 		(só ver qual método tem "reset" no nome, está sendo chamado na configSocket tambem),
// 		inserir a linha que limpa o vetor de ipAddress e port
// 	*/
// 	_ipAddress.clear();
// 	_portNumber.clear();

// 	// _indexFiles.clear();
// 	_locationsMap.clear();
// 	_locations.clear();

// 	// resetar as paginas de erro padrão
// 	_errorPage["503"] = "./web/error/Error503.html";
// 	_errorPage["400"] = "./web/error/Error400.html";
// 	_errorPage["403"] = "./web/error/Error403.html";
// 	_errorPage["405"] = "./web/error/Error405.html";
// 	_errorPage["505"] = "./web/error/Error505.html";
// 	_errorPage["500"] = "./web/error/Error500.html";
// }

// std::map<std::string, LocationDirective> ConfigParser::getLocations() const
// {
// 	return _locations;
// }

// void ConfigParser::setLocations(std::map<std::string, LocationDirective>& locations)
// {
// 	_locations = locations;
// }

// void ConfigParser::setPort(int portNumber)
// {
//     this->_portNumber = portNumber;
// }

// void ConfigParser::setAddress(std::string ipAddress)
// {
//     this->_ipAddress = ipAddress;
// }

// const std::string& ConfigParser::getPort()const
// {
//     return _portNumber;
// }

// const std::string& ConfigParser::getAddress()const
// {
//     return _ipAddress;
// }

// void ConfigParser::setRoot(std::string root)
// {
//     _root = root;
// }

// const std::string& ConfigParser::getRoot()const
// {
//     return _root;
// }

// std::map<std::string, std::string> ConfigParser::getErrorPage(void)const
// {
//     return _errorPage;
// }

// const std::string& ConfigParser::getIndexFile()const
// {
// 	return _indexFile;
// }

// void ConfigParser::setIndexFile(std::string index)
// {
// 	_indexFile = index;
// }

// const std::vector<std::string>& ConfigParser::getMethods()const
// {
//     return _methods;
// }

// bool ConfigParser::getAutoIndex()const
// {
// 	return _autoIndexOn;
// }

// // foi para utils
// //bool ConfigParser::contemApenasLetras(const std::string& str)
// //{
// //    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) 
// //	{
// //        if (!std::isalpha(*it))
// //            return false;
// //    }
// //    return true;
// //}

// // bool ConfigParser::contemApenasNumeros(const std::string& str)
// // {
// //     for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
// // 	{
// //         if (!std::isdigit(*it))
// //             return false;
// //     }
// //     return true;
// // }


// /*
// 	depois, criar um método que irá verificar o vetor de ports e, 
// 	caso haja alguma porta duplicada, dará exceção (não pode ter porta duplicada). 
// 	Esse método será chamado na primeira parte da configSocket.
// */

// void ConfigParser::checkDuplicatePorts()
// {
// 	std::vector<std::string> seenPorts;
// 	for(size_t i = 0; i < _portNumbers.size(); ++i)
// 	{
// 		for(size_t j = 0; j < _portNumbers.size(); ++j)
// 		{
// 			if(_portNumbers[i] == _portNumbers[j])
// 				throw ErrorException("Configuration Error: Duplicate port number found.");
// 		}
// 	}
// }

// void ConfigParser::processListen(std::string &line)
// {
// 	size_t	posInit;
// 	size_t	_semicolonIndex;

//     if(_hasDirListen == true)
//         throw ErrorException("Error: The Directive Listen has been duplicated.");
// 	if (line[line.length() - 1] == ';')
// 		line = line.substr(0, line.length() - 1);

// 	std::istringstream listen_values(line);
// 	std::string token;
//     std::vector<std::string> tokens;

// 	while (listen_values >> token)
// 	   	tokens.push_back(token);
// 	if (tokens.size() != 2)
// 		throw ErrorException("Syntax Error: Only ONE value by listen directive.");
// 	if (tokens[0] != "listen")
// 		throw ErrorException("Syntax Error: Format of line: [directive] [value] ...");
// 	posInit = tokens[1].find("https");
// 	if (posInit != std::string::npos)
// 		throw ErrorException("Configuration Error: Can't configure https protocol.");
// 	posInit = tokens[1].find("://");
// 	if (posInit != std::string::npos)
// 	{
// 		posInit += 3;
// 		tokens[1] = tokens[1].substr(posInit);
// 		std::cout << "Valor da listen sem '://' >> " << tokens[1] << std::endl;
// 	}
// 	posInit = tokens[1].find(":");
// 	if (posInit != std::string::npos)
// 	{
// 		_ipAddress = tokens[1].substr(0, posInit);
// 		_portNumber = tokens[1].substr(posInit + 1);
// 		std::cout << YELLOW << "IP/PORTA: " << _ipAddress << " / " << _portNumber << std::endl;
// 		// **** tem que verificar que a porta SÃO APENAS NUMEROS
// 		// **** e verificar o FORMATO DO IPADDRESS
// 		if (!_utils.containsOnlyNumbers(_portNumber)) //ontemApenasNumeros
// 			throw ErrorException("Syntax Error: Port must be ONLY numbers.");
// 		// std::cout << "Combo IP + Porta: " << _ipAddress << " | " << _portNumber << std::endl;
		
// 		_portNumbers.push_back(_portNumber);
// 		_ipAddresses.push_back(_ipAddress);
// 	} else {
// 		// quer dizer que ou é só ipAddress ou é só porta
// 		// se for só ipAddress -> não aceitamos -> pode ter apenas porta, mas não apenas o ipAddress..
// 		if (_utils.containsOnlyNumbers(tokens[1])) //ontemApenasNumeros
// 		{
// 			_portNumber = tokens[1];
// 			_ipAddress = "localhost";
// 			// std::cout << "Combo IP + Porta: " << _ipAddress << " | " << _portNumber << std::endl;
		
// 			std::cout << YELLOW << "IP/PORTA: " << _ipAddress << " / " << _portNumber << END << std::endl;
// 			_portNumbers.push_back(_portNumber);
// 			_ipAddresses.push_back(_ipAddress);

// 		}
// 		else
// 			throw ErrorException("Configuration Error: You must configure a PORT NUMBER!");
// 	}
// 	_hasDirListen = true;
// }

// void ConfigParser::processServerName(std::string &line)
// {
//     if(_hasDirServerName == true)
//         throw ErrorException("Error: The Directive Server_Name has been duplicated.");
//     if(line.find("server_name") != std::string::npos)
//     {
//         if (line[line.length() - 1] == ';')
//             line = line.substr(0, line.length() - 1);

//         std::istringstream server_name_values(line);
//         std::string token;
//         std::vector<std::string> tokens;
//         while (server_name_values >> token)
//         {
//             if (token.find_first_not_of("abcdefghijklmnopqrstuvwxyz._") != std::string::npos)
// 		throw ErrorException("Syntax Error: Server_name can only be written with [letters and dot]. You CANNOT use regex or wildcards, only LITERAL names.");
//         	tokens.push_back(token);
//         }
//         // o problema é que no vetor "tokens", ele gravou o nome da diretiva (server_name), temos que extrai-la do vetor
//         tokens.erase(tokens.begin());
//         for (size_t i = 0; i < tokens.size(); ++i)
//             _domains.push_back(tokens[i]);
//     }
//     _hasDirServerName = true;
// }

// bool ConfigParser::processRoot(std::string &line){
// 	std::istringstream iss(line);
//     std::vector<std::string> partes;

//     std::string token;
// 	if (_root.empty() == false)
// 		throw ErrorException("Error: The Directive Root has been duplicated.");
//     while (iss >> token)
//         partes.push_back(token);
// 	if (partes[0] != "root" || partes.size() != 2)
// 		throw ErrorException("Syntax Error: Directive Root");
// 	_root = partes[1];
// 	struct stat info;
//     if (stat(_root.c_str(), &info) != 0)
//         throw ErrorException("Error: The path apointed by 'root' doesn't exist.");
// 	if ((info.st_mode & S_IFDIR) == 0)
// 		throw ErrorException("Error: The path apointed by 'root' ISN'T A DIRECTORY.");
// 	// std::cout << "Root: " << _root << std::endl;
//     return _hasRoot;
// }

// void ConfigParser::storeCurrentLocationDirectives(std::string &directiveLine){
// 	std::string currentLocation = _currentLocationPathOnMap;

// 	std::map<std::string, LocationDirective>::iterator it = _locations.find(currentLocation);
// 	if (it != _locations.end())
// 	{
// 		// acessa o método atraves do iterador second.método()
// 		// mas antes tem que extrair o nome da diretiva e o valor (splitar e colocar num vetor?)
// 		// tambem contar quantos itens resultam do split porque isso quer dizer que a diretiva tem mais de um valor...
// 		// primeiro, splitamos a linha da diretiva para poder separar o nome da diretiva de seus valores
// 		std::istringstream iss(directiveLine);
// 		std::vector<std::string> splittedLine;
// 		std::string singleValue;
// 		while (iss >> singleValue)
// 			splittedLine.push_back(singleValue);	
// 		if (splittedLine.size() < 2)
// 			throw ErrorException("Syntax Error: Missing values to directive in location block");
// 		// nao pode ter nem "listen" nem "server_name" dentro de um bloco location...
// 		if (splittedLine[0].find("listen") != std::string::npos || splittedLine[0].find("server_name") != std::string::npos
// 			|| splittedLine[0].find("location") != std::string::npos)
// 			throw ErrorException("Configuration Error: CANNOT have 'listen', 'server_name' or an internal 'location' in location block");
// 		// if (splittedLine[0] != "index" || splittedLine[0] != "auto_index" || splittedLine[0] != "root" || splittedLine[0] != "error_page" || splittedLine[0] != "return")
// 			// throw ErrorException("Configuration Error: Prohibited directives in location block."); --> tem que distinguir locations normais da de cgi...
		
// 		// precisa verificar se essa diretiva JA EXISTE no location tambem...
// 		std::map<std::string, std::vector<std::string> >::iterator directiveIt = it->second.getDirectives().find(splittedLine[0]);
// 		if (directiveIt != it->second.getDirectives().end())
// 			throw ErrorException("Syntax Error: Duplicated directive in location block"); 
// 		// ------------------------------
// 		// com os argumentos splitados e num vetor, vamos inserir a key no objeto
// 		it->second.addDirective(splittedLine[0], splittedLine[1]);
// 		// diretiva (se há mais itens)
// 		if (splittedLine.size() > 2)
// 		{
// 			size_t totalValues = splittedLine.size();
// 			size_t currentIndex = 2;
// 			while (currentIndex < totalValues)
// 			{
// 				it->second.addDirective(splittedLine[0], splittedLine[currentIndex]);
// 				std::cout << currentIndex << std::endl;
// 				currentIndex++;
// 			}
// 		}
// 	}
// }

// void ConfigParser::validateFile(std::ifstream& fileToParse)
// {
 
//     if (!fileToParse.is_open())
//         throw ErrorException("File is not accessible");
//     fileToParse.seekg(0, std::ios::end);
//     std::streampos fileSize = fileToParse.tellg();
//     fileToParse.seekg(0, std::ios::beg);
//     if(fileSize <= 0)
//         throw ErrorException("File is empty");
// }

// /*
// void ConfigParser::hasMandatoryParameters(){

//     if(!getHasDirListen()){
//         throw ErrorException("");
//     }
//     if(!getHasDirServerName()){

//     }


    
//     listen 
//     server_name 
//     se nao tiver nada no error pages fazemos atribuição e 
//     validar que as paginas estao presentes
//     root [ja tem um default]

// }*/

// void ConfigParser::processLocation(std::string &line)
// {
//     std::vector<std::string> filesForPath1;

//     if (line.find("location")!= std::string::npos)
//     {
// 		if (line[line.size() - 1] != '{')
// 			throw ErrorException("Syntax Error: Missing '{' to open LOCATION BLOCK"); 
//         // verificar o tipo de location ("/", "/[directorio], etc")
//         // pra isso, teremos que dividir a line em partes pra ver o que é apontado pela location
// 		// std::cout << "Line in Location: " << line << std::endl;
//         std::istringstream iss(line);
//         std::vector<std::string> parts;
    
//         std::string part;
//         while (iss >> part)
//             parts.push_back(part);
//         _currentLocationPathOnMap = parts[1];
// 		// antes de passar, verifica se em _locationsMap ja nao tem um location desse igual do current...
// 		std::map<std::string, std::vector<std::string> >::iterator it = _locationsMap.find(_currentLocationPathOnMap);
// 		if (it != _locationsMap.end())
// 			throw ErrorException("Syntax Error: DUPLICATED location");
//         _locationsMap[_currentLocationPathOnMap];
// 		_locations[_currentLocationPathOnMap];
//     }
//     else //acho que esse "else" tambem pode ser identificando os caminhos do location (hardcoded) e, dependendo de qual for, encaminha
//     {	//pra um metodo correspondente (entao se for '/' é pra um, se for 'cgi-bin' é outro, se for regex é outro, etc)
//         std::cout << std::endl;
//         // std::cout << "Current location: " << _currentLocationPathOnMap << std::endl;
//         storeCurrentLocationDirectives(line);
//     }
// }

// void ConfigParser::processIndex(std::string &line)
// {
// 	std::vector<std::string>	_indexFiles;

//     if(_hasDirIndex == true)
//         throw ErrorException("Error: The Directive Index has been duplicated.");
//     if(line.find("index") != std::string::npos)
//     {
//         std::istringstream indicesStream(line);
//         std::string index;
//         while(indicesStream >> index)
//             _indexFiles.push_back(index);
// 		if (_indexFiles.size() != 2)
// 			throw ErrorException("Configuration Error: You must point ONE index file.");
// 		if (_indexFiles[0] != "index")
// 			throw ErrorException("Syntax Error: Correct Format: [directive] [value]");
// 		_indexFile = _indexFiles[1];
//     }
//     _hasDirIndex = true;
// }

// void ConfigParser::processErrorPage(std::string &line)
// {
//     if(line.find("error_page") != std::string::npos)
//     {
//         std::istringstream iss(line);
//         std::vector<std::string> errorCode;
//         std::string token;
//         while(iss >> token){
//             std::cout << "error_page: " << token << std::endl;
//             errorCode.push_back(token);   
//         }
//         if(errorCode.size() != 3 || errorCode[0] != "error_page")
//             throw ErrorException("Syntax Error: Found inconsistence in error_page directive");
// 		std::string statusCode;
//         statusCode = errorCode[1];
//         for(size_t i = 0; i < statusCode.size(); ++i){
//             if(!isdigit(statusCode[i]))
//                 throw ErrorException("Syntax Error: Found inconsistence in status code apointed by error_page directive");
//         }
//         std::string pathError;
//         pathError = errorCode[2];
// 		struct stat info;
// 		if (stat(pathError.c_str(), &info) != 0)
// 			throw ErrorException("Configuration Error: Error page file doesn't exist!");
//         _errorPage[statusCode] = pathError; // Armazena a página de erro no mapa
//     }
// }

// void ConfigParser::processAllowMethods(std::string &line)
// {
//     if(_hasDirAllowMethods == true)
//         throw ErrorException("Error: The Directive Allow Methods has been duplicated.");
//     std::vector<std::string> methods;
//     std::istringstream iss(line);
//     std::string method;
//     while (iss >> method)
//         methods.push_back(method);
//     if (methods.size() < 2 || methods[0] != "allow_methods")
// 		throw ErrorException("Syntax Error: The Directive Allow Methods isn't properly configured.");
// 	_methods = methods;
// 	_methods.erase(_methods.begin());
// 	for (size_t i = 0; i < _methods.size(); ++i)
// 	{
// 		if (_methods[i] != "GET" && _methods[i] != "POST" && _methods[i] != "DELETE")
// 			throw ErrorException("Configuration Error: Only GET | POST | DELETE methods are accepted.");
// 	}
//     _hasDirAllowMethods = true;
// }

// void ConfigParser::processClientMaxBodySize(std::string &line)
// {
// 	if (line == "client_max_body_size")
// 		throw ErrorException("Syntax Error: The Directive Client Max Body Size need a value.");
//     if(_hasDirMaxBodySize == true)
//         throw ErrorException("Configuration Error: The Directive Client Max Body Size has been duplicated.");
//     std::string directive = "client_max_body_size";
//     std::size_t pos = line.find(directive);
//     if (pos != std::string::npos)
//     {
//         pos = line.find_first_of("0123456789", pos + directive.size());
//         if (pos != std::string::npos) {
//             std::size_t end_pos = line.find_first_not_of("0123456789KMG", pos);
//             if (end_pos != std::string::npos) {
//                 std::string size_str = line.substr(pos, end_pos - pos);
//                 std::cout << "client_max_body_size: " << size_str << std::endl;
// 				_maxBodySize = atoi(size_str.c_str());
//             }
//         }
//     }
//     _hasDirMaxBodySize = true;
// }

// void ConfigParser::processAutoIndex(std::string &line)
// {
// 	if (_hasAutoIndex)
// 		throw ErrorException("Configuration Error: AutoIndex directive duplicated.");
		
// 	std::vector<std::string> autoIndexLine;
//     std::istringstream iss(line);
//     std::string value;
//     while (iss >> value)
//         autoIndexLine.push_back(value);
// 	if (autoIndexLine.size() != 2)
// 		throw ErrorException("Syntax Error: AutoIndex directive must have ONE VALUE.");
// 	if (autoIndexLine[0] != "auto_index")
// 		throw ErrorException("Syntax Error: AutoIndex directive.");
// 	if (autoIndexLine[1] == "on")
// 		_autoIndexOn = true;
// 	else if (autoIndexLine[1] == "off")
// 		_autoIndexOn = false;
// 	else
// 		throw ErrorException("Syntax Error: AutoIndex directive has wrong value. Value must be only ON | OFF");
// 	_hasAutoIndex = true;
	
// 	//22.09.2023
// 	if(_hasDirIndex && _autoIndexOn)
// 		_autoIndexOn = false;
// }	/*
// 		Ajuste de AutoIndex - inverter a ordem. quando na location tem a diretiva index indicando 
// 		uma pagina padrao e tem um autoindex a location e: quando coloca a barra / primeiro vc ver 
// 		se tem o autoindex se esta on e ele tem a precedencia ai vc ve se tem index e server.
// 	*/

// void ConfigParser::processReturn(std::string &line)
// {
//     std::string directive = "return";
//     std::size_t pos = line.find(directive);
//     if(_hasDirReturn == true)
//         throw ErrorException("Error: The Directive Return has been duplicated.");
//     if (pos != std::string::npos)
//     {
//         pos = line.find_first_of("0123456789", pos + directive.size());
//         if (pos != std::string::npos)
//         {
//             std::size_t status_pos = pos;
//             std::size_t end_status_pos = line.find_first_not_of("0123456789", pos);
//             if (end_status_pos != std::string::npos)
//             {
//                 std::string status_str = line.substr(status_pos, end_status_pos - status_pos);
//                 int status_code = atoi(status_str.c_str());
//                 std::size_t message_pos = line.find_first_of('"', end_status_pos);
//                 if (message_pos != std::string::npos)
//                 {
//                     std::size_t end_message_pos = line.find('"', message_pos + 1);
//                     if (end_message_pos != std::string::npos)
//                     {
//                         std::string message = line.substr(message_pos + 1, end_message_pos - message_pos - 1);
//                         std::cout << "return status code: " << status_code << ", message: " << message << std::endl;
//                     }
//                 }
//             }
//         }
//     }
//     _hasDirReturn = true;
// }

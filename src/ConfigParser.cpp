/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 21:24:02 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/24 18:56:48 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"
#include "../inc/ConfigParser.hpp"

// #include "../inc/ConfigParser.hpp"
// #include <cctype> //Para usar a função std::isspace
// #include <vector>
// #include <map>
// #include <string>
// #include <iostream>
// #include <sstream>

ConfigParser::ConfigParser()
{
    //_line = ""; // talvez, mas acho que nao precisa mesmo
    _domain = ""; // server_name
    //_ssl = "";    //ssl_certificate
    _key = "";      // ssl_certificate
    _rules = "";    // location
    _directive = 0;
    _portNumber = "";
    _ipAddress = "";
    _httpAddress = "";
    _path = "";
    _posInit = 0;
    _posEnd = 0;
    _posSemicolon = 0;
    _semicolonIndex = 0;
    _delimiter = 0;
    _root = "";
    _hasRoot = true;
}
/*

    g++ -std=c++98 -I inc/ src/main.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp src/LocationDirective.cpp -o executavel 
    ./executavel src/config.txt  
*/

ConfigParser::~ConfigParser() {}

void ConfigParser::processListen(std::string &line)
{
    _directive = line.find("listen");
    if(line == "listen"){
        _ipAddress = "0.0.0.0";
        _portNumber = "80";
        std::cout << "\n-----[PRIMEIRO] TESTE DEFAULT-----";
        std::cout << "IP Address: " << _ipAddress << std::endl;
        std::cout << "Port: " << _portNumber << std::endl;
        //std::cout << std::endl;
        return ;
    }
    _semicolonIndex = line.find_first_of(";");
    if(_semicolonIndex != std::string::npos && _semicolonIndex == (line.length() - 1)){
        line = line.substr(0, (line.length() - 1));
    }
    if(_directive != std::string::npos){
        _directive += std::string("listen ").length();
        _posInit = line.find("://", _directive);
        if(_posInit != std::string::npos){
            _ipAddress = line.substr(_directive, (line.length() - (_directive)));
            size_t _delimiter = line.find_last_of(":");
            if(_delimiter != _posInit){
                _portNumber = line.substr(_delimiter + 1, line.length() - (_delimiter + 1));
                size_t portIndex = _ipAddress.find_last_of(":");
                _ipAddress = _ipAddress.substr(0, portIndex);
            }else
                _portNumber = "80";
            std::cout << "\n-----[SEGUNDO] TESTE DE HTTP E HTTPS  COM E SEM PORTA-----" << std::endl;
            std::cout << "IP Address: " << _ipAddress << std::endl;
            std::cout << "Port: " << _portNumber << std::endl;
            //std::cout << std::endl;
        }
        else
        { // sem protocolo http ou https // trata primeiro com :
            _posInit = line.find(":", _directive);
            if (_posInit != std::string::npos){// std::cout << "Posição dos ':' >> " << _posInit << std::endl;
                _ipAddress = line.substr(_directive, _posInit - _directive);
                _portNumber = line.substr(_posInit + 1, line.length());
            }
            else if(line.find(".", _directive) != std::string::npos){// significa que tem ipAddress (port será default)
               _ipAddress = line.substr(_directive);
               _portNumber = "80";   
            }
            else{
                _portNumber = line.substr(_directive);
                _ipAddress = "0.0.0.0";
            }
            std::cout << "\n-----[TERCEIRO] TESTE SEM HTTP E HTTPS -----\n";
            std::cout << "IP: " << _ipAddress << std::endl;
            std::cout << "Porta: " << _portNumber << std::endl;
            //std::cout << std::endl;
        }
        
    }
}     


void ConfigParser::processServerName(std::string &line)
{
    _directive = line.find("server_name");
    if(_directive != std::string::npos){
        _directive += std::string("server_name").length();                               // avança para além da palavra "server_name"
        _posInit = line.find_first_of("*", _directive); // busca para os casos de wildcard
        if(_posInit != std::string::npos){ // se ele encontrou
            _posSemicolon = line.find_first_of(";", _posInit); // estrai do asterisco até antes do ;
            if(_posSemicolon != std::string::npos){
                _domain = line.substr(_posInit, _posSemicolon - (_posInit)); // se quiser extrair sem * _domain = line.substr(_posInit + 1, _posSemicolon - (_posInit + 1));
                std::cout << "Domain: " << _domain << std::endl;     
            }
        }
        else{
            if(line.find_first_of("~", _directive) != std::string::npos){ // busca para os casos de expressao regular
                _posInit = line.find_first_of("~", _directive);
                _posSemicolon = line.find_first_of(";", _posInit); // estrai do asterisco até antes do ;
                if(_posSemicolon != std::string::npos){
                    _domain = line.substr(_posInit, _posSemicolon - (_posInit)); // se quiser extrair sem ~ _domain = line.substr(_posInit + 1, _posSemicolon - (_posInit + 1));
                    std::cout << "Domain: " << _domain << std::endl;             // pra TESTE
                }
            }
            else if(line.find_first_of("/", _directive) != std::string::npos){
                _posInit = line.find_first_of("/", _directive);
                _posSemicolon = line.find_first_of(";", _posInit); // estrai do asterisco até antes do ;
                if(_posSemicolon != std::string::npos){
                    _domain = line.substr(_posInit, _posSemicolon - (_posInit));
                    std::cout << "Domain: " << _domain << std::endl; // pra TESTE
                } 
            }
            else if(line.find(".", _directive) != std::string::npos && line.find(" ", _directive) != std::string::npos && line.find_first_of(";", _directive) != std::string::npos){
                size_t _posDot = line.find(".", _directive); // busca para os casos de caminho vazio de dominio
                size_t _posSpace = line.find(" ", _directive); // busca para os casos de caminho vazio de dominio
                _posSemicolon = line.find_first_of(";", _directive); // estrai do asterisco até antes do ;
                _domain = line.substr(_posSpace + 1, _posSemicolon - (_posSpace + 1));
                // PRECISA ARMAZNAR NUM VETOR AQUI ENTÃO
                //só copiar o trecho odo código da processLocation pra splitar a line e armazenar num vector
                std::istringstream iss(line);
                std::vector<std::string> multipleDomains;
                
                std::string currentFile;
                while(iss >> currentFile){
                    multipleDomains.push_back(currentFile);
                }
                multipleDomains.erase(multipleDomains.begin());
                std::cout << "------Server names ------\n" ;
                for (size_t i = 0; i < multipleDomains.size(); ++i) {
                    std::cout << multipleDomains[i] << " " << std::endl;
                }  
                std::cout << "--------------------------" << std::endl;
                
                // std::cout << "Domain LISTA DE DOMINIOS: " << _domain << std::endl; // pra TESTE   
            }
            else{ // ou seja não encontrou nada e é defaut
                _domain = "default";
                std::cout << "Domain: " << _domain << std::endl; // pra TESTE
            }
        }
    }
}

bool ConfigParser::processRoot(std::string &line)
{
    _directive = line.find("root");
    if(_directive != std::string::npos){
        _directive += std::string("root").length();;
        _posInit = line.find_first_of("/", _directive);
        if(_posInit != std::string::npos){
            _posSemicolon = line.find_first_of(";", _posInit);
            if(_posSemicolon != std::string::npos){
                _root = line.substr(_posInit, _posSemicolon - (_posInit));
                std::cout << "Root: " << _root << std::endl;
            }
        }
        else
        {
                //antes de dar o erro devera recorrer a outras diretivas e verificar se algum dado foi armazenado
                std::cout << "Error 404 (Not Found)" << std::endl;
                _hasRoot = false;
                std::cout << "Flag não teve root: " << _hasRoot << std::endl;
                return _hasRoot;
        }
    }
    std::cout << "Flag teve root: " << _hasRoot << std::endl;
    return _hasRoot;
}


void ConfigParser::storeCurrentLocationDirectives(std::string &line)
{
	std::string directiveName = _currentLocationPathOnMap;

	std::map<std::string, LocationDirective>::iterator it = _locations.find(directiveName);
	if (it != _locations.end())
	{
		// acessa o método atraves do iterador second.método()
		// mas antes tem que extrair o nome da diretiva e o valor (splitar e colocar num vetor?)
		// tambem contar quantos itens resultam do split porque isso quer dizer que a diretiva tem mais de um valor...
		// primeiro, splitamos a linha da diretiva para poder separar o nome da diretiva de seus valores
		std::istringstream iss(line);
		std::vector<std::string> values;
		std::string singleValue;

		while (iss >> singleValue)
		{
			values.push_back(singleValue);
		}
		// if (values.size() < 2)
		// 	throw exception porque diretivas tem que ter valores
		// com os argumentos splitados e num vetor, vamos inserir a key no objeto
		it->second.addDirective(values[0], values[1]);
		std::cout << "Diretiva: " << values[0] << std::endl;
		std::cout << "Valor: " << values[1] << std::endl;
		// agora vamos verificar se tem mais valores a serem inseridos para essa
		// diretiva (se há mais itens)
		if (values.size() > 2)
		{
			size_t totalValues = values.size();
			size_t currentIndex = 2;
			while (currentIndex < totalValues)
			{
				it->second.addDirective(values[0], values[currentIndex]);
				std::cout << "Valor: " << values[currentIndex] << std::endl;
				currentIndex++;
			}
		}
	}
}

void ConfigParser::processLocation(std::string &line)
{
    std::vector<std::string> filesForPath1;
    
    _directive = line.find("location");
    if (_directive != std::string::npos)
    {
        // verificar o tipo de location ("/", "/[directorio], etc")
        // pra isso, teremos que dividir a line em partes pra ver o que é apontado pela location
        std::istringstream iss(line);
        std::vector<std::string> parts;
    
        std::string part;
        while (iss >> part)
            parts.push_back(part);
        _currentLocationPathOnMap = parts[1];
        _locationsMap[_currentLocationPathOnMap];
		_locations[_currentLocationPathOnMap];
    }
    else //acho que esse "else" tambem pode ser identificando os caminhos do location (hardcoded) e, dependendo de qual for, encaminha
    {	//pra um metodo correspondente (entao se for '/' é pra um, se for 'cgi-bin' é outro, se for regex é outro, etc)
	std::cout << std::endl;
	std::cout << "Current location: " << _currentLocationPathOnMap << std::endl;
	storeCurrentLocationDirectives(line);
    }
}

void ConfigParser::processIndex(std::string &line)
{
    if(line.find("index") != std::string::npos)
    {
        std::istringstream iss(line);
        std::string directive, indices;
        iss >> directive >> indices;
        
        std::istringstream indicesStream(indices);
        std::string index;
        while(indicesStream >> index)
            _indexFiles.push_back(index);
    }
    //só para saber a posição mesmo
    //for (size_t i = 0; i < _indexFiles.size(); ++i)
    //std::cout << "Index file at position " << i << ": " << _indexFiles[i] << std::endl;
}

void ConfigParser::processErrorPage(std::string &line)
{
    if(line.find("error_page") != std::string::npos)
    {
        std::istringstream iss(line);
        std::string directive, status, path;
        iss >> directive >> status >> path;
        int statusCode = std::atoi(status.c_str()); // Converte o status para um número inteiro
        _errorPages[statusCode] = path; // Armazena a página de erro no mapa
        std::cout << "Error page for status " << statusCode << ": " << path << std::endl;
    }
}

void ConfigParser::processAllowMethods(std::string &line)
{
    std::vector<std::string> methods;
    std::istringstream iss(line);
    std::string method;
    while (iss >> method)
        methods.push_back(method);
    for (size_t i = 0; i < methods.size(); ++i) {
        if (methods[i] == "GET") {
            std::cout << "GET method is allowed." << std::endl;
        } else if (methods[i] == "POST") {
            std::cout << "POST method is allowed." << std::endl;
        } else if (methods[i] == "DELETE") {
            std::cout << "DELETE method is allowed." << std::endl;
        }
    }  
}

void ConfigParser::processClientMaxBodySize(std::string &line)
{
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
            }
        }
    }
}

void ConfigParser::processSSL(std::string &line)
{
    if (line == "ssl on")
           std::cout << "SSL enabled" << std::endl;
    else if (line == "ssl off")
           std::cout << "SSL disabled" << std::endl;
    else
           std::cout << "Unknown SSL configuration: " << line << std::endl;
}

void ConfigParser::processAutoIndex(std::string &line) {
    try
    {
        if (line == "on")
        {
            const char *directoryPath = ".";
            DIR *directory = opendir(directoryPath);
            if (directory)
            {
                struct dirent *entry;
                std::vector<std::string> fileList;
                while ((entry = readdir(directory)) != NULL)
                    fileList.push_back(std::string(entry->d_name));
                for (size_t i = 0; i < fileList.size(); ++i)
                    std::cout << fileList[i] << std::endl;
                closedir(directory);
            } else {
                throw std::runtime_error("Error when open directory.");
            }
        } else if (line == "off") {
            std::cout << "Autoindex is on." << std::endl;
        } else {
            throw std::runtime_error("Invalid value to autoindex.");
        }
    } catch (const std::exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

void ConfigParser::processReturn(std::string &line)
{
    std::string directive = "return";
    std::size_t pos = line.find(directive);
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
}

void ConfigParser::setPort(int portNumber){
    this->_portNumber = portNumber;
}

void ConfigParser::setAddress(std::string ipAddress){
    this->_ipAddress = ipAddress;
}

const std::string& ConfigParser::getPort(void)const{
    return _portNumber;
}

const std::string& ConfigParser::getAddress(void)const{
    return _ipAddress;
}

const std::vector<std::string>& ConfigParser::getIndexFiles(void) const{
    return _indexFiles;
}

const std::string ConfigParser::getErrorPage(int statusCode)const{
    std::map<int, std::string>::const_iterator it = _errorPages.find(statusCode);
        if (it != _errorPages.end())
            return it->second;
        return "";
}
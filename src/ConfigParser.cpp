/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 21:24:02 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/15 13:29:56 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"
#include "../inc/ConfigParser.hpp"

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
	_errorPage["500"] = "./web/error/Error500.html";
}

ConfigParser::~ConfigParser() {}

// reset dir bools
void ConfigParser::resetConfig()
{
	_hasDirAllowMethods = false;
	_hasDirIndex = false;
	_hasDirListen = false;
	_hasDirLocation = false;
	_hasDirMaxBodySize = false;
	_hasDirReturn = false;
	_hasDirServerName = false;

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
}


// getter e setter do locations
std::map<std::string, LocationDirective> ConfigParser::getLocations(void) const{
	return _locations;
}

void ConfigParser::setLocations(std::map<std::string, LocationDirective>& locations){
	_locations = locations;
}

// bool ConfigParser::getHasDirLocation(void)const{
//     return _hasDirLocation;
// }

// bool ConfigParser::getHasDirListen(void)const{
//     return _hasDirListen;
// }

// bool ConfigParser::getHasDirServerName(void)const{
//     return _hasDirServerName;
// }

// bool ConfigParser::getHasDirRoot(void)const{
//     return _hasDirRoot;
// }

// bool ConfigParser::getHasDirIndex(void)const{
//     return _hasDirIndex;
// }

// bool ConfigParser::getHasDirAllowMethods(void)const{
//     return _hasDirAllowMethods;
// }

// bool ConfigParser::getHasDirMaxBodySize(void)const{
//     return _hasDirMaxBodySize;
// }

// bool ConfigParser::getHasDirReturn(void)const{
//     return _hasDirReturn;
// }

bool ConfigParser::contemApenasLetras(const std::string& str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!std::isalpha(*it)) {
            return false;
        }
    }
    return true;
}

bool ConfigParser::contemApenasNumeros(const std::string& str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!std::isdigit(*it)) {
            return false;
        }
    }
    return true;
}

/* REFATORAÇÃO DO PROCESS LISTEN
	-> vamos fazer a diretiva listen ser obrigatoria?
	-> outra: ela pode ser duplicada
	-> a pessoa tem que PELO MENOS configurar UMA PORTA (e tem que ser DENTRO DO RANGE)
		-> ou seja, NADA DE DEFINIR PORTA PADRÃO, tem que FICAR EXPLICITO no arquivo de config
	-> pessoa NÃO PODE setar MESMA PORTA várias vezes

	Para realizar os pontos acima, siga:
	1) Dividir a line de acordo com espaços - split
	2) garantir que hajam apenas DOIS ARGUMENTOS (a diretiva + valor) -> não dá pra setar vários [ip:porta] de uma vez
	3) Se NAO HOUVER ':', TEM QUE SER A PORTA SETADA (numero)
		-> se o valor da diretiva nao tiver numero, provavelmente indica um ip, e daí não atinge o requisito de a pessoa setar ao menos a porta
	4) Se HOUVER ':', daí verifica se a primeira parte (ip) é endereço ip ou um nome e armazena de acordo (endereços podem ser 127.0.0.1, algum que começa com 128 mas eu nao lembro, e 0.0.0.0)
		-> nome acho que só pode ser 'localhost' (acho que só assim funciona)
	5) Se o ip não estiver setado, armazena 'localhost' na variavel ipAddress

	** LEMBRE: como a diretiva pode ser duplicada MAS NAO PODE MESMA PORTA,
	**		   vamos ter que armazenar isso em vetores (?) mas tomar cuidado pra ver se essa porta ja nao está armazenada
*/
void ConfigParser::processListen(std::string &line){
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
	/*
		localhost:2000
		5005
		127.0.0.1:5002
		
	*/
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
		if (!contemApenasNumeros(_portNumber))
			throw ErrorException("Syntax Error: Port must be ONLY numbers.");
		std::cout << "Combo IP + Porta: " << _ipAddress << " | " << _portNumber << std::endl;
	}
	else
	{
		// quer dizer que ou é só ipAddress ou é só porta
		// se for só ipAddress -> não aceitamos (?) -> pode ter apenas porta, mas não apenas o ipAddress.. porque nós não
		// colocamos nenhuma porta por default no nosso server...
		if (contemApenasNumeros(palavras[1]))
		{
			_portNumber = palavras[1];
			_ipAddress = "localhost";
			std::cout << "Combo IP + Porta: " << _ipAddress << " | " << _portNumber << std::endl;
		}
		else
			throw ErrorException("Configuration Error: You must configure a PORT NUMBER!");
	}
    // _directive = line.find("listen");
    // if(line == "listen"){
    //     _ipAddress = "0.0.0.0";
    //     _portNumber = "80";
    //     std::cout << "\n-----[PRIMEIRO] TESTE DEFAULT-----";
    //     // std::cout << "IP Address: " << _ipAddress << std::endl;
    //     // std::cout << "Port: " << _portNumber << std::endl;
    //     //std::cout << std::endl;
    //     return ;
    // }
    // _semicolonIndex = line.find_first_of(";");
    // if(_semicolonIndex != std::string::npos && _semicolonIndex == (line.length() - 1)){
    //     line = line.substr(0, (line.length() - 1));
    // }
    // if(_directive != std::string::npos){
    //     _directive += std::string("listen ").length();
    //     posInit = line.find("://", _directive);
    //     if(posInit != std::string::npos){
    //         _ipAddress = line.substr(_directive, (line.length() - (_directive)));
    //         size_t _delimiter = line.find_last_of(":");
    //         if(_delimiter != posInit){
    //             _portNumber = line.substr(_delimiter + 1, line.length() - (_delimiter + 1));
    //             size_t portIndex = _ipAddress.find_last_of(":");
    //             _ipAddress = _ipAddress.substr(0, portIndex);
    //         }else
    //             _portNumber = "80";
    //         std::cout << "\n-----[SEGUNDO] TESTE DE HTTP E HTTPS  COM E SEM PORTA-----" << std::endl;
    //         // std::cout << "IP Address: " << _ipAddress << std::endl;
    //         // std::cout << "Port: " << _portNumber << std::endl;
    //         //std::cout << std::endl;
    //     }
    //     else
    //     { // sem protocolo http ou https // trata primeiro com :
    //         posInit = line.find(":", _directive);
    //         if (posInit != std::string::npos){// std::cout << "Posição dos ':' >> " << _posInit << std::endl;
    //             _ipAddress = line.substr(_directive, posInit - _directive);
    //             _portNumber = line.substr(posInit + 1, line.length());
    //         }
    //         else if(line.find(".", _directive) != std::string::npos){// significa que tem ipAddress (port será default)
    //            _ipAddress = line.substr(_directive);
    //            _portNumber = "80"; //a gente vai usar o default como? porque 8080 e 80 ele nao deixa (sera que na VM deixa?
    //         }
    //         else{
	// 			std::cout << "Ou SÓ TEM PORTA ou SÓ TEM IP" << std::endl;
	// 			// tem que verificar se é a porta ou o ipadress (e se estiver escrito "localhost" apenas?)
    //             std::string listenValue = line.substr(_directive);
	// 			 if (contemApenasNumeros(listenValue))
	// 			 {
	// 				_portNumber = listenValue;
    //             	_ipAddress = "localhost"; // isso ou 'localhost?'
	// 			 }
	// 			 else if (contemApenasLetras(listenValue))
	// 			 {
	// 				_ipAddress = listenValue;
	// 				// _portNumber = "4005"; //a gente vai usar o default como? porque 8080 e 80 ele nao deixa (sera que na VM deixa?)
	// 			 }
	// 			//  mas um else, pro caso das duas funções darem falso -> erro de sintaxe, pessoa pode ter misturado letra e numero e daí dá exceção
    //         }
    //         std::cout << "\n-----[TERCEIRO] TESTE SEM HTTP E HTTPS -----\n";
    //         // std::cout << "IP: " << _ipAddress << std::endl;
    //         // std::cout << "Porta: " << _portNumber << std::endl;
    //         //std::cout << std::endl;
    //     }
		// std::cout << "Que diabo...?" << std::endl;
    // }
    _hasDirListen = true;
}

void ConfigParser::processServerName(std::string &line){

    if(_hasDirServerName == true)
        throw ErrorException("Error: The Directive Server_Name has been duplicated.");
    if(line.find("server_name") != std::string::npos){
		// tem que extrair o ;
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
	std::cout << "Root: " << _root << std::endl;
    return _hasRoot;
}

void ConfigParser::storeCurrentLocationDirectives(std::string &line){
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
		if (values.size() < 2)
			throw ErrorException("Syntax Error: Missing values to directive in location block"); 

		// nao pode ter nem "listen" nem "server_name" dentro de um bloco location...
		if (values[0].find("listen") != std::string::npos || values[0].find("server_name") != std::string::npos
			|| values[0].find("location") != std::string::npos)
			throw ErrorException("Configuration Error: CANNOT have 'listen', 'server_name' or an internal 'location' in location block");
		// precisa verificar se essa diretiva JA EXISTE no location tambem...
		std::map<std::string, std::vector<std::string> >::iterator directiveIt = it->second.getDirectives().find(values[0]);
		if (directiveIt != it->second.getDirectives().end())
			throw ErrorException("Syntax Error: Duplicated directive in location block"); 
		// ------------------------------
		// com os argumentos splitados e num vetor, vamos inserir a key no objeto
		it->second.addDirective(values[0], values[1]);
		// std::cout << "Diretiva: " << values[0] << std::endl;
		// std::cout << "Valor: " << values[1] << std::endl;
		// agora vamos verificar se tem mais valores a serem inseridos para essa
		// diretiva (se há mais itens)
		if (values.size() > 2)
		{
			size_t totalValues = values.size();
			// std::cout << "Size values: " << values.size() << std::endl;
			size_t currentIndex = 2;
			while (currentIndex < totalValues)
			{
				it->second.addDirective(values[0], values[currentIndex]);
				// std::cout << "Valor: " << values[currentIndex] << std::endl;
				std::cout << currentIndex << std::endl;
				currentIndex++;
			}
		}
		// std::cout << "TAMANHO DO LOCATIONS: " << _locations.size() << std::endl;
	}
}

void ConfigParser::trimWhiteSpace(std::string &line){

    size_t startPos = line.find_first_not_of(" \t");
    if(startPos != std::string::npos)
        line = line.substr(startPos);
    size_t endPos = line.find_last_not_of(" \t");
    if(endPos != std::string::npos)
        line = line.substr(0, endPos + 1);
}

void ConfigParser::removeComments(std::string &line){

    size_t start = line.find('#');
    while (start != std::string::npos)
    {
        size_t end = line.find('\n', start);
        if (end != std::string::npos)
            line.erase(start, end - start + 1); // +1 para '\n'
        else{
            line.erase(start);
            break;
        }
        start = line.find('#');
    }
}

void ConfigParser::validateFile(std::ifstream& fileToParse){
 
    if (!fileToParse.is_open())
        throw ErrorException("File is not accessible");
    fileToParse.seekg(0, std::ios::end);
    std::streampos fileSize = fileToParse.tellg();
    fileToParse.seekg(0, std::ios::beg);
    if(fileSize <= 0)
        throw ErrorException("File is empty");
}
/*
void ConfigParser::processErrorPages(std::map<std::string, std::string> errorPages){

    errorPages = _errorPages;
    
    //vai ter que ser um mapa
    // apenasvai setar
}*/
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

/*
    DIRETIVA DUPLICADA OK
    VER SE JÁ ESTA PREENCHIDO OK
    ESTA ARMAZENANDO NO ConfigParser OK
    ANTES DE PARSEAR VERIFICAR SE O ATRIBUTO NA QUAL SE ARMAZENA JA ESTA PREENCHIDO OK
*/

/*
    EM CADA METODO PROCESS PRECISO ANTES VERIFICAR SE A
    NO CASOS DAS PORTAS PRECISO VER O RANGE DE PORTAS PERMITIDAS 
    FORA DISSO DA ERRO  EXCEÇÃO
*/

void ConfigParser::processLocation(std::string &line)
{
    std::vector<std::string> filesForPath1;

    if (line.find("location")!= std::string::npos)
    {
		if (line[line.size() - 1] != '{')
			throw ErrorException("Syntax Error: Missing '{' to open LOCATION BLOCK"); 
        // verificar o tipo de location ("/", "/[directorio], etc")
        // pra isso, teremos que dividir a line em partes pra ver o que é apontado pela location
		std::cout << "Line in Location: " << line << std::endl;
        std::istringstream iss(line);
        std::vector<std::string> parts;
    
        std::string part;
        while (iss >> part)
            parts.push_back(part);
        _currentLocationPathOnMap = parts[1];
		// antes de passar, verifica se em _locationsMap ja nao tem um location desse igual do current...
		std::map<std::string, std::vector<std::string> >::iterator it = _locationsMap.find(_currentLocationPathOnMap);
		if (it != _locationsMap.end())
			throw ErrorException("Syntax Error: DUPLICATED location");
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

const std::string& ConfigParser::getIndexFile(void) const
{
	return _indexFile;
}

void ConfigParser::setIndexFile(std::string index)
{
	_indexFile = index;
}

void ConfigParser::processIndex(std::string &line)
{
	std::vector<std::string>            _indexFiles;
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
            throw ErrorException("Found inconsistence in error_page parameter");
		std::string statusCode;
        statusCode = errorCode[1];
        for(size_t i = 0; i < statusCode.size(); ++i){
            if(!isdigit(statusCode[i]))
                throw ErrorException("Found inconsistence in status parameter");
        }
        std::string pathError;
        pathError = errorCode[2];
		struct stat info;
		if (stat(pathError.c_str(), &info) != 0)
			throw ErrorException("Syntax Error: Error page file doesn't exist!");
        _errorPage[statusCode] = pathError; // Armazena a página de erro no mapa
    }
}

const std::vector<std::string>& ConfigParser::getMethods()const
{
    return _methods;
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
    if(_hasDirMaxBodySize == true)
        throw ErrorException("Error: The Directive Client Max Body Size has been duplicated.");
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
    _hasDirMaxBodySize = true;
}

// eu acho que isso é na hora de executar o autoindexamento, nao agora
// agora é só pra armazenar essa informação no objeto configparser, que depois passa pro server
void ConfigParser::processAutoIndex(std::string &line)
{
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

void ConfigParser::setRoot(std::string root){
    _root = root;
}

const std::string& ConfigParser::getRoot(void)const{
    return _root;
}

const std::string ConfigParser::getErrorPage(int statusCode)const{
    std::map<int, std::string>::const_iterator it = _errorPages.find(statusCode);
        if (it != _errorPages.end())
            return it->second;
        return "";
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 21:24:02 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/12 12:24:22 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"
#include "../inc/ConfigParser.hpp"

ConfigParser::ConfigParser()
{
    //_line = ""; // talvez, mas acho que nao precisa mesmo
    // _domains = ""; // server_name
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
    _hasDirListen = false;
    _hasDirServerName = false;
    _hasDirRoot = false;
    _hasDirIndex = false;
    _hasDirSsl = false;
    _hasDirAllowMethods = false;
    _hasDirMaxBodySize = false;
    _hasDirReturn = false;
        
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
	_hasDirRoot = false;
	_hasDirServerName = false;
	_hasDirSsl = false;

	//
	// tinha algo aqui dando problema antes.... ver onde essas var comentadas estao sendo usadas e avaliar
	_key = "";      // ssl_certificate
    _rules = "";    // location
    _directive = 0;
    // _portNumber = "";
    // _ipAddress = "";
    // _httpAddress = "";
    _path = "";
    _posInit = 0;
    _posEnd = 0;
    _posSemicolon = 0;
    _semicolonIndex = 0;
    _delimiter = 0;
    _root = "";
    _hasRoot = true;

	_indexFiles.clear();
}


// getter e setter do locations
std::map<std::string, LocationDirective> ConfigParser::getLocations(void) const{
	return _locations;
}

void ConfigParser::setLocations(std::map<std::string, LocationDirective>& locations){
	_locations = locations;
}

bool ConfigParser::getHasDirLocation(void)const{
    return _hasDirLocation;
}

bool ConfigParser::getHasDirListen(void)const{
    return _hasDirListen;
}

bool ConfigParser::getHasDirServerName(void)const{
    return _hasDirServerName;
}

bool ConfigParser::getHasDirRoot(void)const{
    return _hasDirRoot;
}

bool ConfigParser::getHasDirIndex(void)const{
    return _hasDirIndex;
}

bool ConfigParser::getHasDirSsl(void)const{
    return _hasDirSsl;
}

bool ConfigParser::getHasDirAllowMethods(void)const{
    return _hasDirAllowMethods;
}

bool ConfigParser::getHasDirMaxBodySize(void)const{
    return _hasDirMaxBodySize;
}

bool ConfigParser::getHasDirReturn(void)const{
    return _hasDirReturn;
}

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

void ConfigParser::processListen(std::string &line){
    if(_hasDirListen == true)
	{
		std::cout << "Line: " << line << std::endl;
        throw ErrorException("Error: The Directive Listen has been duplicated.");
	}
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
               _portNumber = "80"; //a gente vai usar o default como? porque 8080 e 80 ele nao deixa (sera que na VM deixa?
            }
            else{
				std::cout << "Ou SÓ TEM PORTA ou SÓ TEM IP" << std::endl;
				// tem que verificar se é a porta ou o ipadress (e se estiver escrito "localhost" apenas?)
                std::string listenValue = line.substr(_directive);
				 if (contemApenasNumeros(listenValue))
				 {
					_portNumber = listenValue;
                	_ipAddress = "0.0.0.0"; // isso ou 'localhost?'
				 }
				 else if (contemApenasLetras(listenValue))
				 {
					_ipAddress = listenValue;
					_portNumber = "4005"; //a gente vai usar o default como? porque 8080 e 80 ele nao deixa (sera que na VM deixa?)
				 }
				//  mas um else, pro caso das duas funções darem falso -> erro de sintaxe, pessoa pode ter misturado letra e numero e daí dá exceção
            }
            std::cout << "\n-----[TERCEIRO] TESTE SEM HTTP E HTTPS -----\n";
            std::cout << "IP: " << _ipAddress << std::endl;
            std::cout << "Porta: " << _portNumber << std::endl;
            //std::cout << std::endl;
        }
		// std::cout << "Que diabo...?" << std::endl;
    }
    _hasDirListen = true;
}

void ConfigParser::processServerName(std::string &line){

    if(_hasDirServerName == true)
        throw ErrorException("Error: The Directive Server_Name has been duplicated.");
    _directive = line.find("server_name");
    if(_directive != std::string::npos){
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

        // _directive += std::string("server_name").length(); // avança para além da palavra "server_name"
        // _posInit = line.find_first_of("*", _directive); // busca para os casos de wildcard
        // if(_posInit != std::string::npos){ // se ele encontrou
        //     _posSemicolon = line.find_first_of(";", _posInit); // estrai do asterisco até antes do ;
        //     if(_posSemicolon != std::string::npos){
        //         _domain = line.substr(_posInit, _posSemicolon - (_posInit)); // se quiser extrair sem * _domain = line.substr(_posInit + 1, _posSemicolon - (_posInit + 1));
        //         std::cout << "Domain: " << _domain << std::endl;
		// 		// passar esse dominio pra classe
        //     }
        // }
        // else{
        //     if(line.find_first_of("~", _directive) != std::string::npos){ // busca para os casos de expressao regular
        //         _posInit = line.find_first_of("~", _directive);
        //         _posSemicolon = line.find_first_of(";", _posInit); // estrai do asterisco até antes do ;
        //         if(_posSemicolon != std::string::npos){
        //             _domain = line.substr(_posInit, _posSemicolon - (_posInit)); // se quiser extrair sem ~ _domain = line.substr(_posInit + 1, _posSemicolon - (_posInit + 1));
        //             std::cout << "Domain: " << _domain << std::endl;             // pra TESTE
		// 			// passar esse dominio pra classe
        //         }
        //     }
        //     else if(line.find_first_of("/", _directive) != std::string::npos){
        //         _posInit = line.find_first_of("/", _directive);
        //         _posSemicolon = line.find_first_of(";", _posInit); // estrai do asterisco até antes do ;
        //         if(_posSemicolon != std::string::npos){
        //             _domain = line.substr(_posInit, _posSemicolon - (_posInit));
        //             std::cout << "Domain: " << _domain << std::endl; // pra TESTE
		// 			// passar esse dominio pra classe
        //         } 
        //     }
        //     else if(line.find(".", _directive) != std::string::npos && line.find(" ", _directive) != std::string::npos && line.find_first_of(";", _directive) != std::string::npos){
        //         size_t _posDot = line.find(".", _directive); // busca para os casos de caminho vazio de dominio
        //         size_t _posSpace = line.find(" ", _directive); // busca para os casos de caminho vazio de dominio
        //         _posSemicolon = line.find_first_of(";", _directive); // estrai do asterisco até antes do ;
        //         _domain = line.substr(_posSpace + 1, _posSemicolon - (_posSpace + 1));
        //         // PRECISA ARMAZNAR NUM VETOR AQUI ENTÃO
        //         //só copiar o trecho odo código da processLocation pra splitar a line e armazenar num vector
        //         std::istringstream iss(line);
        //         std::vector<std::string> multipleDomains;
                
        //         std::string currentFile;
        //         while(iss >> currentFile){
        //             multipleDomains.push_back(currentFile);
        //         }
        //         multipleDomains.erase(multipleDomains.begin());
        //         std::cout << "------Server names ------\n" ;
        //         for (size_t i = 0; i < multipleDomains.size(); ++i) {
        //             std::cout << multipleDomains[i] << " " << std::endl;
        //         }  
        //         std::cout << "--------------------------" << std::endl;
		// 		// passar esse dominio pra classe


        //         // std::cout << "Domain LISTA DE DOMINIOS: " << _domain << std::endl; // pra TESTE   
        //     }
        //     else{ // ou seja não encontrou nada e é defaut
        //         _domain = "default";
        //         std::cout << "Domain: " << _domain << std::endl; // pra TESTE
		// 		// passar esse dominio pra classe
        //     }
        // }
    }
    _hasDirServerName = true;
}

bool ConfigParser::processRoot(std::string &line){
	std::istringstream iss(line);
    std::vector<std::string> partes;

    std::string palavra;
	if (_root.empty())
		std::cout << "Nesse momento, _root tá empty" << std::endl;
    while (iss >> palavra) {
        partes.push_back(palavra);
    }
	if (partes[0] != "root" || partes.size() != 2)
		throw ErrorException("Syntax Error: Directive Root");
    if(_hasDirRoot == true)
        throw ErrorException("Error: The Directive Root has been duplicated.");
	_root = partes[1];
	struct stat info;
    if (stat(_root.c_str(), &info) != 0) {
        // Erro ao chamar stat (o diretório não existe)
        throw ErrorException("Error: The path apointed by 'root' doesn't exist.");
    }
	if ((info.st_mode & S_IFDIR) == 0)
		throw ErrorException("Error: The path apointed by 'root' ISN'T A DIRECTORY.");
    // _directive = line.find("root");
	// std::cout << "ROOOT : " << partes[1] << std::endl;
    // if(_directive != std::string::npos){
        // _directive += std::string("root").length();
		
        // _posInit = line.find_first_of("/", _directive); //arrumar isso aqui
        // if(_posInit != std::string::npos){
            // _posSemicolon = line.find_first_of(";", _posInit);
            // if(_posSemicolon != std::string::npos){
                // _root = line.substr(_posInit, _posSemicolon - (_posInit));
                // std::cout << "Root: " << _root << std::endl;
            // }
        // }
        // else
        // {
            //antes de dar o erro devera recorrer a outras diretivas e verificar se algum dado foi armazenado
            // std::cout << "Error 404 (Not Found)" << std::endl;
            // _hasRoot = false;
            // std::cout << "Flag não teve root: " << _hasRoot << std::endl;
            // return _hasRoot;
        // }
    // }
    std::cout << "Flag teve root: " << _hasRoot << std::endl;
	std::cout << "Root: " << _root << std::endl;
    _hasDirRoot = true;
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
		if (values[0].find("listen") != std::string::npos || values[0].find("server_name") != std::string::npos)
			throw ErrorException("Configuration Error: CANNOT have 'listen' or 'server_name' in location block");
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
			std::cout << "Size values: " << values.size() << std::endl;
			size_t currentIndex = 2;
			while (currentIndex < totalValues)
			{
				it->second.addDirective(values[0], values[currentIndex]);
				// std::cout << "Valor: " << values[currentIndex] << std::endl;
				std::cout << currentIndex << std::endl;
				currentIndex++;
			}
		}
		std::cout << "TAMANHO DO LOCATIONS: " << _locations.size() << std::endl;
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
    
    _directive = line.find("location");
	// verificar que essa linha 'location' tem que ser finalizada com '{'
	// (tambem teremos que verificar, pra outras diretivas do config file,
	// que elas nao estejam dentro do location -> que o bool dele seja falso, no caso,
	// porque pode ter aberto o 'location...' mas pode nao ter fechado)
	// no fim, tem que verificar se esses bools (tanto o de location
	// quanto o de server) são falsos, se algum deles foi
	// verdadeiro.. é porque tem algo errado no arquivo
	// na hora que encontra um novo 'server {' tambem tem que verificar antes
	// se o bool dele está como falso, porque se for verdadeiro, quer dizer que
	// partiu pra um outro server sem fechar o anterior..

    if (_directive != std::string::npos)
    {
		// --------------------- parte nova adicionada
		if (line[line.size() - 1] != '{')
			throw ErrorException("Syntax Error: Missing '{' to open LOCATION BLOCK"); 

		// --------------- 
        // verificar o tipo de location ("/", "/[directorio], etc")
        // pra isso, teremos que dividir a line em partes pra ver o que é apontado pela location
        std::istringstream iss(line);
        std::vector<std::string> parts;
    
        std::string part;
        while (iss >> part)
            parts.push_back(part);
        _currentLocationPathOnMap = parts[1];
		// antes de passar, verifica se em _locationsMap ja nao tem um location desse igual do current...
		std::map<std::string, std::vector<std::string> >::iterator it = _locationsMap.find(_currentLocationPathOnMap);
		if (it != _locationsMap.end())
		{
			// quer dizer que ja tem esse location
			throw ErrorException("Syntax Error: DUPLICATED location");
		}
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
    if(_hasDirIndex == true)
        throw ErrorException("Error: The Directive Index has been duplicated.");
    if(line.find("index") != std::string::npos)
    {
		std::cout << "Index line found: " << line << std::endl;
        
        std::istringstream indicesStream(line);
        std::string index;
        while(indicesStream >> index)
		{
			std::cout << "Index no CONFIG: " << index << std::endl;
            _indexFiles.push_back(index);
		}
		std::cout << std::endl;
		_indexFiles.erase(_indexFiles.begin());
		std::cout << _indexFiles[0] << std::endl;
    }
	std::cout << "Tamanho do indexFiles: " << _indexFiles.size() << std::endl;
    _hasDirIndex = true;
	// precisa passar pra classe
	

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
    for (size_t i = 0; i < methods.size(); ++i) {
        if (methods[i] == "GET") {
            std::cout << "GET method is allowed." << std::endl;
        } else if (methods[i] == "POST") {
            std::cout << "POST method is allowed." << std::endl;
        } else if (methods[i] == "DELETE") {
            std::cout << "DELETE method is allowed." << std::endl;
        }
    }
    _methods = methods;
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

void ConfigParser::processSSL(std::string &line)
{
  if(_hasDirMaxBodySize == true)
        throw ErrorException("Error: The Directive SSL has been duplicated.");
    if (line == "ssl on")
           std::cout << "SSL enabled" << std::endl;
    else if (line == "ssl off")
           std::cout << "SSL disabled" << std::endl;
    else
           std::cout << "Unknown SSL configuration: " << line << std::endl;
    _hasDirSsl = true;
}

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

const std::vector<std::string>& ConfigParser::getIndexFiles(void)const{
    return _indexFiles;
}

void ConfigParser::setIndexFiles(std::string index){
    _indexFiles.push_back(index);
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
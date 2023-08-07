/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 21:24:02 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/06 20:21:15 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ConfigParser.hpp"
#include <cctype> //Para usar a função std::isspace
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>

ConfigParser::ConfigParser()
{
    //_line = ""; // talvez, mas acho que nao precisa mesmo
    _domain = ""; // server_name
    //_ssl = "";    //ssl_certificate
    _key = "";      // ssl_certificate
    _rules = "";    // location
    _nPos = std::string::npos;
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
// g++ -std=c++98 -I inc/ src/main.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp -o executavel
// ./executavel src/config.txt  

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
    if(_semicolonIndex != _nPos && _semicolonIndex == (line.length() - 1)){
        line = line.substr(0, (line.length() - 1));
    }
    if(_directive != _nPos){
        _directive += std::string("listen ").length();
        _posInit = line.find("://", _directive);
        if(_posInit != _nPos){
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
        else{ // sem protocolo http ou https // trata primeiro com :
            _posInit = line.find(":", _directive);
            if (_posInit != _nPos){// std::cout << "Posição dos ':' >> " << _posInit << std::endl;
                _ipAddress = line.substr(_directive, _posInit - _directive);
                _portNumber = line.substr(_posInit + 1, line.length());
            }
            else if(line.find(".", _directive) != _nPos){// significa que tem ipAddress (port será default)
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
        // else{
        // }
        // else{
        // }
        // else{
        // }
        
    }
}       


void ConfigParser::processServerName(std::string &line)
{
    _directive = line.find("server_name");
    if(_directive != _nPos){
        _directive += std::string("server_name").length();                               // avança para além da palavra "server_name"
        _posInit = line.find_first_of("*", _directive); // busca para os casos de wildcard
        if(_posInit != _nPos){ // se ele encontrou
            _posSemicolon = line.find_first_of(";", _posInit); // estrai do asterisco até antes do ;
            if(_posSemicolon != _nPos){
                _domain = line.substr(_posInit, _posSemicolon - (_posInit)); // se quiser extrair sem * _domain = line.substr(_posInit + 1, _posSemicolon - (_posInit + 1));
                std::cout << "Domain: " << _domain << std::endl;     
            }
        }
        else{
            if(line.find_first_of("~", _directive) != _nPos){ // busca para os casos de expressao regular
                _posInit = line.find_first_of("~", _directive);
                _posSemicolon = line.find_first_of(";", _posInit); // estrai do asterisco até antes do ;
                if(_posSemicolon != _nPos){
                    _domain = line.substr(_posInit, _posSemicolon - (_posInit)); // se quiser extrair sem ~ _domain = line.substr(_posInit + 1, _posSemicolon - (_posInit + 1));
                    std::cout << "Domain: " << _domain << std::endl;             // pra TESTE
                }
            }
            else if(line.find_first_of("/", _directive) != _nPos){
                _posInit = line.find_first_of("/", _directive);
                _posSemicolon = line.find_first_of(";", _posInit); // estrai do asterisco até antes do ;
                if(_posSemicolon != _nPos){
                    _domain = line.substr(_posInit, _posSemicolon - (_posInit));
                    std::cout << "Domain: " << _domain << std::endl; // pra TESTE
                } 
            }
            else if(line.find(".", _directive) != _nPos && line.find(" ", _directive) != _nPos && line.find_first_of(";", _directive) != _nPos){
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
    if(_directive != _nPos){
        _directive += std::string("root").length();;
        _posInit = line.find_first_of("/", _directive);
        if(_posInit != _nPos){
            _posSemicolon = line.find_first_of(";", _posInit);
            if(_posSemicolon != _nPos){
                _root = line.substr(_posInit, _posSemicolon - (_posInit));
                std::cout << "Root: " << _root << std::endl;
            }
        }
        else{
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


void ConfigParser::processLocation(std::string &line)
{
    std::vector<std::string> filesForPath1;
    
    _directive = line.find("location");
    if (_directive != _nPos)
    {
        // verificar o tipo de location ("/", "/[directorio], etc")
        // pra isso, teremos que dividir a line em partes pra ver o que é apontado pela location
        std::istringstream iss(line);
        std::vector<std::string> parts;
    
        std::string part;
        while (iss >> part) {
            parts.push_back(part);
            //std::cout << parts.back() << std::endl; //verificando a parte da line que foi colocada em parts
        }
        _currentLocationPathOnMap = parts[1];
        _locationsMap[_currentLocationPathOnMap];
    }
    else
    {
        size_t indexDirectivePos = line.find("index");
        if (indexDirectivePos != _nPos)
        {
        ///////////////////////////////////////////////>>>>>>>>>>>>>>>>
        // CODIGO DA SPLITAÇÃO E PUSHAR NO VETOR
            std::istringstream iss(line);
            std::vector<std::string> files;
        
            std::string currentFile;
            while (iss >> currentFile) {
                files.push_back(currentFile);
            }
            // inseriu os arquivos dentro do vetor 'files', mas isso tambem inclui a diretiva 'index'
            // precisamos remove-la do vetor com o método erase combinado com o método begin
            files.erase(files.begin());
            
            
         ///////////////////////////////////////////////>>>>>>>>>>>>>>>>
         ///////////////////////////////////////////////>>>>>>>>>>>>>>>>
            
            
            //pornto, agora só tem a lista de arquivos
            // basta passarmos esse vetor para o map com o índice marcando o caminho/path atrelado a esses arquivos
            _locationsMap[_currentLocationPathOnMap] = files;
            
            // agora imprimimos no terminal os itens (nomes dos arquivos) relacionados a esse path
            for (std::map<std::string, std::vector<std::string> >::iterator it = _locationsMap.begin(); it != _locationsMap.end(); ++it) {
                std::cout << "Chave Index: " << it->first << "\nValores: ";
                const std::vector<std::string>& valores = it->second;
                for (size_t i = 0; i < valores.size(); ++i) {
                    std::cout << valores[i] << " ";
                }
                std::cout << std::endl;
            }
        }
        else if (line.find("alias") != _nPos)
        {
            size_t aliasDirectivePos = line.find("alias");
            // std::string aliasFileName = line.substr(aliasDirectivePos + 5);
            // std::cout << "Nome do arquivo da location " << _currentLocationPathOnMap << " : " << aliasFileName << std::endl;
            
            //substitui pelo que tá no if acima e vê se funfa
                    // CODIGO DA SPLITAÇÃO E PUSHAR NO VETOR
            std::istringstream iss(line);
            std::vector<std::string> files;
        
            std::string currentFile;
            while (iss >> currentFile) {
                files.push_back(currentFile);
            }
            // inseriu os arquivos dentro do vetor 'files', mas isso tambem inclui a diretiva 'index'
            // precisamos remove-la do vetor com o método erase combinado com o método begin
            files.erase(files.begin());
            _locationsMap[_currentLocationPathOnMap] = files;
            
            // agora imprimimos no terminal os itens (nomes dos arquivos) relacionados a esse path
            for (std::map<std::string, std::vector<std::string> >::iterator it = _locationsMap.begin(); it != _locationsMap.end(); ++it) {
                std::cout << "Chave Alias: " << it->first << "\nValores: ";
                const std::vector<std::string>& valores = it->second;
                for (size_t i = 0; i < valores.size(); ++i) {
                    std::cout << valores[i] << " ";
                }
                std::cout << std::endl;
            }
            
        }
    }
}





/*
void ConfigParser::processHost(std::string &line)
{

}


void ConfigParser::applyDefaultValues(void)
{
}

*/

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







/*

    server_name example.com; //nome de dominio
    server_name example.com www.example.com;  // multiplos nomes de dominio
    server_name *.example.com;  // Wildcard (Coringa) em subdomínio
    server_name ~^www\.example\.(com|org)$;  //expressões regulares para dominio
    server_name example.com/path;  //caminho específico de dominio
    server_name example.com ""; // caminho vazio em um dominio
    server_name ""; // vazio default

     essa parte pode ter como delimitador:
     
        ""  espaço: casod default
        "." && "": caso de caminho vazio de dominio
        "*" asterisco: caso de wildcard sundominio
        "~" caso de expressão regular
        "/" casode caminhos
        else 
           caso de dominios
    else
        linha de configuração inválida
     

*/

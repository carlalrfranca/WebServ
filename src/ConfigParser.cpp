/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 21:24:02 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/01 21:39:01 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ConfigParser.hpp"
#include <cctype> //Para usar a função std::isspace

ConfigParser::ConfigParser()
{
    //_line = ""; // talvez, mas acho que nao precisa mesmo
    _domain = ""; // server_name
    //_ssl = "";    //ssl_certificate
    _key = "";      // ssl_certificate
    _rules = "";    // location
    _nPos = std::string::npos;
    _directive = 0;
    _port = "";
    _ipAddress = "";
    _httpAddress = "";
    _path = "";
    _posInit = 0;
    _posEnd = 0;
    _posSemicolon = 0;
    _semicolonIndex = 0;
    _delimiter = 0;
}
// g++ -std=c++98 -I inc/ src/main.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp -o executavel


/*

    _directive = line.find("server_name");
    if(_directive != _nPos){
        _directive += 11;                              
        _posInit = line.find_first_of("*", _directive); 
        if(_posInit != _nPos){                                                             
            _posSemicolon = line.find_first_of(";", _posInit);
            if(_posSemicolon != _nPos){
                _domain = line.substr(_posInit, _posSemicolon - (_posInit));
                std::cout << "Domain: " << _domain << std::endl; 
            }
        }
        else{

*/

ConfigParser::~ConfigParser() {}

void ConfigParser::processListen(std::string &line, WebServ &wsManager)
{
    _directive = line.find("listen");
    if(line == "listen"){
        _ipAddress = "0.0.0.0";
        _port = "80";
        std::cout << "\n-----[PRIMEIRO] TESTE DEFAULT-----\n";
        std::cout << "IP Address: " << _ipAddress << std::endl;
        std::cout << "Port: " << _port << std::endl;
        std::cout << "-------------------------------------" << std::endl;
        std::cout << std::endl;
        return ;
    }
    _semicolonIndex = line.find_first_of(";");
    if(_semicolonIndex != _nPos && _semicolonIndex == (line.length() - 1)){
        line = line.substr(0, (line.length() - 1));
    }
    if(_directive != _nPos){
        _directive += 7;
        _posInit = line.find("://", _directive);
        if(_posInit != _nPos){
            _ipAddress = line.substr(_directive, (line.length() - (_directive)));
            size_t _delimiter = line.find_last_of(":");
            if(_delimiter != _posInit){
                _port = line.substr(_delimiter + 1, line.length() - (_delimiter + 1));
                size_t portIndex = _ipAddress.find_last_of(":");
                _ipAddress = _ipAddress.substr(0, portIndex);
            }else
                _port = "80";
            std::cout << "\n-----[SEGUNDO] TESTE DE HTTP E HTTPS  COM E SEM PORTA-----\n";
            std::cout << "IP Address: " << _ipAddress << std::endl;
            std::cout << "Port: " << _port << std::endl;
            std::cout << "------------------------------------------------------------" << std::endl;
            std::cout << std::endl;
        }
        else{ // sem protocolo http ou https // trata primeiro com :
            _posInit = line.find(":", _directive);
            if (_posInit != _nPos){// std::cout << "Posição dos ':' >> " << _posInit << std::endl;
                _ipAddress = line.substr(_directive, _posInit - _directive);
                _port = line.substr(_posInit + 1, line.length());
            }
            else if(line.find(".", _directive) != _nPos){// significa que tem ipAddress (port será default)
               _ipAddress = line.substr(_directive);
               _port = "80";   
            }
            else{
                _port = line.substr(_directive);
                _ipAddress = "0.0.0.0";
            }
            std::cout << "\n-----[TERCEIRO] TESTE SEM HTTP E HTTPS -----\n";
            std::cout << "IP: " << _ipAddress << std::endl;
            std::cout << "Porta: " << _port << std::endl;
            std::cout << "-----------------------------------------------" << std::endl;
            std::cout << std::endl;
        }
    }
}       



void ConfigParser::processServerName(std::string &line, WebServ &wsManager)
{
    _directive = line.find("server_name");
    if(_directive != _nPos){
        _directive += 11;                               // avança para além da palavra "server_name"
        _posInit = line.find_first_of("*", _directive); // busca para os casos de wildcard
        if(_posInit != _nPos){                                                             // se ele encontrou
            _posSemicolon = line.find_first_of(";", _posInit); // estrai do asterisco até antes do ;
            if(_posSemicolon != _nPos){
                _domain = line.substr(_posInit, _posSemicolon - (_posInit)); // se quiser extrair sem * _domain = line.substr(_posInit + 1, _posSemicolon - (_posInit + 1));
                std::cout << "Domain: " << _domain << std::endl;             // pra TESTE
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
                std::cout << "Domain: " << _domain << std::endl; // pra TESTE   
            }
            else{ // ou seja não encontrou nada e é defaut
                _domain = "default";
                std::cout << "Domain: " << _domain << std::endl; // pra TESTE
            }
        }
    }
}






/*
void ConfigParser::processHost(std::string &line)
{
}

void ConfigParser::processLocation(std::string &line)
{
}

void ConfigParser::applyDefaultValues(void)
{
}
*/

/*


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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/28 13:36:38 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/27 20:38:07 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _HPP_CONFIG_PARSER_
#define _HPP_CONFIG_PARSER_
#include "HeadersLibs.hpp"

// #include <iostream>
// #include <unistd.h>
// #include <dirent.h>
// #include <sstream>
// #include <string>
// #include <fstream>
// #include <string>
// #include <cstdlib>
// #include <map>
// #include <vector>
#include "LocationDirective.hpp"


class ConfigParser{

    public:
    
        ConfigParser();
        ~ConfigParser();
        // ConfigParser(std::string line);
    
        void processListen(std::string &line);
        void processServerName(std::string &line);
        bool processRoot(std::string &line);
        void processLocation(std::string& line);
        
		// expansao do processamento do location (divisao da funcao)
		void storeCurrentLocationDirectives(std::string &line);
		void processIndex(std::string &line);
        void processErrorPage(std::string &line);
		void processRewrite(std::string &line);
		void processSSL(std::string &line);
		void processAllowMethods(std::string &line);
        void processAutoIndex(std::string &line);
        void processClientMaxBodySize(std::string &line);
        void processReturn(std::string &line);
        
        void setPort(int portNumber);
        void setAddress(std::string ipAddress);
        
        const std::string& getPort(void)const;
        const std::string& getAddress(void)const;
        const std::vector<std::string>& getIndexFiles(void)const;
        const std::string getErrorPage(int statusCode)const;
		std::map<std::string, LocationDirective> getLocations(void) const;
		void setLocations(std::map<std::string, LocationDirective>& locations);

    private:
        
        std::vector<std::string> _indexFiles;
        std::string     _domain; //server_name
        std::string     _ssl; //ssl_certificate
        std::string     _key; //ssl_certificate
        std::string     _rules; //location
        std::string     _ipAddress;
        std::string     _httpAddress;
        std::string     _path;
        std::string     _portNumber;
        std::string     _root;
        std::string     _currentLocationPathOnMap;
        size_t          _directive;
        size_t          _posInit;
        size_t          _posEnd;
        size_t          _http;
        size_t          _https;
        size_t          _posSemicolon;
        size_t          _semicolonIndex;
        size_t          _delimiter;
        bool            _hasRoot;
        
        // abaixo: map e string do path location atual pra guardar os arquivos de locations no config file
        std::map<std::string, std::vector<std::string> > _locationsMap;
        //std::vector<std::string> _multipleDomains;
        //MAP DE OBJETOS DE LOCATION PRA INCORPORAR DEPOIS (tirar o _locationsMap acima):
		std::map<std::string, LocationDirective> _locations; //vai ser -> "/" com seu objeto location respectivo, daí "cgi-bin" com seu objeto location respectivo (com suas respectivas diretivas)
        std::map<int, std::string> _errorPages;
};
#endif



/*
    g++ -std=c++98 -I inc/ src/main.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp src/LocationDirective.cpp -o executavel
    ./executavel src/config.txt

*/
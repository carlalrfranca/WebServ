/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/28 13:36:38 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/17 09:16:35 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _HPP_CONFIG_PARSER_
#define _HPP_CONFIG_PARSER_

#include "HeadersLibs.hpp"
#include "LocationDirective.hpp"
#include <stdio.h>


class ConfigParser
{

    public:
    
        ConfigParser();
        ~ConfigParser();
    
        void processListen(std::string &line);
        void processServerName(std::string &line);
        bool processRoot(std::string &line);
		void processLocation(std::string& line);
		void processIndex(std::string &line);
		void processReturn(std::string &line);
		void processAllowMethods(std::string &line);
		void processClientMaxBodySize(std::string &line);
        void processErrorPage(std::map<std::string, std::string> errorPage);

		// expansao do processamento do location (divisao da funcao)
		void storeCurrentLocationDirectives(std::string &directiveLine);
        void processErrorPage(std::string &line);
		void processRewrite(std::string &line);
        void processAutoIndex(std::string &line);
        
        void setPort(int portNumber);
        void setAddress(std::string ipAddress);
        void setRoot(std::string root);
		void setIndexFiles(std::string index);
		void setLocations(std::map<std::string, LocationDirective>& locations);
		void setIndexFile(std::string index);
        
		bool getAutoIndex(void) const;
        const std::string& getRoot(void)const;
        const std::string& getPort(void)const;
        const std::string& getAddress(void)const;
		const std::string& getIndexFile(void) const;
        const std::vector<std::string>& getMethods(void)const;
        const std::vector<std::string>& getIndexFiles(void)const;
        std::map<std::string, std::string> getErrorPage(void)const;
        std::map<std::string, LocationDirective> getLocations(void) const;
		void trimWhiteSpace(std::string &line);
		void removeComments(std::string &line);
	    void validateFile(std::ifstream& fileToParse);
		void resetConfig(void);
        void hasMandatoryParameters(void);
        void listFilesAndGenerateHtml(std::string &line);
	    // bool getHasDirLocation(void)const;
	    // bool getHasDirListen(void)const;
        // bool getHasDirServerName(void)const;
        // bool getHasDirRoot(void)const;
        // bool getHasDirIndex(void)const;
        // bool getHasDirAllowMethods(void)const;
        // bool getHasDirMaxBodySize(void)const;
        // bool getHasDirReturn(void)const;
		// validar o tipo do ipaddress
		bool contemApenasLetras(const std::string& str);
		bool contemApenasNumeros(const std::string& str);
		//acrescentado dia 06.09
        
    private:
        
        std::string            _indexFile;
        std::vector<std::string>            _domains; //server_name
        std::map<std::string, std::string>  _errorPage;
        // std::string     _rules; //location
        std::string     _ipAddress;
        std::string     _path;
        std::string     _portNumber;
        std::string     _root;
        std::string     _currentLocationPathOnMap;
        size_t          _directive;
        size_t			_maxBodySize;
		bool			_autoIndexOn;

		bool            _hasRoot;
        bool            _hasDirListen;
        bool            _hasDirLocation;
        bool            _hasDirServerName;
        bool            _hasDirIndex;
        bool            _hasDirAllowMethods;
        bool            _hasDirMaxBodySize;
        bool            _hasDirReturn;
		bool			_hasAutoIndex;

        
        std::vector<std::string> _methods;
        // abaixo: map e string do path location atual pra guardar os arquivos de locations no config file
        std::map<std::string, std::vector<std::string> > _locationsMap;
        //std::vector<std::string> _multipleDomains;
        //MAP DE OBJETOS DE LOCATION PRA INCORPORAR DEPOIS (tirar o _locationsMap acima):
		std::map<std::string, LocationDirective> _locations; //vai ser -> "/" com seu objeto location respectivo, daí "cgi-bin" com seu objeto location respectivo (com suas respectivas diretivas)
        std::map<int, std::string> _errorPages;
        
        
        class ErrorException: public std::exception{
        	public:
	
        	    ErrorException(const std::string& message) throw() : _errorMessage(message) {}
	
				virtual ~ErrorException() throw() {}
				
				virtual const char* what() const throw(){
        	        return _errorMessage.c_str();
				}
        	private:
        	    std::string _errorMessage;
	
        };
};

#endif



/*
    g++ -std=c++98 -I inc/ src/main.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp src/LocationDirective.cpp -o executavel
    ./executavel src/config.txt

*/
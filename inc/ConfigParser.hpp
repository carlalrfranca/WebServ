/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/28 13:36:38 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/26 19:36:28 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #ifndef _HPP_CONFIG_PARSER_
// #define _HPP_CONFIG_PARSER_

// #include "Utils.hpp"
// #include "HeadersLibs.hpp"
// #include "LocationDirective.hpp"

// struct ListenDirective
// {
//     std::string ipAddress;
//     std::string portNumber;
// };

// class ConfigParser
// {
//     public:
    
//         ConfigParser();
//         ~ConfigParser();
    
//         bool processRoot(std::string &line);
// 		void processIndex(std::string &line);
//         void processListen(std::string &line);
// 		void processReturn(std::string &line);
// 		void processRewrite(std::string &line);
// 		void processLocation(std::string& line);
//         void processAutoIndex(std::string &line);
//         void processErrorPage(std::string &line);
//         void processServerName(std::string &line);
// 		void processAllowMethods(std::string &line);
// 		void processClientMaxBodySize(std::string &line);

//         void setPort(int portNumber);
//         void setRoot(std::string root);
// 		void setIndexFile(std::string index);
// 		void setIndexFiles(std::string index);
//         void setAddress(std::string ipAddress);
// 		void setLocations(std::map<std::string, LocationDirective>& locations);
        
// 		bool getAutoIndex()const;
//         const std::string& getRoot()const;
//         const std::string& getPort()const;
//         const std::string& getAddress()const;
// 		const std::string& getIndexFile() const;
//         const std::vector<std::string>& getMethods()const;
//         const std::vector<std::string>& getIndexFiles()const;
//         std::map<std::string, std::string> getErrorPage()const;
//         std::map<std::string, LocationDirective> getLocations() const;

//         void hasMandatoryParameters();
//         void listFilesAndGenerateHtml(std::string &line);
// 		void storeCurrentLocationDirectives(std::string &directiveLine);
//         // void processErrorPage(std::map<std::string, std::string> errorPage);
//         // bool getHasDirLocation(void)const;
// 	    // bool getHasDirListen(void)const;
//         // bool getHasDirServerName(void)const;
//         // bool getHasDirRoot(void)const;
//         // bool getHasDirIndex(void)const;
//         // bool getHasDirAllowMethods(void)const;
//         // bool getHasDirMaxBodySize(void)const;
//         // bool getHasDirReturn(void)const;
// 		// validar o tipo do ipaddress 
//         // void trimWhiteSpace(std::string &line);

// 		void resetConfig();
// 		void checkDuplicatePorts();
// 		void removeComments(std::string &line);
// 	    void validateFile(std::ifstream& fileToParse);
// 		bool contemApenasLetras(const std::string& str);
// 		bool contemApenasNumeros(const std::string& str);
// 		//acrescentado dia 06.09
        
//     private:
        
//         std::string     _currentLocationPathOnMap;
//         std::string     _portNumber;
//         std::string     _indexFile;
//         std::string     _ipAddress;
//         std::string     _path;
//         std::string     _root;
//         Utils           _utils;
//         size_t          _directive;
//         size_t			_maxBodySize;
//         bool            _hasDirAllowMethods;
//         bool            _hasDirMaxBodySize;
//         bool            _hasDirServerName;
//         bool            _hasDirLocation;
//         bool            _hasDirReturn;
//         bool            _hasDirListen;
// 		bool			_hasAutoIndex;
//         bool            _hasDirIndex;
//         bool			_autoIndexOn;
//         bool            _hasRoot;

        
//         std::vector<std::string>                            _domains; //server_name
//         std::vector<std::string>                            _methods;
//         std::map<int, std::string>                          _errorPages;
//         std::map<std::string, std::string>                  _errorPage;
// 		std::map<std::string, LocationDirective>            _locations;
//         std::map<std::string, std::vector<std::string> >    _locationsMap;
//         std::vector<std::string>                            _portNumbers;
//         std::vector<std::string>                            _ipAddresses;
        
        
//     class ErrorException: public std::exception
//     {
//     	public:

//     	    ErrorException(const std::string& message) throw() : _errorMessage(message){}
// 			virtual ~ErrorException() throw(){}
// 			virtual const char* what() const throw()
//             {
//     	        return _errorMessage.c_str();
// 			}

//     	private:
        
//     	    std::string _errorMessage;

//     };
// };

// #endif

#ifndef _HPP_CONFIG_PARSER_
#define _HPP_CONFIG_PARSER_

#include "Utils.hpp"
#include "HeadersLibs.hpp"
#include "LocationDirective.hpp"


class ConfigParser
{

    public:
    
        ConfigParser();
        ~ConfigParser();
    
        bool processRoot(std::string &line);
		void processIndex(std::string &line);
        //void processListen(std::string &line);
		void processReturn(std::string &line);
		void processRewrite(std::string &line);
		void processLocation(std::string& line);
        void processAutoIndex(std::string &line);
        void processErrorPage(std::string &line);
        void processServerName(std::string &line);
		void processAllowMethods(std::string &line);
		void processClientMaxBodySize(std::string &line);
		void processListen(std::string &line);

        void setPort(int portNumber);
        void setRoot(std::string root);
		void setIndexFile(std::string index);
		void setIndexFiles(std::string index);
        void setAddress(std::string ipAddress);
		void setLocations(std::map<std::string, LocationDirective>& locations);
        
		bool getAutoIndex()const;
        const std::string& getRoot()const;
        const std::string& getPort()const;
        const std::string& getAddress()const;
		const std::string& getIndexFile() const;
        const std::vector<std::string>& getMethods()const;
        const std::vector<std::string>& getIndexFiles()const;
        std::map<std::string, std::string> getErrorPage()const;
        std::map<std::string, LocationDirective> getLocations() const;

        const std::vector<std::string>& getAllPorts()const;
        const std::vector<std::string>& getAllIps()const;

        void hasMandatoryParameters();
        void listFilesAndGenerateHtml(std::string &line);
		void storeCurrentLocationDirectives(std::string &directiveLine);
		void hasprohibitedDirectiveInLocation(std::string &directive);
        //void processErrorPage(std::map<std::string, std::string> errorPage);
	    
        // bool getHasDirLocation(void)const;
	    // bool getHasDirListen(void)const;
        // bool getHasDirServerName(void)const;
        // bool getHasDirRoot(void)const;
        // bool getHasDirIndex(void)const;
        // bool getHasDirAllowMethods(void)const;
        // bool getHasDirMaxBodySize(void)const;
        // bool getHasDirReturn(void)const;
		// validar o tipo do ipaddress

        
        //void trimWhiteSpace(std::string &line);


		void resetConfig();
	    void validateFile(std::ifstream& fileToParse);
		void removeComments(std::string &line);
		bool contemApenasLetras(const std::string& str);
		bool contemApenasNumeros(const std::string& str);
		bool duplicatedPort(std::string& portNumber);
		//acrescentado dia 06.09
        
    private:
        
        std::string     _indexFile;
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
        Utils           _utils;

        
        std::vector<std::string>                            _domains; //server_name
        std::vector<std::string>                            _methods;
        std::vector<std::string>                            _allPorts;
        std::vector<std::string>                            _allIps;
        std::map<int, std::string>                          _errorPages;
        std::map<std::string, std::string>                  _errorPage;
		std::map<std::string, LocationDirective>            _locations;
        std::map<std::string, std::vector<std::string> >    _locationsMap;
        
        
    class ErrorException: public std::exception
    {
    	public:

    	    ErrorException(const std::string& message) throw() : _errorMessage(message){}
			virtual ~ErrorException() throw(){}
			virtual const char* what() const throw()
            {
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



/*
    g++ -std=c++98 -I inc/ src/main.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp src/LocationDirective.cpp -o executavel
    ./executavel src/config.txt

*/
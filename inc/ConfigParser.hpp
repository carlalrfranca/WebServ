/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/28 13:36:38 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/10 21:43:49 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
    
        void checkErrorPages();
        bool processRoot(std::string &line);
        bool isValidIPAddress(const std::string &ipAddress);
		bool isValidPort(const std::string &port);
		bool containsInvalidCaractersForMaxBodySize(const std::string& str);
		bool duplicatedPort(std::string& portNumber);
        void processIndex(std::string &line);
		void processReturn(std::string &line);
		void processRewrite(std::string &line);
		void processLocation(std::string& line);
        void processAutoIndex(std::string &line);
        void processErrorPage(std::string &line);
        void processServerName(std::string &line);
		void processAllowMethods(std::string &line);
		void processClientMaxBodySize(std::string &line);
		void processListen(std::string &line);
		void storeCurrentLocationDirectives(std::string &directiveLine);
		void hasprohibitedDirectiveInLocation(std::string &directive);
		void resetConfig();
	    void validateFile(std::ifstream& fileToParse);
		void removeComments(std::string &line);
        size_t convertToKB(std::string &sizeStr);

        void setPort(int portNumber);
        void setRoot(std::string root);
		void setIndexFile(std::string index);
        void setAddress(std::string ipAddress);
		void setLocations(std::map<std::string, LocationDirective>& locations);
        void setMaxBodySize(size_t maxBodySize);

		bool getAutoIndex()const;
		size_t getMaxBodySize()const;
        const std::string& getRoot()const;
        const std::string& getPort()const;
        const std::string& getAddress()const;
		const std::string& getIndexFile() const;
        const std::vector<std::string>& getMethods()const;
		const std::vector<std::string>& getDomains()const;
        const std::vector<std::string>& getIndexFiles()const;
        const std::vector<std::string>& getAllIps()const;
        const std::vector<std::string>& getAllPorts()const;
        std::map<std::string, std::string> getErrorPage()const;
        std::map<std::string, LocationDirective> getLocations() const;
        
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

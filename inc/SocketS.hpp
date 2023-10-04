/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketS.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 17:46:53 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/04 17:38:59 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _SOCKETS_HPP_
#define _SOCKETS_HPP_
#include "HeadersLibs.hpp"
#include "LocationDirective.hpp"

// criação do sockt, vinculação com o endereço e a escuta
// tudo em relação ao socket do SocketS

class SocketS
{
    public:
        
        SocketS();
        ~SocketS();
        
        void setPort(std::string portNumber);
        void setAddress(std::string ipAddress);
        void setWebServSocket(int webServSocket);
        void setServerName(std::vector<std::string> serverName);
		void setIndexFile(std::string index);
        void setRoot(std::string root);
		void setMaxBodySize(size_t maxBodySize);
        void setMethods(std::vector<std::string>methods);
		void setLocations(std::map<std::string, LocationDirective> locations);
		void setErrorPage(std::map<std::string, std::string> errorPages);
        
		size_t getMaxBodySize() const;
        const int& getFD()const;
        const int& getWebServSocket()const;
        const std::string& getPort()const;
        const std::string& getAddress()const;
        const std::string& getRoot()const;
		const std::string& getIndexFile() const;
		const std::string getErrorPage(int statusCode)const;
        std::vector<std::string> getServerName()const;
        std::vector<std::string> getMethods() const;
		std::map<std::string, LocationDirective> getLocations()const;
        
		void initServer();
        void loadConfiguration();
		void addAliasToHostsFile(const std::string& alias);
        
    private:
        
        int                                 _webServSocket; //FD
        std::string                         _statusCode;
		std::string		                    _indexFile;
        std::string                         _root;
        std::string                         _portNumber;
        std::string                         _ipAddress;
        std::string                         _response;
        size_t								_maxBodySize;
        std::vector<std::string>            _serverName;
        std::vector<std::string>            _methods;
		std::map<std::string, std::string>  _errorPage;
        // struct sockaddr_in _socketAddress;
        // int bindSocketSListenConnections(); //vincula socket a um endereço e porta 
        std::map<std::string, LocationDirective> _locations;
        
        class SocketSException: public std::exception{
        public:
        
            SocketSException(const char* errorMessage) : message(errorMessage){}
            virtual const char* what()const throw(){
                return message;
            }
        
        private:
        
            const char* message;
        };
};
//esse aqui está mais para depuração e apresentação das informações
std::ostream& operator<<(std::ostream& output, const SocketS& instance);
#endif


/*
    g++ -std=c++98 -I inc/ src/main.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp -o executavel
    ./executavel src/config.txt

*/
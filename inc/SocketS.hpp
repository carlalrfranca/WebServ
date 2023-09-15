/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketS.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 17:46:53 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/15 13:30:01 by lfranca-         ###   ########.fr       */
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
        const int& getFD()const;
        void loadConfiguration();
        
        void setPort(std::string portNumber);
        void setAddress(std::string ipAddress);
        void setWebServSocket(int webServSocket);
        void setServerName(std::vector<std::string> serverName);
        std::vector<std::string> getServerName(void)const;
        const std::string& getPort(void)const;
        const std::string& getAddress(void)const;
        const int& getWebServSocket(void)const;
		std::map<std::string, LocationDirective> getLocations(void) const;
		void setLocations(std::map<std::string, LocationDirective> locations);
        void setMethods(std::vector<std::string>methods);
        std::vector<std::string> getMethods(void) const;
        void setRoot(std::string root);
        const std::string& getRoot(void)const;
		const std::string& getIndexFile(void) const;
		void setIndexFile(std::string index);
        
    private:
        
		std::string		_indexFile;
        int             _webServSocket; //FD
        std::string     _root;
        std::string     _portNumber;
        std::string     _ipAddress;
        std::string     _response; //talvez na response class
        // std::string     _cssContent;
        std::vector<std::string> _serverName;
        std::vector<std::string> _methods;
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
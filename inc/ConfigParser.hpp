/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/28 13:36:38 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/01 16:01:26 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef _HPP_CONFIG_PARSER_
#define _HPP_CONFIG_PARSER_

#include <iostream>
#include <unistd.h>
#include <string>
#include <cstdlib>
#include "WebServ.hpp"

class WebServ;

class ConfigParser{

    public:
    
        ConfigParser();
        ~ConfigParser();
        //ConfigParser(std::string line);
    
        void processListen(std::string& line, WebServ& webserv_manager);
        void processServerName(std::string &line, WebServ &wsManager);
        //void processHost(std::string& line);
        //void processLocation(std::string& line);
        //void applyDefaultValues(void);
    
    private:
        
        //SocketS     _serverSocket;
        
        //std::string _line; //talvez, mas acho que nao precisa mesmo
        std::string     _domain; //server_name
        std::string     _ssl; //ssl_certificate
        std::string     _key; //ssl_certificate
        std::string     _rules; //location
        std::string     _ipAddress;
        std::string     _httpAddress;
        std::string     _path;
        std::string     _port;
        size_t          _nPos;
        size_t          _directive;
        size_t          _posInit;
        size_t          _posEnd;
        size_t          _http;
        size_t          _https;
        size_t          _posSemicolon;

};

#endif
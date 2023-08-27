/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeadersLibs.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/24 16:15:00 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/24 18:55:08 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _HEADERS_LIBS_HPP_
#define _HEADERS_LIBS_HPP_

// C++ LIBRARIES
#include <sys/epoll.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// LOCAL
// #include "LocationDirective.hpp"
// #include "ConfigParser.hpp"
// #include "RequestParser.hpp"
// #include "WebServ.hpp"
// #include "ConfigParser.hpp"
// #include "SocketS.hpp"


// DEFINES
#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cctype> //Para usar a função std::isspace
#include <vector>
#include <string>
#include <map>
#include <stdexcept>



// COLORS DEFINES
#define YELLOW  "\033[0;33m"
#define BLUE    "\033[0;34m"
#define RED     "\033[0;31m"
#define END     "\033[0m"

// HTTP DEFINES

#endif
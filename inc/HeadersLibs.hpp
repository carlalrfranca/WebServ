/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeadersLibs.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/24 16:15:00 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/24 20:52:52 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _HEADERS_LIBS_HPP_
#define _HEADERS_LIBS_HPP_

// C++ LIBRARIES
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <netdb.h>

// LOCAL
#include "../inc/RequestParser.hpp"
#include "../inc/ConfigParser.hpp"
#include "../inc/StatusMessages.hpp"
#include "LocationDirective.hpp"
#include "../inc/WebServ.hpp"
#include "ConfigParser.hpp"
#include "SocketS.hpp"

// DEFINES
#include <stdexcept>
#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <vector>
#include <string>
#include <map>

// COLORS DEFINES
#define YELLOW  "\033[0;33m"
#define BLUE    "\033[0;34m"
#define RED     "\033[0;31m"
#define END     "\033[0m"

// HTTP DEFINES

#endif
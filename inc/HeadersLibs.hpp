/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeadersLibs.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/24 16:15:00 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/06 21:12:17 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _HEADERS_LIBS_HPP_
#define _HEADERS_LIBS_HPP_

// C++ LIBRARIES

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <time.h>

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
#include <cctype> // to std::isspace
#include <map>


// COLORS DEFINES
#define YELLOW  "\033[0;33m"
#define BLUE    "\033[0;34m"
#define RED     "\033[0;31m"
#define END     "\033[0m"

// HTTP DEFINES

#endif
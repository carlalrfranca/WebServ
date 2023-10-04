/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 17:24:52 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/03 22:02:52 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"
#include "../inc/WebServ.hpp"

int main(int argc, char **argv)
{
	std::string filename;
	if(argc == 1)
		filename = "./src/config.txt";
    else if(argc == 2)
	{
		struct stat info;
		 if(stat(argv[1], &info) != 0)
		 {
			std::cerr << "Error: Config file doesn't exist!" << std::endl;
			exit(EXIT_FAILURE);
		 }
		filename = argv[1];
	}
    if(argc != 2)
    {
        std::cerr << "Error: Too many arguments" << std::endl;
        exit(EXIT_FAILURE);
    }
    try
    {
        WebServ WebServ(filename);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }  
    return 0;
 }


/*
    em 31.08.2023

   g++ -std=c++98 -I inc/ src/main.cpp src/CGI.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp src/LocationDirective.cpp src/Epoll.cpp src/Request.cpp src/Response.cpp -o executavel_com_response 
    ./executavel_com_response src/config.txt

*/

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 17:24:52 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/10 13:05:04 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"
#include "../inc/WebServ.hpp"

bool shouldExit = false;

void handleSignal(int signal)
{
    std::cout << YELLOW << "[EXITING WEBSERVER....] - " << signal << END << std::endl;
	shouldExit = true;
}

int main(int argc, char **argv)
{
	signal(SIGINT, handleSignal);
	std::string filename;
	if(argc == 1)
		filename = "./cfgs/default_config.txt";
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

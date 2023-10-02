/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UtilsResponse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/01 23:24:08 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/UtilsResponse.hpp"

UtilsResponse::UtilsResponse()
{}

UtilsResponse::~UtilsResponse()
{}

std::string UtilsResponse::readHtmlFile(const std::string& filePath)
{
    std::ifstream file(filePath.c_str());
    if(!file.is_open())
        return"";
    std::string content;
    char character;
    while(file.get(character))
        content += character;
    file.close();
    return content;
}

int UtilsResponse::selectServer(Request& request, std::vector<SocketS> serverSocket)
{
    int indexChosenSocket = -1;
    std::string domainRequest = request.getDomainRequest();
    std::string portRequest = request.getPortRequest();
    
	std::cout << RED << "Foi aqui?" << END << std::endl;
    if(portRequest.empty())
    {
        for (size_t serverIndex = 0; serverIndex < serverSocket.size(); ++serverIndex)
        {
			std::cout << YELLOW << "-------------- DENTRO DO SELECT SERVER ------------" << std::endl;
	        std::cout << "Dominio da request: " << request.getDomainRequest() << "| Size: " << request.getDomainRequest().size() << std::endl;
	        std::cout << "Address do servidor: " << serverSocket[serverIndex].getAddress() << "| Size: " << serverSocket[serverIndex].getAddress().size() << std::endl;
	        std::cout << "Porta do servidor: " << serverSocket[serverIndex].getPort() << " | Size: " << serverSocket[serverIndex].getPort().size() << std::endl;
	        std::cout << "Porta da request: " << request.getPortRequest() << " | Size: " << request.getPortRequest().size() << END << std::endl;
            std::vector<std::string> tempServerName = serverSocket[serverIndex].getServerName();
            for (size_t nameIndex = 0; nameIndex < tempServerName.size(); ++nameIndex)
            {
				if(domainRequest == tempServerName[nameIndex])
				{
					std::cout << YELLOW << "Servidor escolhido!" << END << std::endl;
                	indexChosenSocket = serverIndex;
					return indexChosenSocket;
				}
            }
		}
    } else {
	    for (size_t serverIndex = 0; serverIndex < serverSocket.size(); ++serverIndex)
	    {
	        std::cout << BLUE << "-------------- DENTRO DO SELECT SERVER ------------" << std::endl;
	        std::cout << "Dominio da request: " << request.getDomainRequest() << "| Size: " << request.getDomainRequest().size() << std::endl;
	        std::cout << "Address do servidor: " << serverSocket[serverIndex].getAddress() << "| Size: " << serverSocket[serverIndex].getAddress().size() << std::endl;
	        std::cout << "Porta do servidor: " << serverSocket[serverIndex].getPort() << " | Size: " << serverSocket[serverIndex].getPort().size() << std::endl;
	        std::cout << "Porta da request: " << request.getPortRequest() << " | Size: " << request.getPortRequest().size() << END << std::endl;
	        if (request.getDomainRequest() == serverSocket[serverIndex].getAddress())
	        {
	            std::cout << "Dominio igual!" << std::endl;
	            if (request.getPortRequest() == serverSocket[serverIndex].getPort())
	            {
	                std::cout << "Porta igual!!!!!" << std::endl;
	                // precisa verificar o sevrer_name nesse caso?
	                //se for tudo certinho...
	                indexChosenSocket = serverIndex; //depois passar isso pra essa condicao abaixo
					return indexChosenSocket;
	            }
	        }  
	    }
	}
	std::cout << RED << "Talvez" << END << std::endl;
    return indexChosenSocket;
    // construir resposta de erro... (se sair do loop é que deu merda)
}

std::string UtilsResponse::readFileToString(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
	{
        std::cout << "Não deu pra abrir o html" << std::endl;
        return ""; // Retorna uma string vazia se não for possível abrir o arquivo
    }
    std::string content;
    std::string line;
    while (std::getline(file, line))
        content += line + "\n"; // Adiciona cada linha ao conteúdo da string
    file.close();
    return content;
}

bool UtilsResponse::isDirectory(const std::string& path)
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
	{
        std::cout << RED << "Esse file >>> " << path << " e aí?" << END << std::endl;
		return false;
	}
    return (info.st_mode & S_IFDIR) != 0;
}

std::map<std::string, LocationDirective>::iterator UtilsResponse::findRequestedLocation(Request &request, std::map<std::string, LocationDirective>& serverLocations)
{
    std::map<std::string, LocationDirective>::iterator it = serverLocations.find(request.getURI()); //cria um iterador p percorrer o map e encontrar uma chave correspondente ao vlr de retorno da getURI
	std::string uri = request.getURI();
	if (it == serverLocations.end())
	{
		bool canBeRootLoc = false;
		// casos em que nao tem literalmente o que vem na URI nos location (tipo /styles/styles.css ou /about.html)
		// vamos ter que encontrar o location que lida com esse tipo de URI. No caso do 'styles/styles.css', tem que ser o location 'styles/'
		// e no caso do '/about.html', tem que ser o location 'about'
		for(std::map<std::string, LocationDirective>::iterator locFind = serverLocations.begin(); locFind != serverLocations.end(); ++locFind) //cria um iterador p percorrer o map e encontrar uma chave correspondente ao vlr de retorno da getURI
		{
			if (locFind->first == "/")
				canBeRootLoc = true;
			if (locFind->first != "/")
			{
				// nao pode tirar as duas primeiras condições? só deixar a terceira?
		        if(uri == locFind->first || uri.find(locFind->first + "/") != std::string::npos || uri.find(locFind->first) != std::string::npos){
	                it = locFind;
					std::cout << YELLOW << "LOCATION que se ENCAIXA nessa URI >>>>> " << it->first << END << std::endl;
	                return it;
	            }
			}
	    }
		if (it == serverLocations.end() && canBeRootLoc == true) //caso nao haja um location mais especifico, o location '/' vai responder pela requisição
			it = serverLocations.find("/");
	}
	std::cout << YELLOW << "LOCATION que se ENCAIXA >> " << it->first << END << std::endl;
	return it;
}

bool UtilsResponse::isThisMethodAllowed(std::map<std::string, LocationDirective>& serverLocations, Request &request, SocketS &server, std::string& requestMethod)
{
	std::vector<std::string> allowed_methods = server.getMethods();
	std::map<std::string, LocationDirective>::iterator itLocation = findRequestedLocation(request, serverLocations);
	if (itLocation != serverLocations.end())
	{
		std::map<std::string, std::vector< std::string > > locationDirectives;
		locationDirectives = itLocation->second.getDirectives();
		std::map<std::string, std::vector< std::string > >::iterator itLocationAllowMethods = locationDirectives.find("allow_methods");
		if (itLocationAllowMethods != locationDirectives.end())
		{
			std::vector<std::string> locationAllowedMethods = itLocationAllowMethods->second;
			for (std::vector<std::string>::iterator it = locationAllowedMethods.begin(); it != locationAllowedMethods.end(); ++it)
			{
    		    if (strcmp(it->c_str(), requestMethod.c_str()) == 0)
    		        return true;
    		}
		} else {
			for  (std::vector<std::string>::iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it)
			{
    		    if (strcmp(it->c_str(), requestMethod.c_str()) == 0)
					return true;
    		}
		}
	}
	return false;
}

size_t UtilsResponse::strToSizeT(const std::string& str)
{
	size_t sizeValue = 0;
    std::istringstream ss(str);
    ss >> sizeValue;
	return sizeValue;
}
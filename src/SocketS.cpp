/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketS.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 17:46:53 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/01 14:23:25 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"
#include "../inc/SocketS.hpp"

SocketS::SocketS()
{
    _portNumber = "";
    _ipAddress = "";
    _response = "";
    _webServSocket = 0;
    _root = "";
    _statusCode = 80;
}


SocketS::~SocketS(){}

// getter e setter do locations

std::map<std::string, LocationDirective> SocketS::getLocations() const
{
	return _locations;
}

void SocketS::setLocations(std::map<std::string, LocationDirective> locations)
{
	_locations = locations;
} 

const int& SocketS::getFD()const
{
    return _webServSocket;
}

void SocketS::loadConfiguration(){}

void SocketS::setPort(std::string portNumber)
{
    unsigned int port = 0;

    for(size_t i = 0; i < portNumber.length(); ++i){
        if(!std::isdigit(portNumber[i]))
            throw SocketSException("Error: Worng Syntax: port");
    }
	port = atoi(portNumber.c_str());
    if(port < 1 || port > 65636)
        throw SocketSException("Error: Worng Syntax: port");
    this->_portNumber = portNumber;
}

void SocketS::setAddress(std::string ipAddress)
{
    this->_ipAddress = ipAddress;
}

void SocketS::setWebServSocket(int webServSocket)
{
    this->_webServSocket = webServSocket;
}

const std::string& SocketS::getPort()const
{
    return _portNumber;
}

const std::string& SocketS::getAddress()const
{
    return _ipAddress;
}

const std::string SocketS::getErrorPage(int statusCode)const
{
	std::stringstream toConvertToStr;
    toConvertToStr << statusCode;
    std::string statusCodeStr = toConvertToStr.str();
	std::map<std::string, std::string>::const_iterator it = _errorPage.find(statusCodeStr);
	if (it != _errorPage.end())
		return it->second;
	return "";
}

const int& SocketS::getWebServSocket()const
{
    return _webServSocket;
}

void SocketS::setServerName(std::vector<std::string> serverName)
{
     this->_serverName = serverName;
}

std::vector<std::string> SocketS::getServerName()const
{
    return _serverName;
}
 
void SocketS::setMethods(std::vector<std::string> methods)
{
    _methods = methods;
}

std::vector<std::string> SocketS::getMethods()const
{
    return _methods;
}

void SocketS::setRoot(std::string root)
{
    _root = root;
}

const std::string& SocketS::getRoot()const
{
    return _root;
}

const std::string& SocketS::getIndexFile()const
{
	return _indexFile;
}

void SocketS::setIndexFile(std::string index)
{
	_indexFile = index;
}

void SocketS::setErrorPage(std::map<std::string, std::string> errorPages)
{
	_errorPage = errorPages;
}

void SocketS::addAliasToHostsFile(const std::string& alias)
{
    std::string command = "echo '127.0.0.1 " + alias + "' >> /etc/hosts";
    int result = system(command.c_str());
    if(result != 0)
        throw SocketSException("Error while adding the alias to the file /etc/hosts.");
}

void SocketS::initServer()
{
	setWebServSocket(socket(AF_INET, SOCK_STREAM, 0));
	if(getWebServSocket() == -1)
	{
        throw SocketSException("Socket Error: Failed to create socket!");
    }
	//configura endereço do servidor e inicializa os campos da estrutura com 0
    struct sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET; //socket usará os ends. IPv4
    server_address.sin_port = htons(std::atoi(getPort().c_str())); //usa a função htons para converter8080 para a ordem de bytes da rede e atribui a sin_port
    server_address.sin_addr.s_addr = INADDR_ANY; //especifica o end.IP que o socket do server será vinculado
    //chamada para o bind - vincula o socket ao endereço e porta, 0 -1 tem haver com a falha na chamada do bind
    if(bind(getWebServSocket(),(struct sockaddr*)&server_address, sizeof(server_address)) == -1)
	{
        close(getWebServSocket());
          throw SocketSException("Socket Error: Bind failed!");
    }
    //habilitar o socket para aguardar conexões de entrada. ) 5 representa o tamanho máximo da fila de conexões pendentes.
    if(listen(getWebServSocket(), 5) == -1)
	{
        close(getWebServSocket());
        throw SocketSException("Socket Error: Listen failed!");
	}
	std::string serverName;
	if(!_serverName.empty())
	{
		for(std::vector<std::string>::const_iterator it = _serverName.begin(); it != _serverName.end(); ++it)
			serverName = *it;
		addAliasToHostsFile(serverName);
	}
}





// int SocketS::bindSocketListenConnections(){
// 
    // try{
        // if(bind(_webServSocket,(struct sockaddr*)&_serverAddress, sizeof(_serverAddress)) == -1){
            // throw SocketSException("Error: when binding socket.");
        // }
        // if(listen(_webServSocket, 5) == -1){
            // throw SocketSException("Error: when listening socket.");
        // }
//  
    // }catch(const SocketSException& e){
        // std::cerr << e.what() << std::endl;
        // close(_webServSocket);
        // return -1;
    // }
    // return 0; //coloquei 1 mais para tirar a linha vermelha
// }
// 
// std::ostream& operator<<(std::ostream& output, const SocketS& instance){
    // output << "SocketS Info:" << std::endl;
    // output << "WebServ Socket : " << instance.getFD() << std::endl;
    // output << "Socket FD: " << instance.getFD() << std::endl;
    // return (output);
// }


    /*
     if(bind(server_socket,(struct sockaddr*)&server_address, sizeof(server_address)) == -1){
        std::cerr << "Erro ao vincular socket: " << std::endl;
        close(server_socket);
        return 1;
    }
    
    //habilitar o socket para aguardar conexões de entrada. ) 5 representa o tamanho máximo da fila de conexões pendentes.
    if(listen(server_socket, 5) == -1){
        std::cerr << "Erro ao ouvir socket: " << std::endl;
        close(server_socket);
        return 1;
    }
    
    */
    
    
    

    // aqui vão ser chamados os métodos (que serão criados ainda)
    // pra parseamento do arquivo de configuração
    // e armazenamento de seus dados importantes

    /*cria socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1){
        std::cerr << "Erro ao criar socket: " << std::endl;
        return 1;
    }
    //configura endereço do servidor e inicializa os campos da estrutura com 0
    struct sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080); 
    server_address.sin_addr.s_addr = INADDR_ANY;
    */
 
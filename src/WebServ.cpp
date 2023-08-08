/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/08 15:37:43 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/WebServ.hpp"
#include "../inc/ConfigParser.hpp"

WebServ::WebServ(){}

WebServ::~WebServ(){}
                
WebServ::WebServ(std::string filename){

    std::ifstream fileToParse;
    fileToParse.open(filename.c_str());
    if(fileToParse.is_open()){
        std::string line;
        bool isLocationBlock = false;
        while(getline(fileToParse, line)){
            if (line.find("listen") != std::string::npos) {
                _configParser.processListen(line);
            } else if (line.find("server_name") != std::string::npos) {
                _configParser.processServerName(line);
            } else if (line.find("root") != std::string::npos) {
                _configParser.processRoot(line);
            } else if (line.find("location") != std::string::npos || isLocationBlock == true){
                isLocationBlock = true;
                _configParser.processLocation(line);
                if (line.find("}") != std::string::npos)
                    isLocationBlock = false;
            }
        } 
    }else
        std::cout << "[Error] : file cannot be opened" << std::endl;
    fileToParse.close();
    mainLoop();
}               

void WebServ::configSocket(){
   
    _serverSocket.setPort(_configParser.getPort());
    _serverSocket.setAddress(_configParser.getAddress());
    
    //cria socket
    _serverSocket.setWebServSocket(socket(AF_INET, SOCK_STREAM, 0));//int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(_serverSocket.getWebServSocket() == -1){
        throw WebServException();
    }
    
    //configura endereço do servidor e inicializa os campos da estrutura com 0
    struct sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET; //socket usará os ends. IPv4
    server_address.sin_port = htons(std::atoi(_configParser.getPort().c_str())); //usa a função htons para converter8080 para a ordem de bytes da rede e atribui a sin_port
    server_address.sin_addr.s_addr = INADDR_ANY; //especifica o end.IP que o socket do server será vinculado

    //chamada para o bind - vincula o socket ao endereço e porta, 0 -1 tem haver com a falha na chamada do bind
    if(bind(_serverSocket.getWebServSocket(),(struct sockaddr*)&server_address, sizeof(server_address)) == -1){
        close(_serverSocket.getWebServSocket());
        throw WebServException();
    }
    
    //habilitar o socket para aguardar conexões de entrada. ) 5 representa o tamanho máximo da fila de conexões pendentes.
    if(listen(_serverSocket.getWebServSocket(), 5) == -1){
        close(_serverSocket.getWebServSocket());
        throw WebServException();
    }
    
}

void WebServ::printRequest(const std::string& request){
    std::istringstream iss(request);//permite tratar a string request como um fluxo de entrada
    std::string line;
    while(std::getline(iss, line) && line != "\r"){
        std::cout << line << std::endl;
    }
}

void WebServ::mainLoop(){

    configSocket();

    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Servidor iniciado. Aguardando conexões..." << std::endl;
    std::cout << "-----------------------------------------\n" << std::endl;

    //abre o loop principal. Aceita novas conexões
    while(true){
        struct sockaddr_in client_address = {0};
        _clientAddressLength = sizeof(client_address);
        _clientSocket = accept(_serverSocket.getWebServSocket(), (struct sockaddr *)&client_address, &_clientAddressLength);
        if(_clientSocket == -1){
            std::cerr << "Erro ao aceitar a conexão" << std::endl;
            continue;
        }
        //Recebe solicitação
        char buffer[1024];
        ssize_t bytesRead = recv(_clientSocket, buffer, sizeof(buffer), 0); //peguei a request
        if(bytesRead <= 0){
            std::cerr << "Erro ao receber solicitação do client " << std::endl;
            continue;
        }
        std::string request(buffer, bytesRead); //buffer contém os dados recebidos do client
        printRequest(request);
        //parceamento da request
            // ver um tipo do metodos
            // ver o que ele ta querenso se [e um html, style, arquivo..
            // constroi a response
            // envia a response
        
        // ---- ESSA PARTE NAO ENTRA DESSA FORMA, AQUI CONTEM POSTAS PARA DAR CONTINUIDADE ---
        // size_t found = request.find("/style.css"); //
        // //se ele nao tiver /style.css na request entao sera npos, significa que ele não encontrou
		// if (found == std::string::npos){
		// 	const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><head><link rel='stylesheet' href='style.css'></head><body><h1>Hello World</h1></body></html>";
		// 	ssize_t bytesSent = send(_clientSocket, response, strlen(response), 0); //começa sempre pelo metodo: envia a reponse para o clienteSocket e retorna a quantidade de bytes.
		// 	if (bytesSent == -1){
		// 		std::cerr << "Erro ao enviar a resposta ao cliente" << std::endl;
		// 	}
		// }
		// else{ //esse bloco acontece quando encontrar o /style.css
		// 	std::string cssContent = readCssFile("style.css");
		// 	std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\n\r\n";
		// 	response += cssContent;
		// 	ssize_t bytesSent = send(_clientSocket, response.c_str(), response.size(), 0);
		// 	if (bytesSent == -1)
		// 		std::cerr << "Erro ao enviar a resposta ao cliente" << std::endl;
        // }
        // --- FIM DA PARTE QUE NAO ENTRA DESSA FORMA  ---
        
        
		// Fecha o socket do client  >> se não fechar antes fica no loop
		close(_clientSocket);
        std::cout << "\n-----------------------------------------" << std::endl;
		std::cout << "----- FECHOU A CONEXÃO COM O CLIENTE ----" << std::endl;
        std::cout << "-----------------------------------------" << std::endl;
    }
    //fecha o socket do server
    close(_clientSocket);
    return;
}

       
/*
                    size_t posIniIP = line.find_first_of("0123456789.:", posListen, + 6); // 6 para pular a palavra listen
                if(posIniIP != std::string::npos){
                    size_t posFinIP = line.find_first_of(" \t\n\r\f\v;", posIniIP);
                    std::string ipAddress = line.substr(posIniIP, posFinIP - posIniIP); // Extrai o endereço IP
    
                    if(ipAddress.size() >= 3 && ipAddress.substr(ipAddress.size() - 3) == ":80") // Remove o ":80"
                        ipAddress = ipAddress.substr(0, ipAddress.size() - 3);
                        
                    if(!foundDefaultServer){
                        _serverSocket.setAddress(ipAddress);
                        std::cout << "[TESTE]Endereço IP: " << ipAddress << std::endl;
                        //foundDefaultServer = true;
                    }
                }
                    
                size_t posPort = line.find("80");
                if(posPort != std::string::npos){
                    std::string eighty = line.substr(posPort, 2);
                    int intPort = std::atoi(eighty.c_str());
                    std::cout << "[TESTE]Posição da Porta: " << posPort << std::endl;
                    std::cout << "[TESTE]Porta: " << eighty << std::endl;
                    std::cout << " --------------------------------- " << std::endl;
                    _serverSocket.setPort(intPort);
                    //_serverSocket.setAddress("0.0.0.0");
                }else
                    _serverSocket.setPort(80);
            }
            // continua para segunda linha
            size_t posServerName = line.find("server_name");
            if(posServerName != std::string::npos){
                std::cout << " --------------------------------- " << std::endl;
                std::cout << "[TESTE]Posição server_name: " << line << posServerName << std::endl;
            }//fim do if maior
        }// final do while
        std::cout << "[TESTE] ENDEREÇO ARMAZENADO: "<< _serverSocket.getAddress() << "\n";
        std::cout << "[TESTE] PORTA ARMAZENADA: "<< _serverSocket.getPort() << "\n";
        fileToParse.close();
    }else //final if de abertura do arquivo
        std::cout << "Erro ao abrir" << std::endl;
}


*/
    

/*
    g++ -std=c++98 -I inc/ src/main.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp -o executavel
    ./executavel src/config.txt

*/
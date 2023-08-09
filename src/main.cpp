/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 17:24:52 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/09 16:14:06 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstdlib>
#include "../inc/WebServ.hpp"



 int main(int argc, char **argv){
    std::string filename(argv[1]);
    if(argc != 2){
        std::cerr << "Error" << std::endl;
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
    g++ -std=c++98 -I inc/ src/main.cpp src/WebServ.cpp src/SocketS.cpp src/ConfigParser.cpp -o executavel
    ./executavel src/config.txt

*/


/*
//entrar na configuração do nginx e ver o arquivo
as validações não devem ser feitas aqui e sim dentro do programa:

    verificar se 
    
    existe se tem arquivo
    se tem conteudo
    se o conteudo condiz com aquilo que é solicitado
    

*/







/*


#include <iostream>
#include <sys/socket.h> //para funções relacionadas a socket
#include <netinet/in.h> //para estrutura de endereço de internet
#include <unistd.h> //para a função close
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>


//    cria socket
//    configura endereço do servidor
//    limpa a estrutura antes de configurar para garantir o valor inicial aos campos ou inicializa todos os valores com 0
//    vincula o socket ao enderço do servidor
//    aguarda conexões
//    habilita socket para aguardar conexões de entrada
//    abre o loop principal para aceitar novas conexões
//    abre a solicitação do client
//    print na solicitação do client
//
//
//    AF_INET : é o dominio neste caso a familia de endereços IPv4
//    SOCK_STREAM : é o tipo usado na comunicação confiável e orientada a conexão cpo o TCP
//    o 0 é quando o sistema escolhe o próprio protocolo automaticamente com base na familia do tipo socket
//    INADDR_ANY : representa todos os endereços IPs disponiveis no sistema
//    htons() para converter o valor da porta 8080 para a ordem de bytes da rede (big-endian) e, em seguida, atribuindo-o ao campo sin_port
//
//
//    a estrutura sockaddr_in:
//    
//    struct sockaddr_in {
//    short sin_family;           // Família de endereços (ex: AF_INET)
//    unsigned short sin_port;    // Número da porta (em ordem de bytes da rede)
//    struct in_addr sin_addr;    // Endereço IP (em ordem de bytes da rede)
//    char sin_zero[8];           // Preenchimento para ser do mesmo tamanho que struct sockaddr
//    };


// le o conteudo de um arquivo Css e retorna o conteudo como uma string
// filename representa o nome do Css que será lido
std::string readCssFile(const std::string& filename){
    std::ifstream file(filename);
    std::stringstream buffer; //tipo especial que trata umasequencia de caracteres como se fosse uma stream de entrada
    buffer << file.rdbuf(); //aqui o conteudo de file é lido e copiado para buffer
    return buffer.str(); //o conteudo lido é retornado como resultado
}

void printRequestHeaders(const std::string& request){
    std::istringstream iss(request);//permite tratar a string request como um fluxo de entrada
    std::string line;
    while(std::getline(iss, line) && line != "\r"){
        std::cout << line << std::endl;
    }
}

int main(void){

    //cria socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1){
        std::cerr << "Erro ao criar socket: " << std::endl;
        return 1;
    }
    //configura endereço do servidor e inicializa os campos da estrutura com 0
    struct sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET; //socket usará os ends. IPv4
    server_address.sin_port = htons(8080); //usa a função htons para converter8080 para a ordem de bytes da rede e atribui a sin_port
    server_address.sin_addr.s_addr = INADDR_ANY; //especifica o end.IP que o socket do server será vinculado
    
    //chamada para o bind - vincula o socket ao endereço e porta, 0 -1 tem haver com a falha na chamada do bind
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
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Servidor iniciado. Aguardando conexões..." << std::endl;
    std::cout << "-----------------------------------------\n" << std::endl;

    //abre o loop principal. Aceita novas conexões
    while(true){
        struct sockaddr_in client_address = {0};
        socklen_t client_address_length = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_length);
        if(client_socket == -1){
            std::cerr << "Erro ao aceitar a conexão" << std::endl;
            continue;
        }
        //Recebe solicitação
        char buffer[1024];
        ssize_t bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
        if(bytesRead <= 0){
            std::cerr << "Erro ao receber solicitação do client " << std::endl;
            continue;
        }
        std::string request(buffer, bytesRead); //buffer contém os dados recebidos do client
        printRequestHeaders(request);
        
        size_t found = request.find("/style.css");
        //se ele nao tiver /style.css na request entao sera npos, significa que ele não encontrou
		if (found == std::string::npos){
			const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><head><link rel='stylesheet' href='style.css'></head><body><h1>Hello World</h1></body></html>";
			ssize_t bytesSent = send(client_socket, response, strlen(response), 0); //começa sempre pelo metodo: envia a reponse para o clienteSocket e retorna a quantidade de bytes.
			if (bytesSent == -1){
				std::cerr << "Erro ao enviar a resposta ao cliente" << std::endl;
			}
		}
		else{ //esse bloco acontece quando encontrar o /style.css
			std::string cssContent = readCssFile("style.css");
			std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\n\r\n";
			response += cssContent;
			ssize_t bytesSent = send(client_socket, response.c_str(), response.size(), 0);
			if (bytesSent == -1)
				std::cerr << "Erro ao enviar a resposta ao cliente" << std::endl;
        }
		// Fecha o socket do client  >> se não fechar antes fica no loop
		close(client_socket);
        std::cout << "\n-----------------------------------------" << std::endl;
		std::cout << "----- FECHOU A CONEXÃO COM O CLIENTE ----" << std::endl;
        std::cout << "-----------------------------------------" << std::endl;
    }
    //fecha o socket do server
    close(server_socket);
    return 0;
}




*/



// listen 127.0.0.1:80;
// server_name example.com "";
// 
// listen 127.0.0.1:8080;
// server_name */8080 "";
// 
// 
// listen
// listen 80;
// listen 8080;
// listen 80 default_server;
// listen 443 ssl;
// listen *:443 ssl;
// listen 127.0.0.1;
// listen yourdomain.com;
// listen *:80;
// listen 127.0.0.1:8080;
// listen 192.168.1.100:80;
// listen localhost:80;
// listen yourdomain.com:80;
// listen 192.168.1.100:443 ssl;
// listen http://localhost.com;
// listen http://localhost.com:80;
// listen 443 ssl;
// 
// 
// server_name example.com;
// server_name example.com www.example.com;
// server_name *.example.com;
// server_name ~^www\.example\.(com|org)$;
// server_name example.com/path;
// server_name example.com "";
// server_name "";

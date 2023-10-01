/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SimpleSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/17 22:13:15 by cleticia          #+#    #+#             */
/*   Updated: 2023/07/18 17:32:39 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_CONNECTIONS 5

class Socket {
private:
    int server_socket;
    int client_socket;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    char buffer[1024];
    const char *response;

public:
    Socket() {
        server_socket = -1;
        client_socket = -1;
        client_len = sizeof(client_addr);
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 14\r\n\r\nHello, client!";
        memset(buffer, 0, sizeof(buffer));
    }

    bool create() {
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        return server_socket != -1;
    }

    bool bind() {
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(PORT);
        return ::bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) != -1;
    }

    bool listen() {
        return ::listen(server_socket, MAX_CONNECTIONS) != -1;
    }

    bool accept() {
        client_socket = ::accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        return client_socket != -1;
    }

    void respond() {
        send(client_socket, response, strlen(response), 0);
    }

    void printRequest() {
        int bytesRead = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << "Request from client: " << buffer << std::endl;
        }
    }

    void close() {
        if (server_socket != -1) {
            ::close(server_socket);
            server_socket = -1;
        }
        if (client_socket != -1) {
            ::close(client_socket);
            client_socket = -1;
        }
    }
};

int main() {
    Socket server_socket;

    if (!server_socket.create()) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    if (!server_socket.bind()) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    if (!server_socket.listen()) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    while (true) {
        if (server_socket.accept()) {
            server_socket.printRequest(); // Imprimir a requisição do cliente
            server_socket.respond();
        } else {
            perror("Error accepting connection");
            server_socket.close(); // Fechar o socket do cliente em caso de erro na conexão
        }
    }

    return 0;
}


// g++ SimpleSocket.cpp -o SimpleSocket
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdint> // Para incluir uint16_t e in_addr_t

int main() {
    in_addr_t host = inet_addr("127.0.0.1"); // Endereço IP localhost
    uint16_t port = 4003;                      // Porta 80

    // Criar o primeiro socket
    int socket1 = socket(AF_INET, SOCK_STREAM, 0);
    if (socket1 == -1) {
        perror("Erro ao criar o primeiro socket");
        exit(EXIT_FAILURE);
    }

    // Configurar a estrutura de endereço para o primeiro socket
    struct sockaddr_in server1_addr;
    memset(&server1_addr, 0, sizeof(server1_addr));
    server1_addr.sin_family = AF_INET;
    server1_addr.sin_port = htons(port);
    server1_addr.sin_addr.s_addr = host;

    // Vincular o primeiro socket
    if (bind(socket1, (struct sockaddr*)&server1_addr, sizeof(server1_addr)) == -1) {
        perror("Erro ao vincular o primeiro socket");
        close(socket1);
        exit(EXIT_FAILURE);
    }

    std::cout << "O primeiro socket foi vinculado com sucesso!" << std::endl;

    // Criar o segundo socket da mesma maneira
    int socket2 = socket(AF_INET, SOCK_STREAM, 0);
    if (socket2 == -1) {
        perror("Erro ao criar o segundo socket");
        close(socket1); // Feche o primeiro socket se ocorrer um erro
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server2_addr;
    memset(&server2_addr, 0, sizeof(server2_addr));
    server2_addr.sin_family = AF_INET;
    server2_addr.sin_port = htons(port);
    server2_addr.sin_addr.s_addr = host;

    if (bind(socket2, (struct sockaddr*)&server2_addr, sizeof(server2_addr)) == -1) {
        perror("Erro ao vincular o segundo socket");
        close(socket1); // Feche o primeiro socket se ocorrer um erro
        close(socket2); // Feche o segundo socket se ocorrer um erro
        exit(EXIT_FAILURE);
    }

    std::cout << "O segundo socket foi vinculado com sucesso!" << std::endl;

    // Mantenha os sockets abertos ou faça a lógica do servidor aqui

    return 0;
}

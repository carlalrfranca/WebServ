/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 21:24:02 by cleticia          #+#    #+#             */
/*   Updated: 2023/07/31 19:10:37 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ConfigParser.hpp"
#include <cctype> //Para usar a função std::isspace

ConfigParser::ConfigParser(){
    _line = ""; //talvez, mas acho que nao precisa mesmo
    _domain = ""; //server_name
    //_ssl = ""; //ssl_certificate
    _key = ""; //ssl_certificate
    _rules = ""; //location
    _nPos = std::string::npos;
    _directive = 0;
    _port = "";
    _ipAddress = "";
    _httpAddress = "";
    _path = "";
    _posInit = 0;
    _posEnd = 0;


}

ConfigParser::~ConfigParser(){}





void ConfigParser::processListen(std::string& line, WebServ& wsManager){

    //bool foundDefaultServer = false;
    _directive = line.find("listen");
    if(_directive != _nPos){
        _directive += 6; //avança para além da palavra "listen"
        _posInit = line.find_last_of(":", _directive); // aqui verifica se tem os dois pontos e 6 para pular a palavra listen
        if(_posInit != _nPos){ //sse ele encontrou
            _ipAddress = line.substr(_directive, _posInit - (_directive)); //aqui coloquei o conteudo antes dos :
            size_t firstNonSpace = _ipAddress.find_first_not_of(" \t");
            _ipAddress = _ipAddress.substr(firstNonSpace, _ipAddress.length() - firstNonSpace);
            if(_ipAddress == "http" || _ipAddress == "https" ){
                std::string rest = line.substr(_posInit, (line.length() - _posInit));
                _ipAddress += rest; // nao tem porta demarcda no listen -> armazena o valor default na porta
                _port = "80"; //default
            }else{
                _port = line.substr(_posInit + 1); //extrai o conteudo depois de : e coloca no atributo _port
                
                //--------------------- cout para teste -----------------------------
                std::cout << "IP Address: " << _ipAddress << std::endl;
            std::cout << "Port: " << _port << std::endl;
            }
        }else{ //se ele enao encontrou : entao ele vai armazenar tudo dentro de  _ipAddress e _port e entao vou ter que verificar se tem ssl ou default_server;
            _posInit = line.find(".", _directive); // aqui verifica se tem os pontos e 6 para pular a palavra listen
            if(_posInit != _nPos){ //ou seja, se ele encontrou o ponto
                _ipAddress = line.substr(_directive); // coloca o conteudo em _ipAddress
                _port = "80";
            }else{ //se ele nao encontrou ponto, 
                _port = line.substr(_directive); //coloca o conteudo em _port
                _ipAddress = "0.0.0.0";
                
                //--------------------- cout para teste -----------------------------
                std::cout << "IP Address: " << _ipAddress << std::endl;
                std::cout << "Port: " << _port << std::endl;                
            }
        }//aqui ele trata o conteudo de _ipAddress e _port  >> listen 80 default_server;  listen 443 ssl;
        size_t _spaceDot = _ipAddress.find_first_of(" ;");
        if(_spaceDot != _nPos){
            std::string _eighty = _ipAddress.substr(0, _spaceDot); //extrai a parte numérica (80)
            std::string _default_server = _ipAddress.substr(_spaceDot + 1); //extrai a parte do nome
            _port = _eighty;
            _ipAddress = _default_server;
            //--------------------- cout para teste -----------------------------
            std::cout << "IP Address: " << _ipAddress << std::endl;
            std::cout << "Port: " << _port << std::endl; 
        }          
    }
    _directive = line.find("server_name");
        if(_directive != _nPos){
            _directive += 11; //avança para além da palavra "server_name"
            _posInit = line.find_first_of("*", _directive); //busca para os casos de wildcard
            if(_posInit != _nPos){ //se ele encontrou
                size_t _posSemicolon = line.find_first_of(";", _posInit); //estrai do asterisco até antes do ;
                if(_posSemicolon != _nPos){
                    _domain = line.substr(_posInit, _posSemicolon - (_posInit)); //se quiser extrair sem * _domain = line.substr(_posInit + 1, _posSemicolon - (_posInit + 1));
                    std::cout << "Domain: " << _domain << std::endl; //pra TESTE
                }
            }else if(_directive != _nPos){
                _posInit = line.find_first_of("~", _directive); //busca para os casos de expressao regular
                if(_posInit != _nPos){ //se ele encontrou
                    size_t _posSemicolon = line.find_first_of(";", _posInit); //estrai do asterisco até antes do ;
                    if(_posSemicolon != _nPos){
                        _domain = line.substr(_posInit, _posSemicolon - (_posInit)); //se quiser extrair sem ~ _domain = line.substr(_posInit + 1, _posSemicolon - (_posInit + 1));
                        std::cout << "Domain: " << _domain << std::endl; //pra TESTE
                    }
                }
            }else if(_directive != _nPos){
                _posInit = line.find_first_of("/", _directive); //busca para os casos de caminhos
                if(_posInit != _nPos){ //se ele encontrou
                    size_t _posSemicolon = line.find_first_of(";", _posInit); //estrai do asterisco até antes do ;
                    if(_posSemicolon != _nPos){
                        _domain = line.substr(_posInit, _posSemicolon - (_posInit));
                        std::cout << "Domain: " << _domain << std::endl; //pra TESTE
                    }
                
                }
            }else if(_directive != _nPos){
                size_t _posDot = line.find(".", _directive); //busca para os casos de caminho vazio de dominio
                size_t _posSpace = line.find(" ", _directive); //busca para os casos de caminho vazio de dominio
                size_t _posSemicolon = line.find_first_of(";", _directive); //estrai do asterisco até antes do ;
                if(_posDot != _nPos && _posSpace != _nPos && _posSemicolon != _nPos){ //se ele encontrou_domain = line.substr(_posSpace + 1, _posDot - (_posSpace + 1));
                    _domain = line.substr(_posSpace + 1, _posSemicolon - (_posSpace + 1));

                    std::cout << "Domain: " << _domain << std::endl; //pra TESTE
                }
            }else{ //ou seja não encontrou nada e é defaut
                _domain = "default";
                std::cout << "Domain: " << _domain << std::endl; //pra TESTE
            }
            
        }
                //é default    
        // "*" asterisco: caso de wildcard sundominio
        // "~" caso de expressão regular
        // "/" casode caminhos
        // "." && "": caso de caminho vazio de dominio
        // ""  espaço: caso default
}




/*

    server_name example.com; //nome de dominio
    server_name example.com www.example.com;  // multiplos nomes de dominio
    server_name *.example.com;  // Wildcard (Coringa) em subdomínio
    server_name ~^www\.example\.(com|org)$;  //expressões regulares para dominio
    server_name example.com/path;  //caminho específico de dominio
    server_name example.com ""; // caminho vazio em um dominio
    server_name ""; // vazio default

     essa parte pode ter como delimitador:
     
        ""  espaço: casod default
        "." && "": caso de caminho vazio de dominio
        "*" asterisco: caso de wildcard sundominio
        "~" caso de expressão regular
        "/" casode caminhos
        else 
           caso de dominios
    else
        linha de configuração inválida
     

*/

/*

    std::string input = "80 default_server;";

    // Encontrar a posição do primeiro caractere que não seja um dígito (ou seja, o espaço ou ponto-e-vírgula)
    size_t pos = input.find_first_of(" ;");

    if (pos != std::string::npos) {
        // Extrair a parte numérica (80)
        std::string numero = input.substr(0, pos);

        // Extrair a parte do nome (default_server)
        std::string nome = input.substr(pos + 1);

        std::cout << "Número: " << numero << std::endl;
        std::cout << "Nome: " << nome << std::endl;
    } else {
        // Caso a string de entrada esteja mal-formada ou não contenha espaço ou ponto-e-vírgula
        std::cout << "Formato inválido da string de entrada." << std::endl;
    }

    return 0;
}




/*


        }
        size_t posPort = line.find("80");
        if(posPort != _invalidPos){
            std::string eighty = line.substr(posPort, 2);
            int intPort = std::atoi(eighty.c_str());
            wsManager._serverSocket.setPort(intPort);
            wsManager._serverSocket.setAddress("0.0.0.0");
        }else
            wsManager._serverSocket.setPort(80);

    }


    /*
        listen
        
        listen 80;
        listen 8080;
        listen 80 default_server;
        listen 443 ssl;
        listen *:443 ssl;

        
        
        // ---nem ponto e nem dois pontos
        listen 127.0.0.1;
        listen yourdomain.com;
        listen *:80;
        listen 127.0.0.1:8080;
        listen 192.168.1.100:80;
        listen localhost:80;
        listen yourdomain.com:80;
        listen 192.168.1.100:443 ssl;
        listen http://localhost.com;
        listen http://localhost.com:80;
        
        
        
        
        listen 443 ssl;
Exemplo 4 - Escutando em uma interface IP específica e porta com SSL (HTTPS - porta 443):

arduino
Copy code
listen 192.168.1.100:443 ssl;

    */

/*


///////////////////////////////////////////////////


    #include <iostream>
#include <string>
#include <cctype> // Para usar a função std::isspace



void ConfigParser::processListen(std::string& line, WebServ& wsManager){

    //bool foundDefaultServer = false;
    _directive = line.find("listen");
    if(_directive != _nPos){
        _directive += 6; //avaça para além da palavra "listen"
        
        while(_directive < line.length() && std::isspace(line[_directive])) //remove espaços e tabulações depois de listen
            _directive++;
            
        _posEnd = line.find_first_of(" \t;", _directive); // Encontra a próxima posição com espaço, tabulação ou ponto e vírgula
        
        if(_posEnd != _nPos){
            _ipAddress = line.substr(_directive, _posEnd - _directive); // extrai o conteúdo antes dos ":" e coloca em _ipAddress

            if(_http == _nPos || _https == _nPos){
                _posInit = line.find_last_of(":", _posEnd); //busca a ultima posição de : na string line, começando a partir da posição _posEnd
                if (_posInit != _nPos){
                    _port = line.substr(_posInit + 1); //extrai o conteudo depois de : e coloca no atributo _port
                    std::cout << "IP Address: " << _ipAddress << std::endl;
                    std::cout << "Port: " << _port << std::endl;
    
                }
            } else {
                //aqui encontrou  http pu https antes do enredeço IP
                std::cout << "Aqui encontrou o end http, entao devemos fazer o tratamento dele aqui" << std::endl;
            }
        }
    }
}




*/

}

void ConfigParser::processServerName(std::string& line){

}

void ConfigParser::processHost(std::string& line){

}

void ConfigParser::processLocation(std::string& line){
}

void ConfigParser::applyDefaultValues(void){

}


/*

///////////////////////////////////////////////////////////////////////////
exemplo:

server {
    listen        80;
    server_name   .empresa1.com;
    ...
}
server {
    listen        80;
    server_name   .empresa2.com;
    ...
}
server {
    listen        80;
    server_name   .empresa3.com;
    ...
}
server {
    listen        127.0.0.1;
    server_name   localhost;
    ...
}
////////////////////////////////////////////////////////////////////////////////////////////////////////

propriedade

port se tiver : 

string ipAddress . . . 
int portNumber

primeiro valida que é numero
o que estiver depois passa atoi e armazena em portNumber
o que está antes do dois numeros precisa validar a string de acordo com formato de endereço IP e dai armazena ipAddress que deverá ser mantido como string
Se tiver apenas o IP então considero como porta 80 o default e armazeno o valor 80 +em portNumber
Se tiver apenas a porta então considero como IP 0.0.0.0 e armazeno esse valor em ipAddress

serverName[] vetor de strings

Ao receber um resquest ele vai iterar pelo vetor e verificar o ipAddress e portNumber
Ele deve escolher o server mais adequado para a requisicao ele vai compara o ip porta da requisicao com o ip e porta do vertor de servidores[sockets]
é nele que vai aceitar a conexao do client

Se contabilizar mais que 1 server adedaquado entao devera analisar da propriedade server_name.
Voce compara o server_name com o host que é uma linha que virá da request
Se encontrar um server então para e é ele que deverá aceitar a conexão.

Se vários sockets tiverem listen e server_name iguais entoa esoclhe-se o primeiro listado.
e os demais vetores deverão apresentar a mensagem "nginx: [warn] conflicting server name “.empresarepetida.com” on 0.0.0.0:80, ignored"
na inicialização do nginx


Caso de host desconhecido

Se hostRequest for != serverName
Verificar se algum socket tem a flag marcada como default_server
(campo listen de um deles possuia "default_server") 
no caso positivo deverá ir para o socket especificado como default 
[pode criar uma flag para definir se foi marcado como defaultServer]
a conexao devera ser aceita pelo primeiro Socket que contenha Ip e porta iguais 
ao Ip e porta do campo host da Request

o capo listen é do arquivo de configuracao e correpsonde aos atributos da classe Socket
ipAddress e portNumber;

arquivo de configuração

 arquivo deve preencher os atributos de Scket/Server
 e depois o conteudo da request deve ser ocmparado com as variaveis já preenchidas

*/

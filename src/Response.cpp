const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><head><link rel='stylesheet' href='style.css'></head><body><h1>Hello World</h1></body></html>";/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/18 18:00:34 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/30 20:40:11 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HeadersLibs.hpp"
#include "../inc/Response.hpp"
#include "../inc/Request.hpp"

Response::Response()
{
    _chosenSocket = NULL;
}

Response::~Response()
{}

Response::Response(Request request)
{
    //define corpo da resposta lendo o conteudo do arquivo html
    // _body = readHtmlFile("web/html_upload_arquivo/index.html");
    
    // precisa definir os campos do cabeçalho aqui, entao se for inserir mais É AQUI
    //setStatusCode("200 OK");
    //setContentType("text/html");
    //setDateAndTime();
    //setContentLength(_body.length());
    _chosenSocket = NULL;
}

//metodo para ler o conteudo do html
std::string Response::readHtmlFile(const std::string& filePath)
{
    std::ifstream file(filePath.c_str());
    if(!file.is_open())
        return""; //retorna string vazia por enquanto se o arquivo nao pode ser lido
    std::string content; //ler caractere a caractere e add a string content
    char character;
    while(file.get(character))
        content += character;
    file.close();
    return content;
}

void Response::setStatusCode(const std::string& statusCode)
{
    _headers["Status"] = statusCode;
}

void Response::setContentType(const std::string& contentType)
{
    _headers["Content-Type"] = contentType;
}

void Response::setDateAndTime()
{
    char buffer[80];
    time_t rawTime;
    struct tm *timeInfo;
    
    time(&rawTime);
    timeInfo = localtime(&rawTime);
    
    //string info 
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", timeInfo);
    _headers["Date"] = buffer;
}

// void Response::setContentLength(size_t length)
// {
//     _headers["Content-Length"] = to_string(length); //***   contentLength(_response.length());
// }

//void Response::setResponse(const std::string& response)
//{}

const std::string& Response::getHeader(const std::string& header)const
{
    static std::string emptyHeader = ""; // Cria uma string vazia para retorno padrão

    // Implementação para buscar e retornar o cabeçalho específico
    // Se o cabeçalho não existir, retorne a string vazia criada acima

    return emptyHeader;
}

void Response::setResponse(const std::string& response)
{
    _response = response; //Define o corpo da resposta
}

bool Response::contains(const std::vector<std::string>& vec , const std::string& content)
{
    for(size_t i = 0; i < vec.size(); ++i)
    {
        if(vec[i] == content)
            return true;
    }
    return false;
}


// refazer esse metodo selectServer




/*






Na configuração que você forneceu, onde existem dois servidores virtuais ouvindo em portas diferentes, mas com o mesmo nome de servidor, 
a ordem de verificação será a seguinte:

Verificação da Porta e Endereço IP: O Nginx primeiro verificará a combinação de porta e endereço IP da solicitação recebida. 
Ele tentará corresponder a combinação de porta e IP com a configuração dos servidores virtuais. No seu caso:

Se a solicitação for recebida em localhost (127.0.0.1) na porta 8034, ela será direcionada para o primeiro servidor virtual.
Se a solicitação for recebida no endereço IP 142.0.0.1 na porta 8033, ela será direcionada para o segundo servidor virtual.

Verificação do Campo "Host" (Nome do Servidor): Se a solicitação for direcionada para um servidor virtual com base na porta e no IP, 
o Nginx, em seguida, verificará o campo "Host" (nome do servidor) no cabeçalho da solicitação. Ele comparará o valor do campo "Host" 
com os nomes de domínio configurados (server_name) para determinar qual servidor virtual deve lidar com a solicitação.

No seu caso, ambos os servidores virtuais têm o mesmo nome de servidor (exemplo.net e www.exemplo.net), então o Nginx usará o campo "Host" no cabeçalho da solicitação para escolher qual servidor virtual deve ser usado.

Escolha do Servidor Virtual: O Nginx selecionará o primeiro servidor virtual que corresponda ao valor do campo "Host". Portanto, mesmo que os servidores virtuais tenham endereços IP diferentes, se o campo "Host" for exemplo.net ou www.exemplo.net, a solicitação será direcionada para o primeiro servidor virtual que você definiu.
Em resumo, a ordem de verificação é:

Verificação da combinação de porta e endereço IP.
Verificação do campo "Host" (nome do servidor) para selecionar o servidor virtual correspondente.
Certifique-se de configurar os servidores virtuais com nomes de domínio distintos se você desejar que diferentes endereços IP e portas sejam tratados de forma distinta.




*/
int Response::selectServer(Request& stringDomain, std::vector<SocketS> serverSocket)
{
    int indexChosenSocket = -1;
    for (size_t serverIndex = 0; serverIndex < serverSocket.size(); ++serverIndex)
    {
        std::cout << "----- DENTRO DO SELECT SERVER ------------" << std::endl;
        std::cout << "Dominio da request: " << stringDomain.getDomainRequest() << "| Size: " << stringDomain.getDomainRequest().size() << std::endl;
        std::cout << "Address do servidor: " << serverSocket[serverIndex].getAddress() << "| Size: " << serverSocket[serverIndex].getAddress().size() << std::endl;
        std::cout << "Porta do servidor: " << serverSocket[serverIndex].getPort() << " | Size: " << serverSocket[serverIndex].getPort().size() << std::endl;
        std::cout << "Porta da request: " << stringDomain.getPortRequest() << " | Size: " << stringDomain.getPortRequest().size() << std::endl;
        
        if (stringDomain.getDomainRequest() == serverSocket[serverIndex].getAddress())
        {
            std::cout << "Dominio igual!" << std::endl;
            //vê a porta
            if (stringDomain.getPortRequest() == serverSocket[serverIndex].getPort())
            {
                std::cout << "Porta igual!!!!!" << std::endl;
                // precisa verificar o sevrer_name nesse caso?
                //se for tudo certinho...
                indexChosenSocket = serverIndex; //depois passar isso pra essa condicao abaixo
            }
        }
        
    }
    return indexChosenSocket;
    // construir resposta de erro... (se sair do loop é que deu merda)

/*
    for (size_t serverIndex = 0; serverIndex < serverSocket.size(); ++serverIndex)
    {
        if(stringDomain.getDomainRequest().find(".") != std::string::npos) //ip
        {
            if(stringDomain.getDomainRequest() == serverSocket[serverIndex].getAddress())
            {
                std::cout << "é igual a ipaddress" << std::endl;
                if (stringDomain.getPortRequest().size() > 0)
                {
                    // teve port, daí tem que comparar isso e ver se está configurado no server tambem
                    if(stringDomain.getPortRequest() == serverSocket[serverIndex].getPort())
                    {
                        std::cout << "tem port e é o mesmo";
                        // daí sim prossegue pra construir uma response que pode ser bem sucedida
                        //ou seja: chama um objeto Response e passa essa request e esse serverSocket[...] como parametro
                        //......
                        
                        break;
                    }
                    else
                    {
                        std::cout << "Não é o mesmo port. Então NÃO ROLA" << std::endl;
                        //construir uma response de erro (ver qual é o statusCode condizente e 
                        //como ficará o resto do cabeçalho)
                        // response de erro é STRING               
                    }
                }
            }    
        }
        else if(contains(serverSocket[serverIndex].getServerName(), stringDomain.getDomainRequest()))
        {
          std::cout << "tá no server name" << std::endl;
          if (stringDomain.getPortRequest().size() > 0)
            {
                // teve port, daí tem que comparar isso e ver se está configurado no server tambem
                if(stringDomain.getPortRequest() == serverSocket[serverIndex].getPort())
                {
                    std::cout << "tem port e é o mesmo";
                    // daí sim prossegue pra construir uma response que pode ser bem sucedida
                    //ou seja: chama um objeto Response e passa essa request e esse serverSocket[...] como parametro
                    //.....
                    break;
                }
                else
                {
                    std::cout << "Não é o mesmo port. Então NÃO ROLA" << std::endl;
                    //construir uma response de erro (ver qual é o statusCode condizente e 
                    //como ficará o resto do cabeçalho)
                    // response de erro é STRING
                }
            }
        }
    }
    return "";
*/
}

std::string Response::getResponse()
{
    return _response; //Retorna o corpo da resposta
}

std::string readFileToString(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cout << "Não deu pra abrir o html" << std::endl;
        return ""; // Retorna uma string vazia se não for possível abrir o arquivo
    }

    std::string content;
    std::string line;

    while (std::getline(file, line)) {
        content += line + "\n"; // Adiciona cada linha ao conteúdo da string
    }

    file.close();

    return content;
}

std::string Response::buildResponse(Request &request, SocketS &server)
{
    // Implementação para construir a resposta completa, incluindo cabeçalho e corpo
    // Incluir os cabeçalhos necessários, como Content-Length e outros
    // Retorne a resposta completa construída

    //////////

    
    // verificar se o método requisitado pela solicitação é permitido pra esse servidor
    std::vector<std::string> allowed_methods = server.getMethods();
    std::string requestMethod = request.getMethod();
    
    bool found = false;

    for (std::vector<std::string>::iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it) {
        if (strcmp(it->c_str(), requestMethod.c_str()) == 0) {
            found = true;
            break;
        }
    }

    if (found) {    
        // método é permitido pra esse servidor. Continua...
        
        // temos que verificar se o servidor está apto a lidar com esse recurso (ver os
        // locations...)
        std::map<std::string, LocationDirective> serverLocations = server.getLocations();
        std::map<std::string, LocationDirective>::iterator it = serverLocations.find(request.getURI());

        std::map<std::string, std::vector< std::string > > locationDirectives;
        if (it != serverLocations.end()) {
            std::cout << "Directives from this Location found!" << std::endl;
            locationDirectives = it->second.getDirectives();
             std::map<std::string, std::vector< std::string > >::iterator itIndex = locationDirectives.find("index");

            if (itIndex != locationDirectives.end()) {
                std::cout << "Index found! Value: " << itIndex->second[0] << std::endl;
                // COMPLETAR O CAMINHO DO ARQUIVO COM O ROOT (daí tem que verificar a diretiva root tambem)
                std::string bodyHTML = readFileToString(itIndex->second[0]);
                //std::cout << "---- DEU PRA LER HTML ------";
                //std::cout << bodyHTML << std::endl;
                _body = bodyHTML;
                //std::cout << "-----------------------------------" << std::endl;
            } else {
                std::cout << "Index not found!" << std::endl;
            }
        } else {
            std::cout << "This server doesnt have this location!!" << std::endl;
            std::string response = "HTTP/1.1 404 Not found\r\nContent-Type: text/html\r\n\r\n<html><head></head><body><h1>Error 404</h1></body></html>";
            setResponse(response);
            return response;    
        }                
    }
    else
    {
        //constrói resposta de erro porque esse método não é permitido
        // e retorna
        std::cout << "MÉTODO NÃO PERMITIDO!";
        std::cout << "This server doesnt have this location!!" << std::endl;
        std::string response = "HTTP/1.1 404 Not found\r\nContent-Type: text/html\r\n\r\n<html><head></head><body><h1>Error 404</h1></body></html>";
        setResponse(response);
        return response;    
    }
    //////////

    std::string fullResponse;
    
    // Construa os cabeçalhos aqui, usando os dados em _headers
    
    fullResponse += "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"; // Carla não esquece de adds uma linha em branco entre cabeçalhos e corpo
    fullResponse += _body;  // Adicione o corpo da resposta
    setResponse(fullResponse);
    return fullResponse;
}

void Response::reset()
{
    _headers.clear(); //limpa os cabeçalhos
    _body.clear(); //limpa o corpo
    _code = ""; //limpa o código de status
}

void Response::httpError(std::string errorCode, const std::string &errorMessage)
{
    _code = errorCode;
    _response = errorMessage;
    _headers.clear(); //limpa cabeçalhos anteriores
}


/*





    std::map<std::string, std::string>  _headers;
    std::string                         _body;
    std::string                         _response; //criar uma string response que, será todo esse cabeçalho + body (ver exemplos no chat)
    std::string                         _code;
    
    ------------------------------------------- LETICIA DEIXOU ANOTADO ----------------------------------------------
    criar uma string response que, será todo esse cabeçalho + body (ver exemplos no chat)
    PRA DEIFINIR O BODY (um arquivo html que voce precisa "pegar" do diretorio que temos, por exemplo)
    1. Identificar QUAL é o método sendo pedido pela request e verificar que esse socket ACEITA ESSA MÉTODO (get, por exemplo)
        ---> se NÃO aceitar, construir uma RESPOSTA DE ERRO (com o statusCode adequado) e ARMAZENAR isso na response da classe e retornar (pra interromper o metodo)
    2. SE ACEITAR, daí IDENTIFICAR qual é o RECURSO (URI) pedido pela request e verificar se
         esse recurso está entre os locations (map) desse SERVER SOCKET (qualquer duvida, dar uma olhada no hpp do SocketS)
        ---> Se esse recurso NÃO ESTIVER DENTRE AS LOCATIONS, tem que construir uma RESPOSTA DE ERRO (404?) e armazenar na resposne da classe e retornar (pra interromper o metodo)
    3. SE ESTIVER PRESENTE, acessar o valor (LocationsDirective) desse location no map e indeitificar o arquivo INDEX pedido (está em algum lugar do diretorio) e
        criar um método pra abrir esse arquivo e armazenar seu conteúdo numa string (a string body, no caso)
 precisa devolver essa string de alguma forma (get?) pra mainLoop
 pra que ela possa fazer send() dessa response pro clientSocket

*/



   // Response::Response(){}
   // Response::~Response(){}
   // Response::Response(Request *req, Server *_server){}
   // 
   // //getters and setters
   // size_t Response::getIdClient()const{}
   // const std::string& Response::getHeader(const std::string& header)const{} // busca pelo campo de cabeçalho na lista de cabeçalhos
   // 
   // //response methods
   // void Response::status(int statusCode, const std::string& header)const{} // status da resposta com base no código e no texto fornecidos
   // void Response::text(){} //configuração do corpo da resposta como texto
   // void Response::reset(){} //implementa a redefinição de resposta, limpando cabeçalhos e corpo
   // 
   // //response methods
   // void Response::processResponse(){} //processa a solicitacao, verifica método, URI, etc. e constrói a resposta de acordo com os requisitos
   // void Response::createResponse(std::string requestedContent){}
   // void Response::readRequestedFile(std::string requestedContent){}

/*

Receber uma Request e Identificar o Socket do Servidor:

Quando o servidor recebe uma solicitação (request) de um cliente, ele primeiro analisa essa solicitação.
Para determinar qual servidor socket deve processar essa solicitação, o servidor verifica o cabeçalho Host da solicitação.
(IPADRESS, )
Ele compara o valor do cabeçalho Host (que geralmente é o nome de domínio do site) com os endereços IP, nomes de servidor e portas associados aos seus próprios servidores sockets.
Com base nessa comparação, o servidor identifica o socket específico que deve lidar com essa solicitação.
Passar o Socket e a Request para o Método da Classe Response:

Uma vez que o servidor tenha identificado o socket correto, ele passa o socket e o objeto da solicitação (Request) para um método na classe Response.
Isso permite que o objeto Response saiba qual socket está lidando com a solicitação e possa trabalhar com ele.
Verificar o Método e Validar o Socket:

No método da classe Response, o objeto Response verifica o método (GET, POST, etc.) que foi solicitado na Request.
Também verifica se o socket correspondente suporta esse método. Se não suportar, constrói uma resposta de erro indicando que o método não é permitido.
Se houver um erro, a Response cria uma resposta de erro e retorna essa resposta para a MainLoop.
Comparar a URI (Recurso) com os Locations Disponíveis:

Se o método for suportado pelo socket e não houver erro, a Response continua verificando a URI (recurso) que foi solicitada na Request.
Ela compara essa URI com os possíveis "locations" que esse socket tem gravados.
Os "locations" são mapeados usando um mapa que relaciona URIs a objetos LocationDirectives.
Construir a Resposta Básica:

Se a URI solicitada corresponder a um dos "locations" disponíveis para esse socket, a Response acessa o objeto LocationDirectives correspondente.
Se houver um recurso chamado "index" dentro desse objeto LocationDirectives (representado por uma entrada no mapa), a Response usa o valor associado para localizar o recurso real (por exemplo, "index.html") no diretório.
Ela lê o conteúdo desse recurso e o armazena em uma string.
Enviar a Resposta para o Cliente:

A Response, após ler e processar o conteúdo do recurso, constrói uma resposta de sucesso usando o conteúdo do recurso como o "body" da resposta.
Ela retorna essa resposta construída para a MainLoop.
A MainLoop, por sua vez, envia essa resposta de sucesso para o cliente usando o socket associado.
Esse é um fluxo detalhado de como o servidor identifica o socket, verifica o método, 
compara a URI e constrói uma resposta básica. Lembre-se de que essa é uma descrição 
de alto nível e que a implementação real envolverá várias etapas, verificações e 
tratamento de erros ao longo do processo.



SE FOR NUMERO CONFERE O FORMATO E CONFERE SE É O MESMO QUE ESTA EM IP ADRESS BATE COM O DO LISTEM
SE FOR ESCRITO ENTAO PUXA NO SERVER_NAME
SE TIVER PORTA, SE ELA ESTA PREENCHIDA NA PORT DO SOCKETS
SE NAO TIVER PORTA É DEFAULT ENTAO PRECISA VERFICAR SE ESTÁ HABILITADA
SE DER ALGO ERRADO ENTAO CONSTROI UMA RESPNSE DE ERRO COM O STATUS DE ERRO E RETORNA ISSO PRA MAIN LOOP E A MAIN LOOP FAZ UMA SEND PARA O CLIENT
SE DER CERTO DAI VOCE PROSSEGUE, MANDA O OBJETO REQUEST E ESSE SOCKET QUE ENCONTREI. NESSA FUNCAO QUE TEM
*/

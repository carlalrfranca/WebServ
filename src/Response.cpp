/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/18 18:00:34 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/28 18:51:08 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Response.hpp"

    Response::Response(){}
    Response::~Response(){}
    Response::Response(Request *req, Server *_server){}
    
    //getters and setters
    size_t Response::getIdClient()const{}
    const std::string& Response::getHeader(const std::string& header)const{} // busca pelo campo de cabeçalho na lista de cabeçalhos
    
    //response methods
    void Response::status(int statusCode, const std::string& header)const{} // status da resposta com base no código e no texto fornecidos
    void Response::text(){} //configuração do corpo da resposta como texto
    void Response::reset(){} //implementa a redefinição de resposta, limpando cabeçalhos e corpo
    
    //response methods
    void Response::processResponse(){} //processa a solicitacao, verifica método, URI, etc. e constrói a resposta de acordo com os requisitos
    void Response::createResponse(std::string requestedContent){}
    void Response::readRequestedFile(std::string requestedContent){}

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

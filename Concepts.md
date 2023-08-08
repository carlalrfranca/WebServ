
## Servidor Web


- **Visão geral do HTTP**


### Requisições HTTP

Uma requisição HTTP é um pedido ou se preferir uma mensagem feito por um cliente (geralmente um navegador web ou aplicação) a um servidor web para obter ou manipular recursos. As requisições HTTP geralmente contém informações sobre o método (verbo) que deve ser executado no servidor e o recurso (URL) em que a ação deve ser realizada. <br>

Uma requisição HTTP típica inclui: <br>

Método: O verbo HTTP que indica a ação a ser realizada (GET, POST, PUT, DELETE, etc.). <br>
URL: O endereço do recurso no servidor. <br>
Cabeçalhos: Informações adicionais sobre a requisição, como informações de autenticação, tipo de conteúdo desejado, entre outros. <br>
Corpo: Dados opcionais enviados junto com a requisição, geralmente usado em requisições POST ou PUT para enviar dados ao servidor. <br>


### Response

Uma "response" é a mensagem enviada pelo servidor em resposta a uma requisição recebida do cliente. A resposta contém informações sobre o status da requisição, bem como os dados solicitados ou as ações executadas. <br>

No contexto do HTTP, a resposta também inclui informações como código de status, cabeçalhos e o corpo da resposta, que pode conter o conteúdo solicitado ou mensagens de erro.<br>

Uma resposta HTTP típica inclui: <br>

Código de Status: Um código numérico que indica o resultado da requisição (por exemplo, 200 OK para sucesso, 404 Not Found para recurso não encontrado, etc.). <br>
Cabeçalhos: Informações adicionais sobre a resposta, como tipo de conteúdo, data, etc. <br>
Corpo: Dados enviados de volta pelo servidor, geralmente o conteúdo solicitado ou informações relevantes. <br>

**Verbos (Métodos) e Respostas HTTP**

### GET:

Utilizado para solicitar recursos do servidor. <br>
Em geral para obter informações do servidor, sem modificar os dados.  <br>
O servidor deve retornar o recurso solicitado, ou uma mensagem de erro se o recurso não existir.  <br>

### POST:

Usado para enviar dados ao servidor para processamento.  <br>
Aplicado para criar ou atualizar recursos no servidor.  <br>
Pode ser aplicado para enviar informações do formulário, carregar arquivos, etc.  <br>
O servidor deve processar os dados e retornar uma resposta apropriada. <br>

### PUT:

Método empregado para atualizar um recurso no servidor ou criar um recurso se ele não existir.  <br>
O cliente envia os dados do recurso atualizado.  <br>
O servidor deve processar os dados e atualizar o recurso correspondente.  <br>

### DELETE:

Método usado para solicitar a exclusão de um recurso no servidor.  <br>
O servidor deve excluir o recurso especificado pelo cliente.  <br>

### TRACE:

Utilizado para diagnosticar a rota de uma requisição. <br>
O servidor retorna uma mensagem de resposta que contém as informações da requisição, permitindo ao cliente ver como a requisição foi tratada em cada ponto.  <br>

### OPTIONS:

Aplicado para obter informações sobre as opções de comunicação disponíveis para um recurso.  <br>
O servidor retorna uma lista de métodos suportados, cabeçalhos permitidos, etc.  <br>

### CONNECT:

Usado para estabelecer uma conexão de rede com um recurso identificado pelo URI.  <br>
Geralmente é usado para configurar uma conexão segura (SSL/TLS) através de um proxy.  <br>

### HEAD:

Método semelhante ao GET, mas solicita apenas os cabeçalhos do recurso, sem o corpo.  <br>
Pode ser usado para verificar informações sobre um recurso sem transferir os dados completos.  <br>

**Grupos de Status HTTP**

- 1XX - info
- 2XX - Sucesso
- 3XX - Redirecionamento
- 4XX - Erro no cliente
- 5XX - Erro no servidor


## Sockets e Conexões

Representa um ponto de comunicação (client-server) em uma rede entre o processo de origem e o de destino. Em geral o de origem é chamado de client, isso porque e o de destino o server.

## NGINX

## Parseamento

O arquivo de configuração

As Requests : dividido entre header e body

## Diretivas

São instruções específicas escritas dentro de blocos de configuração utilizadas para ajustar o comportamento do servidor web. No contexto do Nginx elas são usadas para definir diversas opções tais como porta em que o servidor deve escutar, nomes de domínio que os servidores devem responder, regras para o roteamento das solicitações. Essas instruções são uma forma de personalizar e adaptar o comportamento do servidor web, permitindo definir regras para lidar com as solicitações dos clientes e gerenciar o funcionamento do servidor.
Uma das propostas para o webserv é criar o próprio arquivo de configuração tendo como base os do Nginx. A seguir, algumas diretivas usadas no projeto:

### Listen

Utilizada para especificar em qual endereço IP e porta o servidor deve escutar as solicitações de cliente. Esta diretiva é fundamental para definir o local onde o servidor web estará disponível.


### Server_name

Usada para especificar os nomes de domínio ou endereço IP uqe o servidor deve responder. É necessário para configurar vários servidores virtuais e direcionar solicitações para o servidor correto.


### Location

Utilizada para configurar regras de correspondência de URL e determinar como o servidor deve tratar as solicitações para URLs específicas. Esta diretiva é importante para o roteamento de solicitações para diferentes recursos ou ações do servidor.


### Root

Especifica o diretório raiz usado para procurar um arquivo. É frequentemente usada em conjunto com a location para fornecer a localização dos arquivos disponibilizados pelo servidor.


## CGI


## Chunk



## Multiplex

Técnica que lida com multitarefas ou operações concorrentes em thread ou processo. O objetivo é melhorar a eficiência e desempenho do sistema por meio da manipulação de várias atividades simultaneamente, desta forma evita-se criar threads ou processos separados para cada tarefa, o que pode consumir recursos do sistema e levar a problemas de concorrência e sincronização.  Existem várias abordagens como a multiplexação de eventos e de sinal. Para este projeto usaremos a de entrada e saída.

### Multiplexação I/O

Nessa técnica, várias operações I/O são gerenciadas em uma thread. Em geral usa-se chamadas de sistema assíncronas ou não bloqueantes. Para o sistema linux são usados select(), poll(), epoll() . Essas chamadas fazem o thread aguardar várias operações I/O, em diferentes FD de arquivo de forma simultânea. Quando uma operação estiver pronta, o thread é notificado para processá-la.

## Não bloqueante

É um estilo de programação ou operação que não aguardam ou não bloqueiam a conclusão de determinada ação para prosseguir com a execução do código. Ou seja, as operações não-bloqueantes não esperam que uma operação seja concluída. Elas continuam executando outras tarefas ou verificam periodicamente o status da operação. Há duas principais formas de abordagem ou por operações assíncronas ou por chamadas de sistema. Para este projeto usaremos a função poll(). Tem como característica a abordagem não bloqueante, possibilitando que um programa monitore vários descritores de arquivo como sockets sem bloquear a execução de outras tarefas enquanto aguarda eventos de I/O nos demais descritores de arquivo.

## Função poll()

Monitora vários descritores de arquivo para eventos de leitua, escrita ou exceção (erro). É uma função mais recente que a select(), mais fácil porque ao invés de trabalhar com conjunto de bits, o poll() usa uma matriz de estruturas 'pollfd' para os descritores de arquivos que deverão ser monitorados. Embora seja mais intuitiva, essa função pode enfrentar problemas de escabilidade para grandes quantidades de FDs. Nesse sentido a select() lida melhor com muitos FDs sem diminuir significantemente o desempenho.


## Linha de desenvolvimento

A aplicação foi pensada em quatro etapas.



### O Loop Principal



## Referências

https://cplusplus.com/ <br>
http://nginx.org/en/docs/ <br>
https://www.youtube.com/watch?v=VKmPGmFY7H4&t=777s <br>
https://www.youtube.com/watch?v=lc6U93P4Sxw <br>
https://www.youtube.com/watch?v=jS9rBienEFQ <br>
https://www.youtube.com/watch?v=kncOJZrnkTg <br>
https://www.youtube.com/watch?v=OFKtR9e8bYs <br>
https://www.youtube.com/watch?v=CXzbUwK6lc8 <br>
https://www.youtube.com/watch?v=YqEqjODUkWY <br>
https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods/POST <br>
https://medium.com/operacionalti/https-medium-com-operacionalti-como-o-nginx-processa-uma-requisicao-330dc69470ef<br>
https://datatracker.ietf.org/doc/html/rfc2616/ <br>
https://devarea.com/linux-io-multiplexing-select-vs-poll-vs-epoll/ <br>
https://www.ibm.com/docs/en/i/7.1?topic=designs-using-poll-instead-select <br>
https://docs.nginx.com/nginx/admin-guide/web-server/serving-static-content/ <br>
https://stackoverflow.com/questions/15179409/how-to-parse-http-request-in-c <br>

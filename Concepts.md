
## Servidor Web


## Visão geral do HTTP

- Requisições HTTP
- verbos (métodos) que são enviados e as respostas retornadas pelo servidor
- get
- post
- put
- delete
- trace
- options
- connect
- head

### Grupos de Status HTTP

- 1XX - info
- 2XX - Sucesso
- 3XX - Redirecionamento
- 4XX - Erro no cliente
- 5XX - Erro no servidor

### Métodos [verbos]


## Request


## Response


## Sockets e Conexões

Representa um ponto de comunicação (client-server) em uma rede entre o processo de origem e o de destino. Em geral o de origem é chamado de client, isso porque


 e o de destino o server.


## CGI


## NGINX


## Chunk



## Multiplex

Técnica que lida com multitarefas ou operações concorrentes em thread ou processo. O objetivo é melhorar a eficiência e desempenho do sistema por meio da manipulação de várias atividades simultaneamente, desta forma evita-se criar threads ou processos separados para cada tarefa, o que pode consumir recursos do sistema e levar a problemas de concorrência e sincronização.  Existem várias abordagens como a multiplexação de eventos e de sinal. Para este projeto usaremos a de entrada e saída.

### Multiplexação I/O

Nessa técnica, várias operações I/O são gerenciadas em uma thread. Em geral usa-se chamadas de sistema assíncronas ou não bloqueantes. Para o sistema linux são usados select(), poll(), epoll() . Essas chamadas fazem o thread aguardar várias operações I/O, em diferentes FD de arquivo de forma simultânea. Quando uma operação estiver pronta, o thread é notificado para processá-la.

## Não bloqueante

É um estilo de programação ou operação que não aguardam ou não bloqueiam a conclusão de determinada ação para prosseguir com a execução do código. Ou seja, as operações não-bloqueantes não esperam que uma operação seja concluída. Elas continuam executando outras tarefas ou verificam periodicamente o status da operação. Há duas principais formas de abordagem ou por operações assíncronas ou por chamadas de sistema. Para este projeto usaremos a função poll(). Tem como característica a abordagem não bloqueante, possibilitando que um programa monitore vários descritores de arquivo como sockets sem bloquear a execução de outras tarefas enquanto aguarda eventos de I/O nos demais descritores de arquivo.

## Função poll()

Monitora vários descritores de arquivo para eventos de leitua, escrita ou exceção (erro). É uma função mais recente que a select(), mais fácil porque ao invés de trabalhar com conjunto de bits, o poll() usa uma matriz de estruturas 'pollfd' para os descritores de arquivos que deverão ser monitorados. Embora seja mais intuitiva, essa função pode enfrentar problemas de escabilidade para grandes quantidades de FDs. Nesse sentido a select() lida melhor com muitos FDs sem diminuir significantemente o desempenho.


## O Loop Principal


## Parseamento

O arquivo de configuração

As Requests : dividido entre header e body




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

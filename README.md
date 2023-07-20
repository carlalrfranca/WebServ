# WebServ
#### Desenvolvido por lfranca- e cleticia


O objetivo deste projeto é criar um servidor HTTP usando multiplexação de E/S [entrada e saída] em um loop de eventos.<br>

## Sobre o repositório

[`WebServ`](./WEBSERV/) : arquivo contendo o código-fonte da aplicação <br>

[`Info`](./concepts.md) : conceitos e aprendizados necessários <br>



## Tecnologias utilizadas

- [**Analysis tool**](https://valgrind.org/docs/manual/manual-core-adv.html) : software para depuração e localização de erros.
- [**C++**](https://cplusplus.com/) : linguagem que suporta o paradigma de orientação à objetos. 
- [**Curl**](https://curl.se/) : ferramenta de linha de comando para transmissão de dados. Protocolos usados : HTTP, DNS, ICMP (como utilitário PING).
- [**Git**](https://git-scm.com/) : para controle das versões dos arquivos.
- [**Github**](https://github.com/carlalrfranca/WebServ) : repositório para os arquivos.
- [**Makefile**](https://www.gnu.org/software/make/) : script aplicado para automatizar a compilação do projeto.
- [**Trello**](https://trello.com/b/S7qoJpEW/labs) : ferramenta visual para planejamento e organização.
- [**Stackedit**](https://stackedit.io/) : editor Markdown para escrever a documenntação do projeto.
- [**Vscode**](https://code.visualstudio.com/) : editor de código-fonte.
- [**Diagrams**](https://app.diagrams.net/) : ferramenta para criação de diagramas.



## Frameworks, bibliotecas e outros projetos

- [**LibCurl**](https://curl.se/libcurl/) : biblioteca para transferência URL.
- [**Markdown**](https://www.markdownguide.org/) : linguagem de marcação padronizada para formatação de textos Web, usada para escrever a documentação.


## Outline

#### **Divisão em Quatro etapas**

![matriz de tokens dentro nodo de subcomandos](./gifs_doc/WebServ.png)

## Funções

#### Primeira etapa


#### Segunda etapa


#### Terceira etapa


#### Quarta etapa




## Possíveis Soluções


## Acesso

#### 1. Repo

`git clone https://github.com/carlalrfranca/WebServ.git`


#### 2. Curl

`sudo apt-get install -y libcurl4-gnutls-dev`


#### 3. Compilação

`cd webserv` : acessa o diretório <br>
`make` : compila o projeto <br>
`make clean` : remove objetos <br>
`make fclean` : remove objetos e arquivos binários <br>
`make re` : recompila <br>


#### 4. Códigos usados e depuração


- Sintaxe do comandos usados : ```$ curl [OPÇÕES] [URL]```

| Comando       | Descrição                                                           |
|:--------------|:--------------------------------------------------------------------|
| -i            | <sub> retorna o corpo e a header </sub>                             |
| -i  --include | <sub> retorna a header no output </sub>                             |
| -I            | <sub> retorna a header  </sub>                                      |
| -d  --data	| <sub> dados a serem enviados no POST   </sub>                       |
| -H  --header  | <sub> envia header da requisição   </sub>                           |
| -X  --request |	<sub> especifica o método HTTP a ser usado na requisição </sub>   |


- Depuração Valgrind : `valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all`

| Comando               | Descrição                                                  |
|:----------------------|:-----------------------------------------------------------|
| --track-origins=yes   | <sub> rastreia as origens de valores não iniciados </sub>  |
| --leak-check=full     | <sub> retorna o detalhamento dos vazamentos </sub>         |
| --show-leak-kinds=all | <sub> relatório completo dos vazamentos </sub>             |

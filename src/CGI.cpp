/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 19:53:24 by lfranca-          #+#    #+#             */
/*   Updated: 2023/09/29 21:31:31 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/CGI.hpp"

CGI::CGI() : _response(""), _method(""), _inputFormData(""), _scriptOutput("")
{} 

CGI::~CGI()
{}

void CGI::setPathToScript(std::string scriptName)
{
	// construir o Caminho para o Script
	std::vector<std::string>ext = getExtensions();
	std::string tmp_path = getRoot() + scriptName + ext[0];
	_scriptName = tmp_path;
}

const std::string& CGI::getPathToScript(void) const
{
	return _scriptName;
}

void CGI::setRoot(const std::string& root)
{
	_rootToScripts = root;
}

const std::string& CGI::getRoot(void) const
{
	return _rootToScripts;
}

void CGI::setCommands(std::vector<std::string> commands)
{
	_scriptCommands = commands;
}

std::vector<std::string> CGI::getCommands(void) const
{
	return _scriptCommands;
}

void CGI::setExtensions(std::vector<std::string> extensions)
{
	_scriptExtensions = extensions;
}

std::vector<std::string> CGI::getExtensions(void) const
{
	return _scriptExtensions;
}

// volatile sig_atomic_t timedOut = 0;

// void handleAlarm(int signum) {
//     timedOut = 1;
// }

int CGI::executeScript(int *pipefd)
{
	int stdoutPipe[2];
	if (pipe(stdoutPipe) == -1)
	{
	    std::cerr << "ERROR creating STDOUT pipe" << std::endl;
	    return 500;
	}
	pid_t childPid = fork();
	// signal(SIGCHLD, handleChildExit);
	// signal(SIGALRM, handleAlarm);
	// Defina um limite de tempo em segundos
    unsigned int timeoutSeconds = 10000;

	std::cout << RED << "Conteúdo input: " << _inputFormData << END << std::endl;
	if (childPid == -1)
	{
		std::cerr << "ERROR creating CHILD PROCESS" << std::endl;
		return 500;
	}
	else if (childPid == 0) //é processo filho || AGORA É SÒ USAR ISSO PRA FAZER O UPLOAD DE ARQUIVOS!!!!
	{
		// por estarmos no processo filho nesse bloco, vamos então modificar o valor
		// do STDOUT pra poder redirecionar a saída do script pra cá
		// Escreve o corpo da solicitação no pipe, que será lido pelo processo filho
		// close(pipefd[0]); //não vamos usar o pipe de leitura, então fechamos ele por boa convenção
    	// Fecha o descritor de escrita do pipe, pois não será usado neste processo
    	close(pipefd[1]);
		close(stdoutPipe[0]);
		// Redireciona a entrada padrão (stdin) para o pipe de leitura
    	dup2(pipefd[0], STDIN_FILENO);
		dup2(stdoutPipe[1], STDOUT_FILENO);
    	// Redireciona a saída padrão (stdout) para o pipe de escrita
    	// dup2(pipefd[0], STDOUT_FILENO);
	
    	// Fecha o descritor de leitura do pipe, pois não será usado neste processo
    	close(pipefd[0]);
		close(stdoutPipe[1]);
		// Executamos agora o script de exemplo
		// quando tiver o método que recupera o path to Script
		// o nome do arquivo abaixo tem que ser passado atraves de uma variavel
		// talvez atraves dos parametros da funcao
		// porque daí essa função também fica flexível para o caso em que queremos criar o html
		// com a imagem que foi feita upload -> só passar o caminho dela e o script cuida
		execl(getPathToScript().c_str(), getPathToScript().c_str(), "./info_usuario.txt", static_cast<char*>(0));
		// execl("./web/cgi-bin/process_data.sh", "./web/cgi-bin/process_data.sh", static_cast<char*>(0));
		// Se chegou aqui, houve um erro no execl
		std::cerr << "ERROR executing SCRIPT" << std::endl;
		return 500;
	} else {
		// processo pai
		// Escreve o corpo da solicitação no pipe de leitura, que será lido pelo processo filho
		// close(pipefd[1]); //nao vamos usar o fd de escrita, então o fechamos por boa convenção
    	close(stdoutPipe[1]);
		std::string requestBody = _inputFormData;
    	write(pipefd[1], requestBody.c_str(), requestBody.length());
		close(pipefd[1]);
		// alarm(timeoutSeconds);
		// Ler a saída do script CGI do pipe e armazená-la numa string
		// std::string scriptOutPut;
		// Corpo da solicitação HTTP em uma string (substitua pela sua string real)
		struct timeval startTime;
		gettimeofday(&startTime, NULL);
		
		while (true)
		{
		    pid_t result = waitpid(childPid, NULL, WNOHANG);
		    if (result == -1)
		    {
		        perror("waitpid");
		        throw std::exception();
		    }
		    if (result != 0)
		        break;
		    struct timeval currentTime;
		    gettimeofday(&currentTime, NULL);
		    unsigned int elapsedTime = (currentTime.tv_sec - startTime.tv_sec) * 1000
		        + (currentTime.tv_usec - startTime.tv_usec) / 1000;
		    if (elapsedTime >= timeoutSeconds)
		    {
				std::cout << "ESTOUROU O TEMPO" << std::endl;
		        kill(childPid, SIGKILL);
		        return 504;
		    }
		    usleep(1000);
		}
		char buffer[1024]; //a saída crua terá que vir primeiro para um buffer
		while (true)
		{
			ssize_t bytesRead = read(stdoutPipe[0], buffer, sizeof(buffer)); //lê 1024 bytes do pipefd[0] pro buffer
			if (bytesRead <= 0)
				break;
			_scriptOutput.append(buffer, bytesRead);
		}
		close(stdoutPipe[0]); //terminamos de ler da saída do script, então podemos fechar esse pipe
		// é aqui que breca se o script demorar demais? (SCRIPT COM LOOP?) -> o statusCode de estouro de limite de tempo seria 504
		// Agora a saída do script CGI está armazenada em 'scriptOutPut'
		std::cout << "------ SAÍDA DO SCRIPT --------\n" << _scriptOutput << std::endl;
		return 204;
	}
}

int CGI::uploadImage(std::string request_content, size_t data_init_pos)
{
	std::string image_content = request_content.substr(data_init_pos + 4);
	size_t content_type_pos = image_content.find("Content-Type");
	std::size_t contentTypeEnd = image_content.find("\r\n", content_type_pos);
	if (contentTypeEnd != std::string::npos)
	{
		std::cout << BLUE << "ENCONTROU o fim CONTENT-TYPE NA RESPONSE" << END << std::endl;
		std::size_t fileDataStart = contentTypeEnd + 4;
		std::string image_content_cleaned = image_content.substr(fileDataStart);
    	std::ofstream imageFile("Imagem-salva.jpg", std::ios::binary);
		std::cout << "Size of image: " << image_content_cleaned.size() << std::endl;
		if (imageFile.is_open())
		{
			imageFile.write(image_content_cleaned.c_str(), image_content_cleaned.size());
			imageFile.close();
		}
	} else {
		return 404; //seria outro erro?
		// std::cout << RED << "[[NÃO]] ENCONTROU o fim CONTENT-TYPE NA RESPONSE" << END << std::endl;
		// _response = "HTTP/1.1 404 Not Found\r\nDate: Sat, 03 Sep 2023 12:34:56 GMT\r\nConnection: keep-alive\r\n\r\n";
		// _response += "<html><body><h1>Error 404</h1></body></html>";
	}
	return 204;
}

int CGI::storeFormInput(std::size_t data_init_pos, const std::string& request_content)
{
	_inputFormData = request_content.substr(data_init_pos + 4);
	setenv("QUERY_STRING", _inputFormData.c_str(), 1);
	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		std::cerr << "ERROR creating PIPE" << std::endl;
		return 500;
	}
	int resultCGI = 0;
	resultCGI = executeScript(pipefd);
	if (resultCGI == 504)
		return 504;
	if (_scriptOutput.empty())
		return 500;
	_response += _scriptOutput;
	// std::cout << _response << std::endl;
	return 204;
}

int CGI::handleCGIRequest(Request &request) //provavelmente vai ter que receber o ponteiro pro obj Response pra poder acessar headers
{
	// primeiro criamos a header da response:
	_response = "HTTP/1.1 204 No Content\r\nDate: Sat, 03 Sep 2023 12:34:56 GMT\r\nConnection: keep-alive\r\n\r\n";

	std::string request_content = request.getRequest();
	// Abre um arquivo para escrever (substitua "request.txt" pelo nome do arquivo desejado)
    std::ofstream outputFile("request_imagem.txt");

    // Verifica se o arquivo foi aberto com sucesso
    if (!outputFile.is_open())
	{
        std::cerr << "Erro ao abrir o arquivo." << std::endl;
        return 500;
    }

    // Escreve o conteúdo da solicitação no arquivo
    outputFile << request_content;

    // Fecha o arquivo
    outputFile.close();
	std::size_t data_init_pos = request_content.find("\r\n\r\n");
	std::size_t boundary_pos = request_content.find("boundary=");
	if (boundary_pos == std::string::npos)
	{
		int resultCode = 0;
		resultCode = storeFormInput(data_init_pos, request_content);
		return resultCode;
	}
	if (data_init_pos != std::string::npos)
	{
		// como ele vai ter uma imagem.. entao
		// pegar o boundary (indicativo de que é um arquivo sendo submetido)
		// e extrair o Content-Type a partir disso
		// no caso do formulario, só vem o conteudo submetido direto (ver request_imagem.txt)
		int resultCode = 0;
		resultCode = uploadImage(request_content, data_init_pos);
		return resultCode;
	}
	return 204;
}

std::string CGI::getResponse(void) const
{
	return _response;
}

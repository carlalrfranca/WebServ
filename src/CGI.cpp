/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 19:53:24 by lfranca-          #+#    #+#             */
/*   Updated: 2023/10/02 12:32:59 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/CGI.hpp"

CGI::CGI() : _response(""), _method(""), _inputFormData(""), _scriptOutput("")
{} 

CGI::CGI(const std::string& root, std::vector<std::string> commands, std::vector<std::string> extensions, std::string scriptName)
	: _response(""), _method(""), _inputFormData(""), _scriptOutput("")
{
	setRoot(root);
	setCommands(commands);
	setExtensions(extensions);
	setPathToScript(scriptName);
	struct stat info;
	if (stat(_scriptName.c_str(), &info) != 0)
	{
		std::cerr << "Error: Config file doesn't exist!" << std::endl;
		throw ErrorException("Script Error: Path doesn't exist");
	}
}

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

int CGI::executeScript(int *pipefd, std::string fileName)
{
	int stdoutPipe[2];
	if (pipe(stdoutPipe) == -1)
	{
	    std::cerr << "ERROR creating STDOUT pipe" << std::endl;
	    return 500;
	}
	pid_t childPid = fork();
    unsigned int timeoutSeconds = 10000;

	std::cout << RED << "Conteúdo input: " << _inputFormData << END << std::endl;
	if (childPid == -1)
	{
		std::cerr << "ERROR creating CHILD PROCESS" << std::endl;
		return 500;
	}
	else if (childPid == 0) //é processo filho
	{
    	close(pipefd[1]);
		close(stdoutPipe[0]);
    	dup2(pipefd[0], STDIN_FILENO);
		dup2(stdoutPipe[1], STDOUT_FILENO);
    	close(pipefd[0]);
		close(stdoutPipe[1]);
		execl(getPathToScript().c_str(), getPathToScript().c_str(), fileName.c_str(), static_cast<char*>(0));
		std::cerr << "ERROR executing SCRIPT" << std::endl;
		return 500;
	}
	else 
	{
		// processo pai
    	close(stdoutPipe[1]);
		std::string requestBody = _inputFormData;
    	write(pipefd[1], requestBody.c_str(), requestBody.length());
		close(pipefd[1]);
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

		char buffer[1024];
		while (true)
		{
			ssize_t bytesRead = read(stdoutPipe[0], buffer, sizeof(buffer));
			if (bytesRead <= 0)
				break;
			_scriptOutput.append(buffer, bytesRead);
		}
		close(stdoutPipe[0]);
		return 204;
	}
}

int CGI::uploadImage(std::string request_content, size_t data_init_pos)
{
	std::string image_content = request_content.substr(data_init_pos + 4);
	std::string contentTypeField = "Content-Type:";
	std::string fileNameField = "filename=";
	size_t content_type_pos = image_content.find(contentTypeField);
	size_t content_type_value = content_type_pos + contentTypeField.size();

	std::size_t contentTypeEnd = image_content.find("\r\n", content_type_pos);
	std::string type_file = image_content.substr(content_type_value, contentTypeEnd);
	size_t ext_end = type_file.find_last_not_of(" ");
	type_file = type_file.substr(0, ext_end - content_type_value + 1);
	std::cout << YELLOW << type_file << END << std::endl;
	size_t extension_pos = type_file.find("/");
	std::string extension;
	if (extension_pos != std::string::npos)
		extension = type_file.substr(extension_pos + 1);
	std::cout << BLUE << extension << END << std::endl;
	std::cout << std::endl;
	size_t fileNamePos = request_content.find(fileNameField);
	fileNamePos += fileNameField.size();
	std::string uploadedFileName;
	size_t fileNameEnd = request_content.find_first_of("\"", fileNamePos + 1);
	if (fileNameEnd != std::string::npos)
		uploadedFileName = request_content.substr(fileNamePos, fileNameEnd);
	std::cout << BLUE << "Nome do arquivo UPLOAD: " << uploadedFileName << END << std::endl;
	if (contentTypeEnd != std::string::npos)
	{
		std::cout << BLUE << "ENCONTROU o fim CONTENT-TYPE NA RESPONSE" << END << std::endl;
		std::size_t fileDataStart = contentTypeEnd + 4;
		std::string image_content_cleaned = image_content.substr(fileDataStart);
    	std::ofstream imageFile(uploadedFileName.c_str(), std::ios::binary);
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

/*
int CGI::uploadImageCGI(std::string request_content, size_t data_init_pos)
{
	_inputFormData = request_content;
	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		std::cerr << "ERROR creating PIPE" << std::endl;
		return 500;
	}
	int resultCGI = 0;
	resultCGI = executeScript(pipefd, "Imagem-salva.jpg");
	if (resultCGI == 504)
		return 504;
	if (_scriptOutput.empty())
		return 500;
	_response += _scriptOutput;
	// std::cout << _response << std::endl;
	return 204;
}*/

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
	resultCGI = executeScript(pipefd, PATH_FORM_DATA);
	if (resultCGI == 504)
		return 504;
	if (_scriptOutput.empty())
		return 500;
	_response += _scriptOutput;
	// std::cout << _response << std::endl;
	return 204;
}


int CGI::executeScriptForGET(int *pipefd, std::string requestedFilePath)
{
	pid_t childPid = fork();
	
	if (childPid == -1)
	{
		std::cerr << "ERROR creating CHILD PROCESS" << std::endl;
		return 500;
	}
	else if (childPid == 0) //é processo filho
	{
		// por estarmos no processo filho nesse bloco, vamos então modificar o valor
		// do STDOUT pra poder redirecionar a saída do script pra cá
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]); //não vamos usar o pipe de leitura, então fechamos ele por boa convenção
		
		// Executamos agora o script de exemplo
		std::cout << YELLOW << "Caminho do SCRIPT CHAMADO" << END << std::endl;
		std::cout << YELLOW << getPathToScript() << END << std::endl;
		execl(getPathToScript().c_str(), getPathToScript().c_str(), requestedFilePath.c_str(), static_cast<char*>(0));
		// Se chegou aqui, houve um erro no execl
		std::cerr << "ERROR executing SCRIPT" << std::endl;
		return 500;
	} else {
		// processo pai
		close(pipefd[1]); //nao vamos usar o fd de escrita, então o fechamos por boa convenção
		
		// Ler a saída do script CGI do pipe e armazená-la numa string
		// std::string scriptOutPut;
		char buffer[1024]; //a saída crua terá que vir primeiro para um buffer
		while (true)
		{
			ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer)); //lê 1024 bytes do pipefd[0] pro buffer
			if (bytesRead <= 0)
				break;
			_scriptOutput.append(buffer, bytesRead);
		}
		close(pipefd[0]); //terminamos de ler da saída do script, então podemos fechar esse pipe
		
		// É importante colocarmos esse processo (pai) pra aguardar o término do processo filho
		int status;
		waitpid(childPid, &status, 0);
		// Agora a saída do script CGI está armazenada em 'scriptOutPut'
		std::cout << "------ SAÍDA DO SCRIPT --------\n" << _scriptOutput << std::endl;
	}
	return 200;
}

int CGI::CGIForGetRequest(std::string requestedFilePath)
{
	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		std::cerr << "ERROR creating PIPE" << std::endl;
		return 500;
	}
	int resultCGI = 0;
	resultCGI = executeScriptForGET(pipefd, requestedFilePath);
	if (resultCGI == 504)
		return 504;
	if (_scriptOutput.empty())
		return 500;
	_response += _scriptOutput;
	return 200;
}


int CGI::handleCGIRequest(Request &request) //provavelmente vai ter que receber o ponteiro pro obj Response pra poder acessar headers
{

	// tem que considerar que essa chamada pode vir de uma requisição GET (para o arquivo, por exemplo)
	/* lembrando que, pela request GET, nós já receberemos o caminho completo do recurso
	// ou pelo menos nós vamos ter que passar por parametro o caminho completo do recurso
	*/
	if (request.getMethod() == "GET")
	{
		// tornar esse argumento flexivel (resgatar o valor -> a imagem pedida da request)
		int result = CGIForGetRequest("./imagem-salva.jpg");
		return result;
	}

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
		resultCode = uploadImage(request_content, data_init_pos); //PASSAR ISSO PRO CGI ADAPTADO (é preciso que o server também espere EOF do CGI - ou content-length dele)
		return resultCode;
	}
	return 204;
}

std::string CGI::getResponse(void) const
{
	return _response;
}

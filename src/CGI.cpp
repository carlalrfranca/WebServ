/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 19:53:24 by lfranca-          #+#    #+#             */
/*   Updated: 2023/09/07 22:30:53 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/CGI.hpp"

CGI::CGI() : _response(""), _method(""), _inputFormData(""), _scriptOutput("")
{ }

CGI::~CGI()
{ }

// setters and getters

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

// ----------------------

// Adaptar o método abaixo de acordo com a chamada vinda da Response

void CGI::executeScript(int *pipefd)
{
	pid_t childPid = fork();
	
	if (childPid == -1)
	{
		std::cerr << "ERROR creating CHILD PROCESS" << std::endl;
		return ;
	}
	else if (childPid == 0) //é processo filho
	{
		// por estarmos no processo filho nesse bloco, vamos então modificar o valor
		// do STDOUT pra poder redirecionar a saída do script pra cá
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]); //não vamos usar o pipe de leitura, então fechamos ele por boa convenção
		
		// Executamos agora o script de exemplo
		execl("./process_form.sh", "./process_form.sh", static_cast<char*>(0));
		// Se chegou aqui, houve um erro no execl
		std::cerr << "ERROR executing SCRIPT" << std::endl;
		return ;
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
}

void CGI::handleCGIRequest(Request &request) //provavelmente vai ter que receber o ponteiro pro obj Response pra poder acessar headers
{
	// primeiro criamos a header da response:
	_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

	/* 
		aqui vamos:
			- confirmar que o método é o POST (depois, transferir isso para um outro método, antes de
			chamar essa função aqui)
				> Se for, vamos resgatar o conteúdo enviado pelo formulário e passar para uma
				  função que cuidará disso (parseFormData) -> irá criar um map com os valores dos inputs do formulario
				> Vamos inserir esses valores na variavel de ambiente QUERY_STRING com setenv() - (para que o script possa acessar essas informações)
				> vamos abrir um pipe_fd pra leitura e escrita - e poder manipular o STDIN e STDOUT pra poder
				  "pegar" aqui no programa o que o script, por padrão, só pode escrever no terminal (STDOUT)
				> vamos fazer um fork() - criar um processo filho que irá modificar o STDOUT com dup e executará o script
				> vamos resgatar a saída do script e construir uma response pra retornar pra mainLoop()
	*/

	// int indexMethod = request.find("POST");
	// if (indexMethod != std::string::npos)
	// {
		// _method = request.substr(indexMethod, indexMethod + 3);
		std::string request_content = request.getRequest();
		std::size_t data_init_pos = request_content.find("\r\n\r\n");
		std::cout << "CABEÇALHO DESSA REQUEST ----------- !!!!!!!!!!!!!!!!!!!!" << std::endl;
		std::cout << request_content.substr(0, data_init_pos) << std::endl;
		std::cout << "!!!!!!!!!!!!!!!!!!!!!!! ----------- !!!!!!!!!!!!!!!!!!!!" << std::endl;
		if (data_init_pos != std::string::npos)
		{
			//_inputFormData = request_content.substr(data_init_pos + 4);
			// como ele vai ter uma imagem.. entao
			std::cout << "Entrou no if" << std::endl;
			std::string image_content = request_content.substr(data_init_pos + 4);
			std::cout << "Entrou no if = 2" << std::endl;
			size_t content_type_pos = image_content.find("Content-Type");
			std::cout << "Entrou no if = 3" << std::endl;
			std::size_t contentTypeEnd = image_content.find("\r\n", content_type_pos);
			std::cout << "Entrou no if = 4" << std::endl;
			std::cout << "ContenttypeEnd::: " << contentTypeEnd << std::endl;
			if (content_type_pos != std::string::npos)
				std::cout << "Encontrou content-type" << std::endl;
			if (contentTypeEnd != std::string::npos)
				std::cout << "Encontrou O FIM do content-type" << std::endl;
			std::string content_type = image_content.substr(content_type_pos, contentTypeEnd);

			std::cout << "CONTENT TYPE ----> " << content_type << std::endl;
			// Save the received image data to a file
			std::size_t fileDataStart = contentTypeEnd + 4;
			std::string image_content_cleaned = image_content.substr(fileDataStart);
    		std::ofstream imageFile("SOMEWHERE_image.jpg", std::ios::binary);
			// Process chunks of data
			std::cout << std::endl;
			// std::cout << "Image content: -----------" << std::endl;
			// std::cout << image_content_cleaned << std::endl;
			std::cout << "Size of image: " << image_content_cleaned.size() << std::endl;
			if (imageFile.is_open())
			{
				imageFile.write(image_content_cleaned.c_str(), image_content_cleaned.size());
				imageFile.close();
			}
			_response += "<html><body><h1>Simple Image</h1></body></html>";
			
			// -------------------------------------------------
			// setamos a env QUERY_STRING com esses valores do form
			// setenv("QUERY_STRING", _inputFormData.c_str(), 1);

			// criar o pipe pra redirecionar a saída do script pra poder resgatar pra cá
			//int pipefd[2];
			//if (pipe(pipefd) == -1)
			//{
			//	std::cerr << "ERROR creating PIPE" << std::endl;
			//	return;
			//}
			//std::cout << "----------- CRIOU O PIPE! -----------" << std::endl;
//
			//executeScript(pipefd);
			//if (_scriptOutput.empty())
			//{
			//	return ;
			//}
			//_response += _scriptOutput;
		}
		else
		{
			std::cout << "Entrou no else" << std::endl;
		// ou seja, não foi um 'POST'
		// retorna uma reponse só pra teste
			_response += "<html><body><h1>Simple Form</h1><form method=\"post\">";
		   _response += "Name: <input type=\"text\" name=\"name\"><br>Email: <input type=\"text\" name=\"email\"><br>";
		   _response += "<input type=\"submit\" value=\"Submit\"></form></body></html>";
		}
		std::cout << "Tá fora do if else do cgi" << std::endl;
}

std::string CGI::getResponse(void) const
{
	return _response;
}
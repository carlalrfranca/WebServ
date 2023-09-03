/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 19:53:24 by lfranca-          #+#    #+#             */
/*   Updated: 2023/09/02 23:34:07 by lfranca-         ###   ########.fr       */
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

void CGI::handleCGIRequest(std::string& request)
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

	int indexMethod = request.find("POST");
	if (indexMethod != std::string::npos)
	{
		_method = request.substr(indexMethod, indexMethod + 3);
		std::size_t data_init_pos = request.find("\r\n\r\n");
		if (data_init_pos != std::string::npos)
		{
			_inputFormData = request.substr(data_init_pos + 4);
			
			// setamos a env QUERY_STRING com esses valores do form
			setenv("QUERY_STRING", _inputFormData.c_str(), 1);

			// criar o pipe pra redirecionar a saída do script pra poder resgatar pra cá
			int pipefd[2];
			if (pipe(pipefd) == -1)
			{
				std::cerr << "ERROR creating PIPE" << std::endl;
				return;
			}
			std::cout << "----------- CRIOU O PIPE! -----------" << std::endl;

			executeScript(pipefd);
			if (_scriptOutput.empty())
			{
				return ;
			}
			_response += _scriptOutput;
		}
	}
	else
	{
		// ou seja, não foi um 'POST'
		// retorna uma reponse só pra teste
		_response += "<html><body><h1>Simple Form</h1><form method=\"post\">";
        _response += "Name: <input type=\"text\" name=\"name\"><br>Email: <input type=\"text\" name=\"email\"><br>";
        _response += "<input type=\"submit\" value=\"Submit\"></form></body></html>";
	}
}

std::string CGI::getResponse(void) const
{
	return _response;
}
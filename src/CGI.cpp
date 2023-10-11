/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 19:53:24 by lfranca-          #+#    #+#             */
/*   Updated: 2023/10/10 23:57:53 by lfranca-         ###   ########.fr       */
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
	// struct stat info;

	// if(stat(_scriptName.c_str(), &info) != 0)
	// {
	// 	throw ErrorException("Script Error: Path doesn't exist");
	// }
}

CGI::~CGI()
{}

const std::string& CGI::getPathToScript(void) const
{
	return _scriptName;
}

const std::string& CGI::getUploadStore() const
{
	return _uploadStore;
}

void CGI::setScriptNameDirectly(std::string scriptName)
{
	_scriptName = scriptName;
}

void CGI::setPathToScript(std::string scriptName)
{
	std::vector<std::string>ext = getExtensions();
	std::string tmp_path = getRoot() + scriptName + ext[0];
	_scriptName = tmp_path;
}

void CGI::setRoot(const std::string& root)
{
	_rootToScripts = root;
}

const std::string& CGI::getRoot(void) const
{
	return _rootToScripts;
}

void CGI::setUploadStoreFolder(std::string uploadStore)
{
	_uploadStore = uploadStore;
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

int CGI::executeProcessParent(int *pipefd, unsigned int timeoutSeconds, pid_t childPid, int *stdoutPipe)
{
	close(stdoutPipe[1]);
	std::string requestBody = _inputFormData;
	write(pipefd[1], requestBody.c_str(), requestBody.length());
	close(pipefd[1]);
	struct timeval startTime;
	gettimeofday(&startTime, NULL);

	while(true)
	{
	    pid_t result = waitpid(childPid, NULL, WNOHANG);

	    if(result == -1)
	    {
			std::cout << RED << "Error: waitpid" << END << std::endl;
	        throw std::exception();
	    }
	    if(result != 0)
	        break;
	    struct timeval currentTime;
	    gettimeofday(&currentTime, NULL);

	    unsigned int elapsedTime = (currentTime.tv_sec - startTime.tv_sec) * 1000
	        + (currentTime.tv_usec - startTime.tv_usec) / 1000;
	    if(elapsedTime >= timeoutSeconds)
	    {
	        kill(childPid, SIGKILL);
	        return 504;
	    }
	    usleep(1000);
	}
	char buffer[1024];

	while(true)
	{
		ssize_t bytesRead = read(stdoutPipe[0], buffer, sizeof(buffer));
		if (bytesRead <= 0)
			break;
		_scriptOutput.append(buffer, bytesRead);
	}
	close(stdoutPipe[0]);
	return 200;
}

void CGI::executeProcessChild(int *pipefd, std::string fileName, int *stdoutPipe)
{
	close(pipefd[1]);
	close(stdoutPipe[0]);
	dup2(pipefd[0], STDIN_FILENO);
	dup2(stdoutPipe[1], STDOUT_FILENO);
	close(pipefd[0]);
	close(stdoutPipe[1]);

    if(execl(getCommands()[0].c_str(), getCommands()[0].c_str(), getPathToScript().c_str(), fileName.c_str(), static_cast<char *>(0)) == -1)
        std::cout << RED << "Error executing script" << END << std::endl;
}

int CGI::executeScript(int *pipefd, std::string fileName)
{
	int stdoutPipe[2];
	
	if(pipe(stdoutPipe) == -1)
	{
	    std::cout << RED << "Error creating STDOUT pipe" << END << std::endl;
	    return 500;
	}
	pid_t childPid = fork();
    unsigned int timeoutSeconds = 10000;

	if(childPid == -1)
	{
		std::cout << RED << "Error creating CHILD PROCESS" << END << std::endl;
		return 500;
	} 
	else if(childPid == 0)
	{
		executeProcessChild(pipefd, fileName, stdoutPipe);
		return 500;
	}
	else
	{
		int result = executeProcessParent(pipefd, timeoutSeconds, childPid, stdoutPipe);
		if(result == 504)
			return 504;
	}
	return 200;
}

int CGI::uploadImage(Request &request, std::string request_content, size_t data_init_pos)
{
	std::string image_content = request_content.substr(data_init_pos + 4);
	std::string contentTypeField = "Content-Type:";
	size_t content_type_pos = image_content.find(contentTypeField);
	std::size_t contentTypeEnd = image_content.find("\r\n", content_type_pos);
	std::string uploadedFileName;
	uploadedFileName = request.getFilename();

	if(contentTypeEnd != std::string::npos)
	{
		std::size_t fileDataStart = contentTypeEnd + 4;
		std::string image_content_cleaned = image_content.substr(fileDataStart);
    	std::ofstream imageFile(uploadedFileName.c_str(), std::ios::binary);
		if(imageFile.is_open())
		{
			imageFile.write(image_content_cleaned.c_str(), image_content_cleaned.size());
			imageFile.close();
		}
	} else {
		return 404;
	}
	return 204;
}

int CGI::uploadImageCGI(Request &request)
{
	setenv("CONTENT_TYPE", request.getFileFormat().c_str(), 1);
	setenv("LOCATION", "/images/", 1);
	setenv("FILE_NAME", request.getFilename().c_str(), 1);
	setenv("CONTENT_LENGTH", request.getContentLength().c_str(), 1);
	setenv("E_ARQUIVO", "ARQ", 1);
	_inputFormData = request.getBody();
	std::string fileName = request.getFilename();
	for (size_t i = 0; i < fileName.length(); i++) {
        if (fileName[i] == ' ') {
            fileName[i] = '_';
        }
    }
	request.setFilename(fileName);
	size_t startBinaryContent = _inputFormData.find(request.getFileFormat());

	if(startBinaryContent != std::string::npos)
	{
		startBinaryContent += (request.getFileFormat().size() + 4);
		_inputFormData = _inputFormData.substr(startBinaryContent);
	}
	int pipefd[2];

	if(pipe(pipefd) == -1)
	{
		std::cout << RED << "Error creating PIPE." << END << std::endl;
		return 500;
	}
	int resultCGI = 0;
	std::string pathToStore = getUploadStore() + request.getFilename();
	resultCGI = executeScript(pipefd, pathToStore);
	
	if(resultCGI == 504)
		return 504;
	if(_scriptOutput.empty())
		return 500;
	_response = _scriptOutput;
	return 200;
}

int CGI::storeFormInput(std::size_t data_init_pos, const std::string& request_content)
{
	_inputFormData = request_content.substr(data_init_pos + 4);
	setenv("QUERY_STRING", _inputFormData.c_str(), 1);
	int pipefd[2];

	if(pipe(pipefd) == -1)
	{
		std::cout << RED << "Error creating PIPE" << END << std::endl;
		return 500;
	}
	int resultCGI = 0;
	std::string pathToStore = getUploadStore() + "form_data.txt";
	resultCGI = executeScript(pipefd, pathToStore);
	if(resultCGI == 504)
		return 504;
	if(_scriptOutput.empty())
		return 500;
	_response += _scriptOutput;
	return 204;
}


int CGI::executeScriptForGET(int *pipefd, std::string requestedFilePath)
{
	pid_t childPid = fork();

	if(childPid == -1)
	{
		std::cout << RED << "Error creating CHILD PROCESS" << END << std::endl;
		return 500;
	}
	else if(childPid == 0)
	{
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);

		if (execl(getCommands()[0].c_str(), getCommands()[0].c_str(), getPathToScript().c_str(), requestedFilePath.c_str(), static_cast<char *>(0)) == -1)
        	std::cout << RED << "Error executing SCRIPT" << END << std::endl;
		return 500;
	} else {
		close(pipefd[1]);
		struct timeval startTime;
		unsigned int timeoutSeconds = 10000;
		gettimeofday(&startTime, NULL);

		while(true)
		{
		    pid_t result = waitpid(childPid, NULL, WNOHANG);

		    if(result == -1)
		    {
		        std::cout << RED << "Error: waitpid" << END << std::endl;
		        throw std::exception();
		    }
		    if(result != 0)
		        break;

		    struct timeval currentTime;
		    gettimeofday(&currentTime, NULL);
		    unsigned int elapsedTime = (currentTime.tv_sec - startTime.tv_sec) * 1000
		        + (currentTime.tv_usec - startTime.tv_usec) / 1000;

		    if(elapsedTime >= timeoutSeconds)
		    {
		        kill(childPid, SIGKILL);
		        return 504;
		    }
		    usleep(1000);
		}
		char buffer[1024];
		while(true)
		{
			ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer));
			if(bytesRead <= 0)
				break;
			_scriptOutput.append(buffer, bytesRead);
		}
		close(pipefd[0]);
	}
	return 200;
}

int CGI::CGIForGetRequest(std::string requestedFilePath)
{
	int pipefd[2];

	if(pipe(pipefd) == -1)
	{
		std::cout << RED << "Error creating PIPE." << END << std::endl;
		return 500;
	}
	int resultCGI = 0;
	resultCGI = executeScriptForGET(pipefd, requestedFilePath);

	if(resultCGI == 504)
		return 504;
	if(_scriptOutput.empty())
		return 500;
	_response += _scriptOutput;
	return 200;
}

std::string CGI::setDateAndTime()
{
	char buffer[80];
	time_t rawTime;
	struct tm *timeInfo;
	time(&rawTime);
	timeInfo = localtime(&rawTime);
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", timeInfo);
	std::string bufferString(buffer);
	return bufferString;
}

std::string CGI::generateHeadersSucessCGI(int statusCode, Request &request)
{
	StatusMessages statusMes;
	std::stringstream toConvertToString;
    toConvertToString << statusCode;
    std::string statusCodeStr = toConvertToString.str();
	std::string headers;
	std::string allowedMethodsStr = "";
	std::vector<std::string> allowedMethodsVec = request.getAllowedMethods();

	for(size_t i = 0; i < allowedMethodsVec.size(); ++i)
	{
		if (i == 0)
			allowedMethodsStr = allowedMethodsVec[i];
		else
			allowedMethodsStr =  allowedMethodsStr + ", " + allowedMethodsVec[i]; 
	}
	std::stringstream contentLengthToString;
	int content_length = _response.size();
	std::string codeMessage = statusMes.getMessage(statusCode); 
	contentLengthToString << content_length;
	std::string contentLen = contentLengthToString.str();
    headers += "HTTP/1.1 " + statusCodeStr + " " + codeMessage + "\r\n";

	if(statusCode == 200)
	{
		headers += "Content-Length: " + contentLen + "\r\n";
		headers += "Content-Type: text/html\r\n";
	}
	else if(statusCode == 204)
	{
		headers += "Content-Length: 0\r\n";
		headers += "Content-Type: application/octet-stream\r\n";
	}
	headers += "Date: " + setDateAndTime() + "\r\n";
	headers += "Server: Webserver-42SP\r\n";
	headers += "Access-Control-Allow-Origin: *\r\n";
	headers += "Access-Control-Allow-Methods: " + allowedMethodsStr + "\r\n";
	headers += "Cache-Control: no-cache, no-store, must-revalidate\r\n";
    headers += "\r\n";
	
	if(statusCode == 200)
	{
		headers += _response;
		_response = headers;
	}
	return headers;
}

int CGI::handleCGIRequest(Request &request)
{
	if(request.getMethod() == "GET")
	{
		int result = CGIForGetRequest(getUploadStore());
		return result;
	}
	std::string request_content = request.getRequest();
	std::size_t data_init_pos = request_content.find("\r\n\r\n");
	std::size_t boundary_pos = request_content.find("boundary=");

	if(boundary_pos == std::string::npos)
	{
		int resultCode = 0;
		resultCode = storeFormInput(data_init_pos, request_content);
		if(resultCode == 204)
			_response = generateHeadersSucessCGI(resultCode, request);
		return resultCode;
	}
	if(data_init_pos != std::string::npos)
	{
		int resultCode = 0;
		resultCode = uploadImageCGI(request);

		if(resultCode == 200)
			generateHeadersSucessCGI(resultCode, request);
		return resultCode;
	}
	_response = generateHeadersSucessCGI(200, request);
	return 200;
}

std::string CGI::getResponse(void) const
{
	return _response;
}

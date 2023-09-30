/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/29 21:55:25 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Utils.hpp"

Utils::Utils()
{}

Utils::~Utils()
{}

void Utils::trimSpaces(std::string &str)
{
    std::string::size_type start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos)
    {
        str.clear();
        return;
    }
    std::string::size_type end = str.find_last_not_of(" \t\n\r");
    str = str.substr(start, end - start + 1);
}

//não esta sendo usada pra nada na ConfigParser
void Utils::trimWhiteSpace(std::string &line)
{
    size_t startPos = line.find_first_not_of(" \t");
    if(startPos != std::string::npos)
        line = line.substr(startPos);
    size_t endPos = line.find_last_not_of(" \t");
    if(endPos != std::string::npos)
        line = line.substr(0, endPos + 1);
}

void Utils::removeComments(std::string &line)
{
    size_t start = line.find('#');
    while (start != std::string::npos)
    {
        size_t end = line.find('\n', start);
        if (end != std::string::npos)
            line.erase(start, end - start + 1); // +1 para '\n'
        else{
            line.erase(start);
            break;
        }
        start = line.find('#');
    }
}

bool Utils::containsOnlyLetters(const std::string& str)
{
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) 
	{
        if (!std::isalpha(*it))
            return false;
    }
    return true;
}

bool Utils::containsOnlyNumbers(const std::string& str)
{
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
        if (!std::isdigit(*it))
            return false;
    }
    return true;
}

std::vector<std::string> Utils::splitString(const std::string& input, char delimiter)
{
    size_t start = 0;
    size_t end = input.find(delimiter);
    std::vector<std::string> tokens;

    while (end != std::string::npos)
    {
        end = input.find(delimiter, start);
		std::string part = input.substr(start, end - start);
		if (!part.empty())
        	tokens.push_back(part);
        start = end + 1;
    }
    // Adicione a última parte da string
    // tokens.push_back(input.substr(start));
    return tokens;
}

//antiga contains
bool Utils::isInVector(const std::vector<std::string>& vec, const std::string& content)
{
    for(size_t i = 0; i < vec.size(); ++i)
    {
        if(vec[i] == content)
            return true;
    }
    return false;
}

bool Utils::fileExists(std::string filePath)
{
    std::ifstream file(filePath.c_str());
    return file.good();
}

bool Utils::pathExists(const std::string &path)
{
    struct stat info;
    return (stat(path.c_str(), &info) != 0);
}
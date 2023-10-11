/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/11 15:22:03 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include "HeadersLibs.hpp"

class Utils
{
    public:
    
        Utils();
        ~Utils();

        void trimSpaces(std::string &str);
        void trimWhiteSpace(std::string &line);
		void removeComments(std::string &line);
        bool containsOnlyNumbers(const std::string& str);
		bool containsOnlyLetters(const std::string& str);
        bool fileExists(std::string filePath);
        bool pathExists(const std::string &path);
        bool isInVector(const std::vector<std::string>& vec, const std::string& content);
        std::vector<std::string> splitString(const std::string& input, char delimiter);
};
#endif
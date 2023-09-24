/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:02:01 by cleticia          #+#    #+#             */
/*   Updated: 2023/09/23 22:24:49 by lfranca-         ###   ########.fr       */
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
		bool containsOnlyLetters(const std::string& str); //contemApenasLetras
        bool containsOnlyNumbers(const std::string& str);
        std::vector<std::string> splitString(const std::string& input, char delimiter);
        bool isInVector(const std::vector<std::string>& vec, const std::string& content); //contains
        bool fileExists(std::string filePath);
        bool pathExists(const std::string &path);
};
#endif
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpGet.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 18:43:28 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/10 11:52:55 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _HTTP_GET_HPP_
#define _HTTP_GET_HPP_

#include "HeadersLibs.hpp"

class ValidGet
{

    public:
    
        ValidGet();
        ~ValidGet();
        ValidGet(const std::string& root);
        void setUri(const std::string& uri);

    private:
        
        std::string _root;
        std::string _uri;
        std::string _fullPath;

};
#endif

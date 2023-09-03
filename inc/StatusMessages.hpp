/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusMessages.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/24 18:31:43 by cleticia          #+#    #+#             */
/*   Updated: 2023/08/24 20:45:36 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _STRUCTSTATUS_HPP_
#define _STRUCTSTATUS_HPP_

#include "HeadersLibs.hpp"

struct StatusMessages
{
    std::map<int, std::string> statusMessages;
    StatusMessages();
    std::string getMessage(int statusCode);
};

#endif
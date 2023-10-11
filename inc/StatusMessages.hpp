/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusMessages.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/24 18:31:43 by cleticia          #+#    #+#             */
/*   Updated: 2023/10/11 15:21:29 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _STRUCTSTATUS_HPP_
#define _STRUCTSTATUS_HPP_

#include "HeadersLibs.hpp"

struct StatusMessages
{
    StatusMessages();
    std::string getMessage(int statusCode);
    std::map<int, std::string> statusMessages;
};

#endif

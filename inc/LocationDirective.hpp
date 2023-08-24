/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationDirective.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 20:39:26 by lfranca-          #+#    #+#             */
/*   Updated: 2023/08/24 16:48:56 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _HPP_LOCATIONDIRECTIVE_
#define _HPP_LOCATIONDIRECTIVE_
#include "../inc/HeadersLibs.hpp"

class LocationDirective
{

	public:
		
		LocationDirective(void);
		~LocationDirective(void);
		void addDirective(std::string& directiveName, std::string& value); //se houver mais de um valor pra diretiva, adiciona um por um, só passar o mesmo nome de diretiva

	private:
	
		std::map<std::string, std::vector< std::string > > _directives;
};


#endif

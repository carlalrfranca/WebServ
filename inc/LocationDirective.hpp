/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationDirective.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 20:39:26 by lfranca-          #+#    #+#             */
/*   Updated: 2023/08/30 19:58:46 by cleticia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _HPP_LOCATIONDIRECTIVE_
#define _HPP_LOCATIONDIRECTIVE_
#include "HeadersLibs.hpp"

class LocationDirective
{

	public:
		
		LocationDirective(void);
		~LocationDirective(void);
		void addDirective(std::string& directiveName, std::string& value); //se houver mais de um valor pra diretiva, adiciona um por um, só passar o mesmo nome de diretiva
		std::map<std::string, std::vector<std::string > >getDirectives(void) const;
		std::vector<std::string> getSpecificDirective(std::string directiveName) const;

	private:
	
		std::map<std::string, std::vector< std::string > > _directives;
};


#endif

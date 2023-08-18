/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationDirective.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 20:39:26 by lfranca-          #+#    #+#             */
/*   Updated: 2023/08/18 00:34:55 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _HPP_LOCATIONDIRECTIVE_
#define _HPP_LOCATIONDIRECTIVE_

#include <iostream>
#include <map>
#include <vector>
#include <string>

class LocationDirective {

	public:
		
		LocationDirective(void);
		~LocationDirective(void);
		void addDirective(std::string& directiveName, std::string& value); //se houver mais de um valor pra diretiva, adiciona um por um, só passar o mesmo nome de diretiva

		// getValueDirective
		// getNameDirective

	private:
		std::map<std::string, std::vector< std::string > > _directives;
};


#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationDirective.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 20:39:26 by lfranca-          #+#    #+#             */
/*   Updated: 2023/10/10 11:53:17 by lfranca-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _HPP_LOCATIONDIRECTIVE_
#define _HPP_LOCATIONDIRECTIVE_

#include "HeadersLibs.hpp"

class LocationDirective
{
	public:
		
		LocationDirective();
		~LocationDirective();
		void addDirective(std::string& directiveName, std::string& value);
		std::map<std::string, std::vector<std::string > >getDirectives(void) const;
		std::vector<std::string> getSpecificDirective(std::string directiveName) const;

	private:
	
		std::map<std::string, std::vector< std::string > > _directives;
};


#endif

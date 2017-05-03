/*
 * =====================================================================================
 *
 *       Filename:  token.cpp
 *
 *    Description:  Parsing icinga configuration and showing connections between its objects
 *
 *        Version:  1.0
 *        Created:  17.04.2017 22:51:10
 *       Revision: 
 *       Compiler:  gcc
 *
 *         Author:  Michał Glinka
 *   Organization:  Politechnika Warszawska
 *
 * =====================================================================================
 */
#include "token.h"
#include <stdlib.h>

void Token::printInfo() const
{
	std::cout<<"Value: " << value << std::endl;
}

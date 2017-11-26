/**
	@file 		smf_parser.h
	@author 	Ravi Agrawal (ravia@sfu.ca)
	@date		November 7, 2017
	@version 	1.0

	@brief		CMPT-764: Assignment 2, Header file for SMF parser.
*/

#ifndef SMF_PARSER_H
#define SMF_PARSER_H

#include <mesh.h>
#include <iostream>
#include <fstream>
#include <cstdlib>

using std::cout;
using std::cin;
using std::cerr;
using std::endl;

using std::ifstream;
using std::ofstream;

Mesh *parseSmfFile(string);
void writeSmfFile(Mesh *mesh, string);

#endif
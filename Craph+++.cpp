// Craph+++.cpp : Defines the entry point for the console application.
//

//mandatory
#include "stdafx.h"

//utility
#include <conio.h>
#include <iostream>
#include "timer.h"

//testing
#include "ast.h"
#include "tok.h"
#include "parser.h"
#include "interpreter.h"

int main()
	{
	//TODO all assignment variations must become right associative
	std::stringstream ss;

	ss << R"(
	int max(int a, int b)
		{
		if (a > b) return a;
		else return b;
		}

	int due = 2;
	int tre = 3;

	print "POWER";
	int pow(int a, int b)
		{
		int ret = 1;
		for (int i = 0; i < b; ++i)
			{
			ret *= a;
			}
		return ret;
		}

	print "pow due tre: ";
	print pow(due, tre);
	print "pow tre due: ";
	print pow(tre, due);
	print "pow max max: ";
	print pow(max(due, tre), max(due, tre));

	print "SWAPPING";
	void swap(int& a, int& b)
		{
		int tmp = b;
		b = a;
		a = tmp;
		}
		
	swap(due, tre);
	print "due: ";
	print due;
	print "tre: ";
	print tre;

	)" << std::endl;

	std::vector<const std::string*> keywords;
	mylang::symbol_table st;
	mylang::tok::tokenizer tokenizer(ss, st);

	/*while(true)
		{
		mylang::tok::token tok = tokenizer.next();
		std::cout << tok;
		if (tok.t == mylang::tok::type::_EOF)
			{
			break;
			}
		}*/
	
	std::cout << std::endl << "==================== Parsing ====================" << std::endl;
	mylang::parser parser(tokenizer);
	mylang::ast::stm::block* ast = parser.parse();
	std::cout << std::endl << "==================== Interpreting ====================" << std::endl;
	mylang::inte::interpreter interpreter(ast);
	interpreter.interpret();

	std::cout << std::endl << "==================== Done ====================" << std::endl;
	_getch();
    return 0;
	}


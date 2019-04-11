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
	ss << "int test(int a)                            " << std::endl;
	ss << "	{                                    " << std::endl;
	ss << "	string inner(int a)                  " << std::endl;
	ss << "		{                                " << std::endl;
	ss << "		string ret = \"[\";              " << std::endl;
	ss << "		for(int i = a-3; i>= 0; --i)     " << std::endl;
	ss << "			{ ret += \"a\"; }            " << std::endl;
	ss << "		return ret + \"]\";              " << std::endl;
	ss << "		}                                " << std::endl;
	ss << "	                                     " << std::endl;
	ss << "	string tmp = inner(a);               " << std::endl;
	ss << "	print tmp;                           " << std::endl;
	ss << "	return(tmp == \"[aaa]\");            " << std::endl;
	ss << "	}                                    " << std::endl;
	ss << "                                      " << std::endl;
	ss << "print \"Hello World!\";               " << std::endl;
	ss << "if(test(3))                           " << std::endl;
	ss << "	{                                    " << std::endl;
	ss << "	print \"How are you?\";              " << std::endl;
	ss << "	}                                    " << std::endl;
	ss << "else                                  " << std::endl;
	ss << "	{                                    " << std::endl;
	ss << "	print \"Who are you?\";              " << std::endl;
	ss << "	}                                    " << std::endl;
	ss << "if(test(5))                           " << std::endl;
	ss << "	{                                    " << std::endl;
	ss << "	print \"How are you?\";              " << std::endl;
	ss << "	}                                    " << std::endl;
	ss << "else                                  " << std::endl;
	ss << "	{                                    " << std::endl;
	ss << "	print \"Who are you?\";              " << std::endl;
	ss << "	}                                    " << std::endl;
	ss << "print \"Goodbye World!\";             " << std::endl;
	ss << "inner(5);                             " << std::endl;


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


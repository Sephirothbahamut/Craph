#pragma once
#include "ast.h"

#include <vector>
#include <list>
#include <initializer_list>

namespace mylang
	{
	class parser
		{
		tok::tokenizer* tokenizer;
		tok::token curr;
		tok::token prev;


		// ======================================================================================
		// =====================================  support   =====================================
		// ======================================================================================
		bool match(std::initializer_list<tok::type> v);
		bool check(tok::type t);
		bool end();
		tok::token next();
		tok::token consume(tok::type t, std::string err = "Undefined error.");
		void error(std::string err = "Undefined error.");
		//parsing
		// ======================================================================================
		// ===================================== statements =====================================
		// ======================================================================================
		ast::stm::root* statement();
		ast::stm::block* stm_block();
		ast::stm::print* stm_print();
		ast::stm::ret* stm_return();
		ast::stm::expression* stm_expression();
		ast::stm::root* stm_declaration();
		ast::stm::dec::root* stm_decl_var();
		//ast::stm::dec::func* stm_decl_arr(tok::token type, tok::token name);
		ast::stm::dec::func* stm_decl_fun(tok::token type, tok::token name);
		ast::stm::_if* stm_if();
		ast::stm::_while* stm_while();
		ast::stm::root* stm_for();


		// ======================================================================================
		// ===================================== expression =====================================
		// ======================================================================================
		ast::exp::root* expression();

		ast::exp::root* exp_assignment();
		//ast::exp::root* exp_conditional(); TODO ternary operator (x?y:z)
		ast::exp::root* exp_logical_or();
		ast::exp::root* exp_logical_and();
		ast::exp::root* exp_inclusive_or();
		ast::exp::root* exp_exclusive_or();
		ast::exp::root* exp_bit_and();

		ast::exp::root* exp_equality();
		ast::exp::root* exp_comparison();
		
		ast::exp::root* exp_shifting();
		
		ast::exp::root* exp_addition();
		ast::exp::root* exp_multiplication();
		ast::exp::root* exp_unary();
		ast::exp::root* exp_call();
		ast::exp::root* exp_call_args(ast::exp::root* fname);
		ast::exp::root* exp_primary();

		public:
			parser(mylang::tok::tokenizer& tokenizer);
			~parser();

			ast::stm::block* parse();
		};

	}
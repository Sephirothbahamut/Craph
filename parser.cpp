#include "stdafx.h"
#include "parser.h"
#include <conio.h>
namespace mylang
	{
	// ======================================================================================
	// =====================================  support   =====================================
	// ======================================================================================
	bool parser::match(std::initializer_list<tok::type> v)
		{
		for (tok::type t : v)
			{
			if (check(t))
				{
				next();
				return true;
				}
			}
		return false;
		}
	bool parser::check(tok::type t)
		{
		if (end())
			{
			return false;
			}
		return curr.t == t;
		}
	bool parser::end()
		{
		return curr.t == tok::type::_EOF;
		}
	tok::token parser::next()
		{
		if (!end())
			{
			prev = curr;
			curr = tokenizer->next();
			}
		return prev;
		}

	tok::token parser::consume(tok::type t, std::string err)
		{
		if (check(t))
			{
			next();
			return prev;
			}

		std::cerr << "Parsing error at line: " << curr.line << "." << std::endl;
		std::cerr << err << std::endl;
		std::cerr << "Found " << curr << " instead." << std::endl;
		_getch();
		exit(0);
		}

	void parser::error(std::string err)
		{
		std::cerr << "Parsing error at line: " << curr.line << "." << std::endl;
		std::cerr << err << std::endl;
		std::cerr << "Found " << curr << " instead." << std::endl;
		_getch();
		exit(0);
		}

	// ======================================================================================
	// ===================================== statements =====================================
	// ======================================================================================
	ast::stm::root * parser::statement()
		{
		if (match({ tok::open_curly })) { return stm_block(); }
		if (match({ tok::_if })) { return stm_if(); }
		if (match({ tok::_while })) { return stm_while(); }
		if (match({ tok::_for })) { return stm_for(); }
		if (match({ tok::_print })) { return stm_print(); }
		if (match({ tok::_return })) { return stm_return(); }
		return stm_expression();
		}

	ast::stm::block * parser::stm_block()
		{
		std::list<ast::stm::root*>* statements = new std::list<ast::stm::root*>();
		while (!check(tok::clos_curly) && !end())
			{
			statements->push_back(stm_declaration());
			}
		consume(tok::clos_curly, "Block parenthesis has not been closed.");

		return new ast::stm::block(statements);
		}

	ast::stm::print * parser::stm_print()
		{
		ast::exp::root* exp = expression();
		consume(tok::type::semicolon, "Expected ';' after expression.");
		return new ast::stm::print(exp);
		}
	ast::stm::ret * parser::stm_return()
		{
		ast::exp::root* exp = expression();
		consume(tok::type::semicolon, "Expected ';' after expression.");
		return new ast::stm::ret(exp);
		}
	ast::stm::expression * parser::stm_expression()
		{
		ast::exp::root* exp = expression();
		consume(tok::type::semicolon, "Expected ';' after expression.");
		return new ast::stm::expression(exp);
		}

	ast::stm::root * parser::stm_declaration()
		{
		if (match({ tok::type::_int, tok::type::_float, tok::type::_string, tok::type::_void })) { return stm_decl_var(); }
		return statement();
		}

	ast::stm::dec::root * parser::stm_decl_var()
		{
		tok::token type = prev;
		tok::token name = consume(tok::type::identifier, "Expected identifier.");

		if (match({ tok::open_round })) { return stm_decl_fun(type, name); }

		ast::exp::root* initializer = nullptr;
		if (match({ tok::type::assign })) { initializer = expression(); }

		consume(tok::type::semicolon, "Expected ';' after variable declaration.");
		return new ast::stm::dec::var(type, name, initializer);
		}
	ast::stm::dec::func * parser::stm_decl_fun(tok::token type, tok::token name)
		{
		std::list<ast::stm::func_arg_decl*>* args = new std::list<ast::stm::func_arg_decl*>();
		if (!check(tok::clos_round))
			{
			do
				{
				tok::token type;
				if (match({ tok::_int, tok::type::_float, tok::_string })) { type = prev; }
				else { error("Expected argument type."); }

				tok::token name = consume(tok::identifier, "Expected argument name.");

				args->push_back(new ast::stm::func_arg_decl(type, name));
				} while (match({ tok::comma }));
			}
		consume(tok::clos_round, "Expected ')' after arguments.");
		consume(tok::open_curly, "Expected '{' before function body.");
		ast::stm::block* body = stm_block();
		return new ast::stm::dec::func(type, name, args, body);
		}

	ast::stm::_if * parser::stm_if()
		{
		unsigned long long int line = prev.line;
		consume(tok::open_round, "Expected '(' after \"if\".");
		ast::exp::root* exp = expression();
		consume(tok::clos_round, "Expected ')' after if condition.");

		ast::stm::root* then = statement();
		ast::stm::root* _else = nullptr;
		if (match({ tok::_else }))
			{
			_else = statement();
			}
		return new ast::stm::_if(exp, then, line, _else);
		}
	ast::stm::_while* parser::stm_while()
		{
		unsigned long long int line = prev.line;
		consume(tok::open_round, "Expected '(' after \"while\".");
		ast::exp::root* exp = expression();
		consume(tok::clos_round, "Expected ')' after while condition.");

		ast::stm::root* then = statement();
		return new ast::stm::_while(exp, then, line);
		}
	ast::stm::root * parser::stm_for()
		{
		unsigned long long int line = prev.line;
		consume(tok::open_round, "Expected '(' after \"for\".");

		//first part
		ast::stm::root* decl;
		if (match({ tok::semicolon })) { decl = nullptr; }
		else if (match({ tok::_int, tok::_float, tok::_string })) { decl = stm_decl_var(); }
		else { decl = stm_expression(); }

		//second part
		ast::exp::root* cond = nullptr;
		if (!check(tok::semicolon)) { cond = expression(); }
		consume(tok::semicolon, "Expected ';' after for loop condition.");

		//third part
		ast::exp::root* exp = nullptr;
		if (!check(tok::clos_round)) { exp = expression(); }
		consume(tok::clos_round, "Expected ')' after loop expression.");

		//body
		ast::stm::root* body = statement();

		if (exp != nullptr)
			{
			std::list<ast::stm::root*>* blockbody = new std::list<ast::stm::root*>();
			blockbody->push_back(body);
			blockbody->push_back(new ast::stm::expression(exp));
			body = new ast::stm::block(blockbody);
			}
		
		if (cond == nullptr) { cond = new ast::exp::_int(1); }
		body = new ast::stm::_while(cond, body, line);

		if (decl != nullptr)
			{
			std::list<ast::stm::root*>* blockbody = new std::list<ast::stm::root*>();
			blockbody->push_back(decl);
			blockbody->push_back(body);
			body = new ast::stm::block(blockbody);
			}

		return body;
		}
	// ======================================================================================
	// ===================================== expression =====================================
	// ======================================================================================
	ast::exp::root * parser::expression()
		{
		return exp_assignment();
		}
	ast::exp::root * parser::exp_assignment()
		{
		ast::exp::root* exp = exp_logical_or();
		while (match({tok::type::assign, tok::type::assign_bit_and, tok::type::assign_bit_or, 
			tok::type::assign_sum, tok::type::assign_sub, tok::type::assign_mul, tok::type::assign_div, 
			tok::type::assign_mod, tok::type::assign_excl_or}))
			{
			tok::token op = prev;
			ast::exp::root* right = exp_logical_or();
			exp = new ast::exp::bin(exp, op, right);
			}
		return exp;
		}
	ast::exp::root * parser::exp_logical_or()
		{
		ast::exp::root* exp = exp_logical_and();
		while (match({tok::type::_or}))
			{
			tok::token op = prev;
			ast::exp::root* right = exp_logical_and();
			exp = new ast::exp::bin(exp, op, right);
			}
		return exp;
		}
	ast::exp::root * parser::exp_logical_and()
		{
		ast::exp::root* exp = exp_inclusive_or();
		while (match({tok::type::_and}))
			{
			tok::token op = prev;
			ast::exp::root* right = exp_inclusive_or();
			exp = new ast::exp::bin(exp, op, right);
			}
		return exp;
		}
	ast::exp::root * parser::exp_inclusive_or()
		{
		ast::exp::root* exp = exp_exclusive_or();
		while (match({tok::type::bit_or}))
			{
			tok::token op = prev;
			ast::exp::root* right = exp_exclusive_or();
			exp = new ast::exp::bin(exp, op, right);
			}
		return exp;
		}
	ast::exp::root * parser::exp_exclusive_or()
		{
		ast::exp::root* exp = exp_bit_and();
		while (match({tok::type::_excl_or}))
			{
			tok::token op = prev;
			ast::exp::root* right = exp_bit_and();
			exp = new ast::exp::bin(exp, op, right);
			}
		return exp;
		}
	ast::exp::root * parser::exp_bit_and()
		{
		ast::exp::root* exp = exp_equality();
		while (match({tok::type::bit_and}))
			{
			tok::token op = prev;
			ast::exp::root* right = exp_equality();
			exp = new ast::exp::bin(exp, op, right);
			}
		return exp;
		}
	ast::exp::root * parser::exp_equality()
		{
		ast::exp::root* exp = exp_comparison();
		while (match({tok::type::eq, tok::type::neq}))
			{
			tok::token op = prev;
			ast::exp::root* right = exp_comparison();
			exp = new ast::exp::bin(exp, op, right);
			}
		return exp;
		}
	ast::exp::root * parser::exp_comparison()
		{
		ast::exp::root* exp = exp_shifting();
		while (match({tok::type::lt, tok::type::lte, tok::type::gt, tok::type::gte}))
			{
			tok::token op = prev;
			ast::exp::root* right = exp_shifting();
			exp = new ast::exp::bin(exp, op, right);
			}
		return exp;
		}
	ast::exp::root * parser::exp_shifting()
		{
		ast::exp::root* exp = exp_addition();
		while (match({tok::type::shift_left, tok::type::shift_right}))
			{
			tok::token op = prev;
			ast::exp::root* right = exp_addition();
			exp = new ast::exp::bin(exp, op, right);
			}
		return exp;
		}
	ast::exp::root * parser::exp_addition()
		{
		ast::exp::root* exp = exp_multiplication();
		while (match({tok::type::sum, tok::type::sub}))
			{
			tok::token op = prev;
			ast::exp::root* right = exp_multiplication();
			exp = new ast::exp::bin(exp, op, right);
			}
		return exp;
		}
	ast::exp::root * parser::exp_multiplication()
		{
		ast::exp::root* exp = exp_unary();
		while (match({tok::type::mul, tok::type::div, tok::type::mod}))
			{
			tok::token op = prev;
			ast::exp::root* right = exp_unary();
			exp = new ast::exp::bin(exp, op, right);
			}
		return exp;
		}
	ast::exp::root * parser::exp_unary()
		{
		if (match({tok::type::sub, tok::type::_not, tok::type::inc, tok::type::dec}))
			{
			tok::token op = prev;
			ast::exp::root* right = exp_unary();
			return new ast::exp::un(op, right);
			}
		return exp_call();
		}
	ast::exp::root * parser::exp_call()
		{
		ast::exp::root* exp = exp_primary();

		while (true)
			{
			if (match({ tok::open_round })) { exp = exp_call_args(exp); }
			else break;
			}

		return exp;
		}
	ast::exp::root * parser::exp_call_args(ast::exp::root* fname)
		{
		std::list<ast::exp::root*>* args = new std::list<ast::exp::root*>();
		unsigned long long int line = prev.line;
		if (!check(tok::clos_round))
			{
			do
				{
				args->push_back(expression());
				}
			while (match({tok::comma}));
			}

		tok::token tok = consume(tok::clos_round, "Expected ')' after function arguments.");
		return new ast::exp::call(fname, args, line);
		}
	ast::exp::root * parser::exp_primary()
		{
		if (match({ tok::type::_true }))  return new ast::exp::_int(1); 
		if (match({ tok::type::_false }))  return new ast::exp::_int(0); 
		if (match({ tok::type::value_int })) return new ast::exp::_int(prev.i); 
		if (match({ tok::type::value_float }))  return new ast::exp::_float(prev.f); 
		if (match({ tok::type::value_string }))  return new ast::exp::_string(*(prev.s)); 
		if (match({ tok::type::identifier }))  return new ast::exp::var(prev.s, prev.line); 

		if (match({tok::type::open_round})) {
			ast::exp::root* exp = expression();
			consume(tok::type::clos_round, "Expected ')' after expression.");
			return new ast::exp::grouping(exp);
			}
		}

	parser::parser(mylang::tok::tokenizer& tokenizer) : tokenizer(&tokenizer) {}
	parser::~parser() {}
	ast::stm::block* parser::parse()
		{
		curr = tokenizer->next();

		std::list<ast::stm::root*>* declarations = new std::list<ast::stm::root*>();
		while (!end())
			{
			declarations->push_back(stm_declaration());
			}
		return new ast::stm::block(declarations);
		}
	}
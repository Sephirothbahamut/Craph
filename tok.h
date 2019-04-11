#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <set>

namespace mylang
	{
	class symbol_table : public std::set<std::string> {};

	namespace tok
		{
		enum type
			{
			_print,
			identifier, value_int, value_float, value_string,
			open_curly, clos_curly, open_round, clos_round, open_squar, clos_squar, semicolon, comma,
			sum, sub, div, mul, mod, inc, dec,
			shift_left, shift_right, bit_or, bit_and, _excl_or,
			eq, neq, gt, gte, lt, lte, assign,

			assign_sum, assign_sub, assign_mul, assign_div, assign_mod, assign_excl_or, assign_bit_or, assign_bit_and,
			double_colon /* :: */, colon,

			_int, _float, _string, _var, 
			_and, _auto, _bool, _break, _case, _char, _const, _continue, _default, _delete, _do, _double, _else, _enum, _false, _for, _goto, _if, _new, _not, _nullptr, _or, _return, _switch, _true, _void, _while,
			_EOF
			};

		class token
			{
			public:
			friend class tokenizer;
			friend class parser;

			tok::type t;
			unsigned long long int line;
			union
				{
				const std::string* s;
				double f;
				long int i;
				};

			token(const std::string* s, tok::type t, const unsigned long int line);
			token(const double n, const unsigned long int line);
			token(const long int n, const unsigned long int line);
			token(const std::string* n, const unsigned long int line);
			token();

			friend std::ostream& operator<< (std::ostream& stream, const token& t);
			};

		class tokenizer
			{
			public:
			std::stringstream* const in;
			symbol_table* const st; //const messo lì rende costante solo il puntatore e non l'oggetto puntato

			tokenizer(std::stringstream& in, symbol_table& st);
			~tokenizer();

			bool is_alpha(char c);
			bool is_num(char c);
			bool is_alphanum(char c);

			unsigned long long int index = 0;
			unsigned long long int line = 1;
			void get();
			char c;
			token next();
			token read_id();
			token read_num();
			token read_string();

			void comment_skip_line();
			void comment_skip_section();

			};
		};

	}
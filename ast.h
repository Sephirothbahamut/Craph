#pragma once
#include <string>
#include <iostream>
#include <list>
#include "tok.h"
#include "interpreter.h"


namespace mylang
	{

	namespace ast
		{
		inte::var::type tok_type_to_var_type(tok::token tok);

		class root 
			{
			private:
				virtual void _print(std::ostream& stream);

			public:
				friend std::ostream& operator<< (std::ostream& stream, ast::root& r);

			};

		namespace exp
			{
			class root : public ast::root 
				{
				public:
					virtual inte::var::root* eval(inte::function_scope* fs);
				};
			class grouping : public root
				{
				private: 
					root* exp;
					void _print(std::ostream& stream);
				public: 
					grouping(root* exp);
					inte::var::root* eval(inte::function_scope* fs);
				};

			class _int : public root
				{
				private:
					long int val;
					void _print(std::ostream& stream);
				public:
					_int(long int val);
					inte::var::root* eval(inte::function_scope* fs);
				};
			class _float : public root
				{
				private:
					double val;
					void _print(std::ostream& stream);
				public:
					_float(double val);
					inte::var::root* eval(inte::function_scope* fs);
				};
			class _string : public root
				{
				private:
					std::string val;
					void _print(std::ostream& stream);
				public:
					_string(std::string val);
					inte::var::root* eval(inte::function_scope* fs);
				};
			class var : public root
				{
				private: 
					unsigned long long int line;
					const std::string* name;
					void _print(std::ostream& stream);
				public: 
					var(const std::string* name, unsigned long long int line);
					inte::var::root* eval(inte::function_scope* fs);
				};

			class un : public root
				{
				private:
					tok::token op;
					root * right;
					void _print(std::ostream& stream);
				public:
					un(tok::token op, root* right);
					inte::var::root* eval(inte::function_scope* fs);
				};
			class bin : public root
				{
				private: 
					root * left;
					root* right;
					tok::token op;
					void _print(std::ostream& stream);
				public:
					bin(root* left, tok::token op, root* right);
					inte::var::root* eval(inte::function_scope* fs);
				};
			/* TODO ternary operator (x?y:z)
			class ter : public root
				{
				private:
					root * first;
					root * second;
					root * third;
					tok::token op1;
					tok::token op2;
					void _print(std::ostream& stream);
				public:
					ter(root * first, tok::token op1, root * second, tok::token op2, root * third);
				};*/

			class call : public root
				{
				private:
					exp::root* fname;
					std::list<ast::exp::root*>* args;
					void _print(std::ostream& stream);
					unsigned long long int line;
				public: 
					call(exp::root* fname, std::list<ast::exp::root*>* args, unsigned long long int line);
					inte::var::root* eval(inte::function_scope* fs);
				};
			}

		namespace stm
			{
			class root : public ast::root 
				{
				public:
					virtual void exec(inte::function_scope* fs);
				};

			class block : public root
				{
				friend class function_scope;
				private:
					void _print(std::ostream& stream);
				public:
					std::list<root*>* stms;//TODO understand why as private function_scope cannot access it despite being friend
					block(std::list<root*>* stms);
					void exec(inte::function_scope* fs);
				};

			class func_arg_decl : public root
				{
				friend class mylang::inte::function_scope;
				friend class mylang::ast::exp::call;
				private:
					tok::token name;
					inte::var::type type;
					void _print(std::ostream& stream);
				public: 
					func_arg_decl(tok::token type, tok::token name);
					void exec(inte::function_scope* fs);
				};
			
			class ret : public root
				{
				private:
					exp::root* exp;
					void _print(std::ostream& stream);
				public: 
					ret(exp::root* exp);
					void exec(inte::function_scope* fs);
				};
			
			class print : public root
				{
				private:
					exp::root * exp;
					void _print(std::ostream& stream);
				public: 
					print(exp::root* exp);
					void exec(inte::function_scope* fs);
				};
			
			class expression : public root
				{
				private:
					exp::root * exp;
					void _print(std::ostream& stream);
				public: 
					expression(exp::root* exp);
					void exec(inte::function_scope* fs);
				};
			
			class _if : public root
				{
				private: 
					unsigned long long int line;
					exp::root* cond;
					root* then;
					root* _else;
					void _print(std::ostream& stream);
				public: 
					_if(exp::root* cond, root* then, unsigned long long int line, root* _else = nullptr);
					void exec(inte::function_scope* fs);
				};
			class _while: public root
				{
				private:
					unsigned long long int line;
					exp::root* cond;
					root* then;
					void _print(std::ostream& stream);
				public: 
					_while(exp::root* cond, root* then, unsigned long long int line);
					void exec(inte::function_scope* fs);
				};

			namespace dec
				{
				class root : public stm::root {};

				class func : public root
					{
					friend class mylang::inte::function_scope;
					friend class mylang::ast::exp::call;
					private:
						inte::var::type type;
						tok::token name;
						std::list<func_arg_decl*>* args;
						block* stms;
						void _print(std::ostream& stream);
					public: 
						func(tok::token type, tok::token name, std::list<func_arg_decl*>* args, block* stms);
						func(block* stms);
						void exec(inte::function_scope* fs);
					};
				class var : public root
					{
					private:
						inte::var::type type;
						tok::token name;
						exp::root* init;
						void _print(std::ostream& stream);
					public: 
						var(tok::token type, tok::token name, exp::root* init);
						void exec(inte::function_scope* fs);
					};
				}
			}

		};
	}
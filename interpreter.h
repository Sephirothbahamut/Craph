#pragma once
#include <map>
#include <string>
#include <stack>
#include <list>
#include <conio.h>

namespace mylang
	{
	namespace tok
		{
		class token;
		}
	namespace ast
		{
		namespace exp
			{
			class call;
			}
		namespace stm
			{
			class root;
			class block;
			class func_arg_decl;
			class ret;
			namespace dec
				{
				class func;
				}
			}
		}

	namespace inte
		{
		namespace var
			{
			enum type { t_void, t_auto, t_int, t_float, t_str, t_fun };
			class root
				{
				private:
					virtual void _print(std::ostream& stream);
				public: 
					type t;
					root(type t);
					friend std::ostream& operator<< (std::ostream& stream, var::root& r);
				};

			class _int : public root
				{
				private:
					void _print(std::ostream& stream);
				public:
					long int value;
					_int();
				};
			class _float : public root
				{
				private:
					void _print(std::ostream& stream);
				public:
					double value;
					_float();
				};
			class _string : public root
				{
				private:
					void _print(std::ostream& stream);
				public:
					std::string value;
					_string();
				};

			class fun : public root
				{
				private:
					void _print(std::ostream& stream);
				public: 
					ast::stm::dec::func* function;
					fun();
				};
			}

		class function_scope
			{
			friend class ast::exp::call;
			friend class ast::stm::ret; //allows ret to change the iterator to the end of instructions list
			friend class ast::stm::block; //allows ret to change the iterator to the end of instructions list
			private:
				function_scope * global;
				std::map<const std::string*, std::stack<var::root*> > variables;
				std::stack<std::list<const std::string*> > var_allocs;

				ast::stm::dec::func* f; 
				var::root * retval = nullptr;
			public:
				void block_in();
				void block_out();

				void alloc(const std::string* name, var::root* value);
				void dealloc(const std::string* name);
				var::root* get(const std::string* name);

				function_scope(ast::stm::dec::func* f, function_scope* global);
				function_scope(ast::stm::block* b);
				var::root* exec(std::list<var::root*>* args);
				var::root* exec();
			};

		class interpreter
			{
			private:
				function_scope global;

			public:
				interpreter(ast::stm::block* ast);
				~interpreter();

				double interpret();
			};

		void error_op(tok::token operand, var::root* v1, var::root* v2, var::root* v3);
		void error_var(var::root* var, std::string str, unsigned long long int line);
		void error(std::string err, unsigned long long int line);
		void error_args(std::list<ast::stm::func_arg_decl*>* a, std::list<var::root*>* c, unsigned long long int line);
		}
	}


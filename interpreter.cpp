#include "stdafx.h"
#include "ast.h"


namespace mylang
	{
	namespace inte
		{
		namespace var
			{
			void root::_print(std::ostream & stream) const { stream << "Undefined variable"; }
			std::ostream & operator<<(std::ostream & stream, var::root & r) { r._print(stream); return stream; }
			root::root(type t) : t(t) {}

			void _int::_print(std::ostream & stream) const { stream << "Integer (" << value << ")"; }
			root * _int::copy() const { return new _int(value); }
			_int::_int() : root(t_int) {}
			_int::_int(int value) : root(t_int), value(value) {}
			void _float::_print(std::ostream & stream) const { stream << "Float (" << value << ")"; }
			root * _float::copy() const { return new _float(value); }
			_float::_float() : root(t_float) {}
			_float::_float(float value) : root(t_int), value(value) {}
			void _string::_print(std::ostream & stream) const { stream << "String (" << value << ")"; }
			root * _string::copy() const { return new _string(value); }
			_string::_string() : root(t_str) {}
			_string::_string(std::string value) : root(t_int), value(value) {}

			void fun::_print(std::ostream & stream) const { stream << "Function (" << *function << ")"; }
			root * fun::copy() const { return new fun(); }
			fun::fun() : root(t_fun) {}
			}

		void function_scope::block_in() { var_allocs.push(std::list<const std::string*>()); }
		void function_scope::block_out()
			{ 
			for (auto v : var_allocs.top()) { dealloc(v); }
			var_allocs.pop(); 
			}

		void function_scope::alloc(const std::string * name, var::root * value)
			{
			var_allocs.top().push_back(name);
			variables[name].push(value);
			}
		void function_scope::dealloc(const std::string * name)
			{
			variables[name].pop();
			}

		var::root * function_scope::get(const std::string * name)
			{
			auto it = variables.find(name);
			if (it != variables.end())
				{
				return (*it).second.top();
				}
			if (global != this)
				{
				auto it = global->variables.find(name);
				if (it != global->variables.end())
					{
					return (*it).second.top();
					}
				return nullptr;
				}
			return nullptr;
			}
		
		function_scope::function_scope(ast::stm::dec::func * f, function_scope* global) : f(f), var_allocs(), variables(), global(global)
			{
			block_in();
			}

		function_scope::function_scope(ast::stm::block* b) : var_allocs(), variables(), global(this)
			{
			f = new ast::stm::dec::func(b);
			}

		var::root * function_scope::exec(std::list<var::root*>* args)
			{
			block_in();

			for (std::pair<std::list<ast::stm::func_arg_decl*>::iterator, std::list<var::root*>::iterator> it(f->args->begin(), args->begin());
				it.first != f->args->end();
				it.first++, it.second++)
				{
				if ((*(it.first))->reference) { alloc((*(it.first))->name.s, (*(it.second))); }
				else
					{
					alloc((*(it.first))->name.s, (**(it.second)).copy());
					}
				}

			for (auto stm : *(f->stms->stms))
				{
				(*stm).exec(this);
				if (retval != nullptr) { break; }
				}

			block_out();
			return retval;
			}
		var::root * function_scope::exec()
			{
			block_in();


			for (auto stm : *(f->stms->stms))
				{
				(*stm).exec(this);
				if (retval != nullptr) { break; }
				}
			block_out();
			return retval;
			}

		interpreter::interpreter(ast::stm::block* ast) : global(ast) {}

		interpreter::~interpreter()
			{
			}
		double interpreter::interpret()
			{
			global.exec();
			return 0;
			}
		void error_op(tok::token operand, var::root* v1, var::root* v2, var::root* v3)
			{
			std::cerr << "Runtime error at line " << operand.line << "." << std::endl;

			if (v2 == v3 && v2 == nullptr) { std::cerr << "Operand "; }
			else { std::cerr << "Operands "; }
			std::cerr << "of " << operand << " are not of compatible type." << std::endl;
			std::cerr << "#1 operand is: " << *v1 << std::endl;
			if (v2 != nullptr)
				{
				std::cerr << "#2 operand is: " << *v2 << std::endl;
				if (v3 != nullptr)
					{
					std::cerr << "#3 operand is: " << *v3 << std::endl;
					}
				}
			_getch();
			exit(0);
			}
		void error_var(var::root* var, std::string str, unsigned long long int line)
			{
			std::cerr << "Runtime error at line " << line << "." << std::endl;
			std::cerr << *var << " " << str;
			_getch();
			exit(0);
			}
		void error(std::string err, unsigned long long int line)
			{
			std::cerr << "Runtime error at line " << line << "." << std::endl;
			std::cerr << err << std::endl;
			_getch();
			exit(0);
			}
		void error_args(std::list<ast::stm::func_arg_decl*>* a, std::list<var::root*>* c, unsigned long long int line)
			{
			std::cerr << "Runtime error at line " << line << "." << std::endl;
			std::cerr << "Function expects " << a->size() << " arguments." << std::endl;
			for(auto arg : *a)
				{
				std::cerr << *arg << " ";
				}
			std::cerr << std::endl << "Found instead " << c->size() << " arguments." << std::endl;

			for (auto arg : *c)
				{
				std::cerr << *arg << " ";
				}
			_getch();
			exit(0);
			}
		}
	}
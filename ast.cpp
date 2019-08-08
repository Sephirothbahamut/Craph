#include "stdafx.h"
#include "ast.h"

namespace mylang
	{
	namespace ast
		{
		inte::var::type tok_type_to_var_type(tok::token tok)
			{
			switch (tok.t)
				{
				case tok::_int:		return inte::var::t_int;
				case tok::_float:	return inte::var::t_float;
				case tok::_string:	return inte::var::t_str;
				case tok::_void:	return inte::var::t_void;
				}
			}

		void root::_print(std::ostream & stream) {}
		std::ostream & operator<<(std::ostream & stream, root & r) { r._print(stream); return stream; }

		namespace exp
			{
			inte::var::root * root::eval(inte::function_scope * fs) { return nullptr; }

			grouping::grouping(root* exp) : exp(exp) {}
			inte::var::root * grouping::eval(inte::function_scope * fs) { return exp->eval(fs); }
			void grouping::_print(std::ostream& stream) { stream << "(" << *exp << ")"; }

			void _int::_print(std::ostream& stream) { stream << val; }
			_int::_int(long int val) : val(val) {}
			inte::var::root* _int::eval(inte::function_scope * fs)
				{
				inte::var::_int* n = new inte::var::_int();
				n->value = val;
				return n;
				}
			
			void _float::_print(std::ostream& stream) { stream << val; }
			_float::_float(double val) : val(val) {}
			inte::var::root* _float::eval(inte::function_scope * fs)
				{
				inte::var::_float* n = new inte::var::_float();
				n->value = val;
				return n;
				}
			
			void _string::_print(std::ostream& stream) { stream << val; }
			_string::_string(std::string val) : val(val) {}
			inte::var::root* _string::eval(inte::function_scope * fs)
				{
				inte::var::_string* n = new inte::var::_string();
				n->value = val;
				return n;
				}

			void var::_print(std::ostream& stream) { stream << *name; }
			var::var(const std::string* name, unsigned long long int line) : name(name), line(line) {}
			inte::var::root * var::eval(inte::function_scope * fs)
				{
				auto ret = fs->get(name);
				if (ret != nullptr) { return ret; }
				inte::error("Variable " + *name + " not found.", line);
				}

			void un::_print(std::ostream& stream) { stream << op << *right; }
			un::un(tok::token op, root* right) : right(right), op(op) {}
			inte::var::root * un::eval(inte::function_scope * fs)
				{
				inte::var::root* r = right->eval(fs);
				if (r->t == inte::var::t_int)
					{
					auto n = static_cast<inte::var::_int*>(r);
					switch (op.t)
						{
						case tok::sub: n->value = -(n->value); break;
						case tok::_not: n->value = !(n->value); break;
						case tok::inc: ++(n->value); break;
						case tok::dec: --(n->value); break;
						}
					return n;
					}
				if (r->t == inte::var::t_float)
					{
					auto n = static_cast<inte::var::_float*>(r);
					switch (op.t)
						{
						case tok::sub: n->value = -(n->value); break;
						case tok::_not: n->value = !(n->value); break;
						case tok::inc: ++(n->value); break;
						case tok::dec: --(n->value); break;
						}
					return n;
					}
				mylang::inte::error_op(op, r, nullptr, nullptr);
				}

			void bin::_print(std::ostream& stream) { stream << *left << op << *right; }
			bin::bin(root* left, tok::token op, root* right)
				: left(left), op(op), right(right) {}
			inte::var::root * bin::eval(inte::function_scope * fs)
				{
				inte::var::root* ll = left->eval(fs);
				inte::var::root* rr = right->eval(fs);

				if ((ll->t == inte::var::t_int) && (rr->t == inte::var::t_int))
					{
					auto lln = static_cast<inte::var::_int*>(ll);
					auto rrn = static_cast<inte::var::_int*>(rr);
					auto ret = new inte::var::_int();
					switch (op.t)
						{
						case tok::sum:      ret->value = lln->value + rrn->value; return ret;
						case tok::sub:      ret->value = lln->value - rrn->value; return ret;
						case tok::mul:      ret->value = lln->value * rrn->value; return ret;
						case tok::div:      ret->value = lln->value / rrn->value; return ret;
						case tok::mod:      ret->value = lln->value % rrn->value; return ret;

						case tok::assign:         ret->value = lln->value =  rrn->value; return ret;
						case tok::assign_sum:     ret->value = lln->value += rrn->value; return ret;
						case tok::assign_sub:     ret->value = lln->value -= rrn->value; return ret;
						case tok::assign_mul:     ret->value = lln->value *= rrn->value; return ret;
						case tok::assign_div:     ret->value = lln->value /= rrn->value; return ret;
						case tok::assign_mod:     ret->value = lln->value %= rrn->value; return ret;
						case tok::assign_bit_and: ret->value = lln->value &= rrn->value; return ret;
						case tok::assign_bit_or:  ret->value = lln->value |= rrn->value; return ret;
						case tok::assign_excl_or: ret->value = lln->value ^= rrn->value; return ret;

						case tok::_and:     ret->value = lln->value && rrn->value; return ret;
						case tok::_or:      ret->value = lln->value || rrn->value; return ret;
						case tok::bit_and:  ret->value = lln->value &  rrn->value; return ret;
						case tok::bit_or:   ret->value = lln->value |  rrn->value; return ret;
						case tok::_excl_or: ret->value = lln->value ^  rrn->value; return ret;
						case tok::eq:       ret->value = lln->value == rrn->value; return ret;
						case tok::neq:      ret->value = lln->value != rrn->value; return ret;
						case tok::gt:       ret->value = lln->value >  rrn->value; return ret;
						case tok::gte:      ret->value = lln->value >= rrn->value; return ret;
						case tok::lt:       ret->value = lln->value <  rrn->value; return ret;
						case tok::lte:      ret->value = lln->value <= rrn->value; return ret;
						}
					}
				else if (ll->t == inte::var::t_float && rr->t == inte::var::t_float)
					{
					int8_t a;
					auto lln = static_cast<inte::var::_float*>(ll);
					auto rrn = static_cast<inte::var::_float*>(rr);
					
					switch (op.t)
						{
						case tok::sum: {auto ret = new inte::var::_float(); ret->value = lln->value + rrn->value; return ret; }
						case tok::sub: {auto ret = new inte::var::_float(); ret->value = lln->value - rrn->value; return ret; }
						case tok::mul: {auto ret = new inte::var::_float(); ret->value = lln->value * rrn->value; return ret; }
						case tok::div: {auto ret = new inte::var::_float(); ret->value = lln->value / rrn->value; return ret; }

						case tok::assign:     {auto ret = new inte::var::_float(); ret->value = lln->value = rrn->value; return ret; }
						case tok::assign_sum: {auto ret = new inte::var::_float(); ret->value = lln->value += rrn->value; return ret; }
						case tok::assign_sub: {auto ret = new inte::var::_float(); ret->value = lln->value += rrn->value; return ret; }
						case tok::assign_mul: {auto ret = new inte::var::_float(); ret->value = lln->value += rrn->value; return ret; }
						case tok::assign_div: {auto ret = new inte::var::_float(); ret->value = lln->value += rrn->value; return ret; }

						case tok::_and: {auto ret = new inte::var::_int(); ret->value = lln->value && rrn->value; return ret; }
						case tok::_or:  {auto ret = new inte::var::_int(); ret->value = lln->value || rrn->value; return ret; }
						case tok::eq:   {auto ret = new inte::var::_int(); ret->value = lln->value == rrn->value; return ret; }
						case tok::neq:  {auto ret = new inte::var::_int(); ret->value = lln->value != rrn->value; return ret; }
						case tok::gt:   {auto ret = new inte::var::_int(); ret->value = lln->value >  rrn->value; return ret; }
						case tok::gte:  {auto ret = new inte::var::_int(); ret->value = lln->value >= rrn->value; return ret; }
						case tok::lt:   {auto ret = new inte::var::_int(); ret->value = lln->value <  rrn->value; return ret; }
						case tok::lte:  {auto ret = new inte::var::_int(); ret->value = lln->value <= rrn->value; return ret; }
						}
					}
				else if (ll->t == inte::var::t_str && rr->t == inte::var::t_str)
					{
					auto lln = static_cast<inte::var::_string*>(ll);
					auto rrn = static_cast<inte::var::_string*>(rr);

					switch (op.t)
						{
						case tok::sum: {auto ret = new inte::var::_string(); ret->value = lln->value + rrn->value; return ret; }

						case tok::assign:     {auto ret = new inte::var::_string(); ret->value = lln->value =  rrn->value; return ret; }
						case tok::assign_sum: {auto ret = new inte::var::_string(); ret->value = lln->value += rrn->value; return ret; }

						case tok::eq:  {auto ret = new inte::var::_int(); ret->value = lln->value == rrn->value; return ret; }
						case tok::neq: {auto ret = new inte::var::_int(); ret->value = lln->value != rrn->value; return ret; }
						case tok::gt:  {auto ret = new inte::var::_int(); ret->value = lln->value >  rrn->value; return ret; }
						case tok::gte: {auto ret = new inte::var::_int(); ret->value = lln->value >= rrn->value; return ret; }
						case tok::lt:  {auto ret = new inte::var::_int(); ret->value = lln->value <  rrn->value; return ret; }
						case tok::lte: {auto ret = new inte::var::_int(); ret->value = lln->value <= rrn->value; return ret; }
						}
					}
				mylang::inte::error_op(op, ll, rr, nullptr);
				return nullptr;
				}

			/* TODO ternary operator (x?y:z)
			void ter::_print(std::ostream & stream) { stream << *first << op1 << *second << op2 << *third; }
			ter::ter(root * first, tok::token op1, root * second, tok::token op2, root * third)
				: first(first), op1(op1), second(second), op2(op2), third(third) {}*/

			void call::_print(std::ostream & stream)
				{
				stream << &*fname;
				/*if (args->size())
					{
					stream << " of ";
					for (auto a : *args)
						{
						stream << *a << " ";
						}
					}*/
				}
			call::call(root* fname, unsigned long long int line) : fname(fname), line(line) {}
			void call::add_arg(root* arg) { args.push_back(arg); }
			
			inte::var::root * call::eval(inte::function_scope * fs)
				{
				inte::var::root* f = fname->eval(fs);
				if (f->t == inte::var::t_fun)
					{
					//evaluate call arguments
					std::list<inte::var::root*>* call_evaluated_args = new std::list<inte::var::root*>();
					for(auto arg : args)
						{
						call_evaluated_args->push_back(arg->eval(fs));
						}

					inte::var::fun* tmp = static_cast<inte::var::fun*>(f);
					ast::stm::dec::func* function = tmp->function;
					//checking arguments amoung
					if (args.size() == function->args->size())
						{
						std::list<ast::stm::func_arg_decl*>::iterator ita = function->args->begin();
						std::list<inte::var::root*>::iterator itc = call_evaluated_args->begin();
						//checking argument types match
						while (ita != function->args->end())
							{
							if ((*ita)->type != (*itc)->t) { goto OUT; }
							ita++;
							itc++;
							}

						//finally call it
						inte::function_scope call_fs(function, fs->global);
						return call_fs.exec(call_evaluated_args);
						}
				OUT:
					mylang::inte::error_args(function->args, call_evaluated_args, line);
					}
				mylang::inte::error_var(f, "is not a function. Expected function.", line);
				return nullptr;
				}

			}

		namespace stm
			{
			void root::exec(inte::function_scope * fs) {}

			void block::_print(std::ostream & stream)
				{
				stream << "{" << std::endl;
				for (auto s : *stms)
					{
					stream << *s;
					}
				stream << "}" << std::endl;
				}
			block::block(std::list<root*>* stms) : stms(stms) {}
			void block::exec(inte::function_scope * fs)
				{
				fs->block_in();
				for (auto stm : *stms)
					{
					(*stm).exec(fs);
					if (fs->retval != nullptr) { break; }
					}
				fs->block_out();
				}

			void func_arg_decl::_print(std::ostream & stream) { stream << type << " " << name; }
			func_arg_decl::func_arg_decl(tok::token type, bool reference, tok::token name) : type(tok_type_to_var_type(type)), reference(reference), name(name) {}
			void func_arg_decl::exec(inte::function_scope * fs) {}

			void ret::_print(std::ostream & stream) { stream << "return " << *exp << std::endl; }
			ret::ret(exp::root* exp) : exp(exp) {}
			void ret::exec(inte::function_scope * fs) { fs->retval = exp->eval(fs); }

			void print::_print(std::ostream& stream) { stream << "print " << *exp << std::endl; }
			print::print(exp::root * exp) : exp(exp) {}
			void print::exec(inte::function_scope * fs) { std::cout << *(exp->eval(fs)) << std::endl; }

			void expression::_print(std::ostream & stream) { stream << *exp << std::endl; }
			expression::expression(exp::root * exp) : exp(exp) {}
			void expression::exec(inte::function_scope * fs) { exp->eval(fs); }

			void _if::_print(std::ostream & stream) {
				stream << "if " << *cond << " then" << std::endl << *then << std::endl;
				if (_else != nullptr) { stream << "else" << std::endl << *_else << std::endl; }
				}
			_if::_if(exp::root * cond, root * then, unsigned long long int line, root * _else)
				: cond(cond), then(then), line(line), _else(_else) {}
			void _if::exec(inte::function_scope * fs)
				{
				auto eval = cond->eval(fs);
				switch (eval->t)
					{
					case inte::var::t_int:
						{
						auto n = static_cast<inte::var::_int*>(eval);
						if (n->value) { then->exec(fs); }
						else if (_else != nullptr) { _else->exec(fs); }
						return;
						}

					case inte::var::t_float:
						{
						auto n = static_cast<inte::var::_float*>(eval);
						if (n->value) { then->exec(fs); }
						else if (_else != nullptr) { _else->exec(fs); }
						return;
						}

					default: inte::error("If argument is not a number/boolean.", line);
					}
				}

			void _while::_print(std::ostream & stream) { stream << "while " << *cond << " then" << std::endl << *then << std::endl; }
			_while::_while(exp::root * cond, root * then, unsigned long long int line) : cond(cond), then(then), line(line) {}
			void _while::exec(inte::function_scope * fs)
				{
				while (true)
					{
					auto eval = cond->eval(fs);
					switch (eval->t)
						{
						case inte::var::t_int:
							{
							auto n = static_cast<inte::var::_int*>(eval);
							if (n->value) { then->exec(fs); }
							else { return; }
							break;
							}

						case inte::var::t_float:
							{
							auto n = static_cast<inte::var::_float*>(eval);
							if (n->value) { then->exec(fs); }
							else { return; }
							break;
							}

						default: inte::error("While argument is not a number/boolean.", line);
						}
					}
				}


			namespace dec
				{
				void func::_print(std::ostream & stream)
					{
					stream << name;
					if (args->size())
						{
						stream << " of ";
						for (auto a : *args)
							{
							stream << *a << " ";
							}
						}
					stream << "returns " << type << ", is:" << std::endl;
					stream << *stms;
					}
				func::func(tok::token type, tok::token name, std::list<func_arg_decl*>* args, block * stms)
					: name(name), args(args), stms(stms), type(tok_type_to_var_type(type)) {}
				func::func(block * stms) : stms(stms) {}
				void func::exec(inte::function_scope * fs)
					{
					inte::var::fun* f = new inte::var::fun();
					f->function = this;
					fs->alloc(name.s, f);
					}

				void var::_print(std::ostream & stream)
					{
					stream << type << name;
					if (init != nullptr) { stream << " = " << *init; }
					stream << std::endl;
					}
				var::var(tok::token type, tok::token name, exp::root * init) : name(name), init(init), type(tok_type_to_var_type(type)) {}
				void var::exec(inte::function_scope * fs)
					{
					auto eval = init->eval(fs);
					if (eval->t == type)
						{
						fs->alloc(name.s, eval->copy());
						return;
						}
					inte::error_var(eval, " is not of the correct type.", name.line);
					}
				}
			}
		}
	}

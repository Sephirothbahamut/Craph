#include "stdafx.h"
#include "tok.h"

namespace mylang
	{
	namespace tok
		{
		token::token(const std::string * s, tok::type t, const unsigned long int line) : s(s), t(t), line(line) {}
		token::token(double n, const unsigned long int line) : f(n), t(type::value_float), line(line) {}
		token::token(const long int n, const unsigned long int line) : i(n), t(type::value_int), line(line) {}
		token::token(const std::string* n, const unsigned long int line) : s(n), t(type::value_string), line(line) {}
		token::token() : s(nullptr), t(type::_EOF), line(0) {};

		std::ostream & operator<<(std::ostream & stream, const token & t)
			{
			switch (t.t)
				{
				case identifier: stream << "[id: \"" << *(t.s) << "\"]"; break;
				case value_int: stream << "[int: \"" << t.i << "\"]"; break;
				case value_float: stream << "[float: \"" << t.f << "\"]"; break;
				case value_string: stream << "[string: \"" << *(t.s) << "\"]"; break;

					//symbols
				case open_curly: stream << "{"; break;
				case clos_curly: stream << "}"; break;
				case open_round: stream << "("; break;
				case clos_round: stream << ")"; break;
				case open_squar: stream << "["; break;
				case clos_squar: stream << "]"; break;
				case semicolon: stream << ";"; break;
				case comma: stream << ","; break;

					//operations
				case gt: stream <<">"; break;
				case gte: stream <<">="; break;
				case lt: stream <<"<"; break;
				case lte: stream <<"<="; break;
				case eq: stream <<"=="; break;
				case neq: stream <<"!="; break;
				case shift_left: stream <<"<<"; break;
				case shift_right: stream <<">>"; break;
				case bit_or: stream <<"|"; break;
				case bit_and: stream <<"&"; break;
				case _excl_or: stream <<"^"; break;

				case assign: stream <<"="; break;
				case assign_sum: stream <<"+="; break;
				case assign_sub: stream <<"-="; break;
				case assign_mul: stream <<"*="; break;
				case assign_div: stream <<"/="; break;
				case assign_mod: stream <<"%="; break;
				case sum: stream <<"+"; break;
				case sub: stream <<"-"; break;
				case mul: stream <<"*"; break;
				case div: stream <<"/"; break;
				case mod: stream <<"%"; break;
				case inc: stream <<"++"; break;
				case dec: stream <<"--"; break;

					//keywords
				case _print: stream <<"print"; break;
				case _and: stream <<"and"; break;
				case _auto: stream <<"auto"; break;
				case _bool: stream <<"bool"; break;
				case _break: stream <<"break"; break;
				case _case: stream <<"case"; break;
				case _char: stream <<"char"; break;
				case _const: stream <<"const"; break;
				case _continue: stream <<"continue"; break;
				case _default: stream <<"default"; break;
				case _delete: stream <<"delete"; break;
				case _do: stream <<"do"; break;
				case _double: stream <<"double"; break;
				case _else: stream <<"else"; break;
				case _enum: stream <<"enum"; break;
				case _false: stream <<"false"; break;
				case _float: stream <<"float"; break;
				case _for: stream <<"for"; break;
				case _goto: stream <<"goto"; break;
				case _int: stream << "int "; break;
				case _if: stream << "if"; break;
				case _new: stream <<"new"; break;
				case _not: stream <<"not"; break;
				case _nullptr: stream <<"nullptr"; break;
				case _or: stream <<"or"; break;
				case _return: stream <<"stream <<"; break;
				case _switch: stream <<"switch"; break;
				case _true: stream <<"true"; break;
				case _void: stream <<"void"; break;
				case _while: stream <<"while"; break;
				case _EOF: stream <<"End of File"; break;

				default: stream << t.t; //"NEIN NEIN NEIN NEIN";
				}
			return stream;
			}

		tokenizer::tokenizer(std::stringstream& in, symbol_table& st) : in(&in), st(&st) {};
		tokenizer::~tokenizer() {}

		bool tokenizer::is_alpha(char c) { return((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_')); }
		bool tokenizer::is_num(char c) { return(c >= '0' && c <= '9'); }
		bool tokenizer::is_alphanum(char c) { return(is_alpha(c) || is_num(c)); }

		void tokenizer::get()
			{
			c = in->get();
			if (in->eof())
				{
				c = EOF;
				}
			index++;
			}

		token tokenizer::next()
			{
			while (true)
				{
				if (c == EOF)
					{
					return token(nullptr, type::_EOF, line);
					}
				else if (is_alpha(c))
					{
					return read_id();
					}
				else if (is_num(c))
					{
					return read_num();
					}
				else
					{
					switch (c)
						{
						//string
						case '\"':
							return read_string();

						//multiple
						case ':':
							get();
							if (c == ':') { get(); return token(nullptr, type::double_colon, line); }
							else return token(nullptr, type::colon, line);

							//exp
						case '<':
							get();
							if (c == '=') { get(); return token(nullptr, type::lte, line); }
							else if (c == '<') { get(); return token(nullptr, type::shift_left, line); }
							else return token(nullptr, type::lt, line);
						case '>':
							get();
							if (c == '=') { get(); return token(nullptr, type::gte, line); }
							else if (c == '>') { get(); return token(nullptr, type::shift_right, line); }
							else return token(nullptr, type::gt, line);
						case '=':
							get();
							if (c == '=') { get(); return token(nullptr, type::eq, line); }
							else return token(nullptr, type::assign, line);
						case '!':
							get();
							if (c == '=') { get(); return token(nullptr, type::neq, line); }
							else return token(nullptr, type::_not, line);
						case '|':
							get();
							if (c == '|') { get(); return token(nullptr, type::_or, line); }
							else if (c == '=') { get(); return token(nullptr, type::assign_bit_or, line); }
							else return token(nullptr, type::bit_or, line);
						case '^':
							get();
							if (c == '=') { get(); return token(nullptr, type::assign_excl_or, line); }
							else return token(nullptr, type::_excl_or, line);
						case '&':
							get();
							if (c == '&') { get(); return token(nullptr, type::_and, line); }
							else if (c == '=') { get(); return token(nullptr, type::assign_bit_and, line); }
							else return token(nullptr, type::bit_and, line);
						case '+':
							get();
							if (c == '+') { get(); return token(nullptr, type::inc, line); }
							else if (c == '=') { get(); return token(nullptr, type::assign_sum, line); }
							else return token(nullptr, type::sum, line);
						case '-':
							get();
							if (c == '-') { get(); return token(nullptr, type::dec, line); }
							else if (c == '=') { get(); return token(nullptr, type::assign_sub, line); }
							else return token(nullptr, type::sub, line);

						case '*':
							get();
							if (c == '=') { get(); return token(nullptr, type::assign_mul, line); }
							else return token(nullptr, type::mul, line);
						case '/':
							get();
							if (c == '=') { get(); return token(nullptr, type::assign_div, line); }
							else if (c == '/') { comment_skip_line(); }
							else if (c == '*') { comment_skip_section(); }
							else return token(nullptr, type::div, line);
						case '%':
							get();
							if (c == '=') { get(); return token(nullptr, type::assign_mod, line); }
							else return token(nullptr, type::mod, line);


							//simple
						case '(': get(); return token(nullptr, type::open_round, line);
						case ')': get(); return token(nullptr, type::clos_round, line);
						case '[': get(); return token(nullptr, type::open_squar, line);
						case ']': get(); return token(nullptr, type::clos_squar, line);
						case '{': get(); return token(nullptr, type::open_curly, line);
						case '}': get(); return token(nullptr, type::clos_curly, line);
						case ',': get(); return token(nullptr, type::comma, line);
						case ';': get(); return token(nullptr, type::semicolon, line);
						case '\n': line++;
						case ' ': case '\t': case '\r': default: break;
						}
					get();
					}
				}
			}

		token tokenizer::read_id()
			{
			std::string word = "";
			word += c;
			{
			if (c == 'a')
				{
				get();
				if (is_alphanum(c))
					{
					word += c;
					if (c == 'n')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 'd')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									goto RGW;
									}
								else return token(nullptr, tok::_and, line);
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else goto RGW;
					}
				else goto END_W;
				}
			else if (c == 'b')
				{
				get();
				if (is_alphanum(c))
					{
					word += c;
					if (c == 'o')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 'o')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									if (c == 'l')
										{
										get();
										if (is_alphanum(c))
											{
											word += c;
											goto RGW;
											}
										else return token(nullptr, tok::_bool, line);
										}
									else goto RGW;
									}
								else goto END_W;
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else if (c == 'r')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 'e')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									if (c == 'a')
										{
										get();
										if (is_alphanum(c))
											{
											word += c;
											if (c == 'k')
												{
												get();
												if (is_alphanum(c))
													{
													word += c;
													goto RGW;
													}
												else return token(nullptr, tok::_break, line);
												}
											else goto RGW;
											}
										else goto END_W;
										}
									else goto RGW;
									}
								else goto END_W;
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else goto RGW;
					}
				else goto END_W;
				}
			else if (c == 'c')
				{
				get();
				if (is_alphanum(c))
					{
					word += c;
					if (c == 'a')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 's')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									if (c == 'e')
										{
										get();
										if (is_alphanum(c))
											{
											word += c;
											goto RGW;
											}
										else return token(nullptr, tok::_case, line);
										}
									else goto RGW;
									}
								else goto END_W;
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else goto RGW;
					}
				else goto END_W;
				}
			else if (c == 'd')
				{
				get();
				if (is_alphanum(c))
					{
					word += c;
					if (c == 'e')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 'l')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									if (c == 'e')
										{
										get();
										if (is_alphanum(c))
											{
											word += c;
											if (c == 't')
												{
												get();
												if (is_alphanum(c))
													{
													word += c;
													if (c == 'e')
														{
														get();
														if (is_alphanum(c))
															{
															word += c;
															goto RGW;
															}
														else return token(nullptr, tok::_delete, line);
														}
													else goto RGW;
													}
												else goto END_W;
												}
											else goto RGW;
											}
										else goto END_W;
										}
									else goto RGW;
									}
								else goto END_W;
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else if (c == 'o')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							goto RGW;
							}
						else return token(nullptr, tok::_do, line);
						}
					else goto RGW;
					}
				else goto END_W;
				}
			else if (c == 'e')
				{
				get();
				if (is_alphanum(c))
					{
					word += c;
					if (c == 'l')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 's')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									if (c == 'e')
										{
										get();
										if (is_alphanum(c))
											{
											word += c;
											goto RGW;
											}
										else return token(nullptr, tok::_else, line);
										}
									else goto RGW;
									}
								else goto END_W;
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else if (c == 'n')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 'u')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									if (c == 'm')
										{
										get();
										if (is_alphanum(c))
											{
											word += c;
											goto RGW;
											}
										else return token(nullptr, tok::_enum, line);
										}
									else goto RGW;
									}
								else goto END_W;
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else goto RGW;
					}
				else goto END_W;
				}
			else if (c == 'f')
				{
				get();
				if (is_alphanum(c))
					{
					word += c;
					if (c == 'a')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 'l')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									if (c == 's')
										{
										get();
										if (is_alphanum(c))
											{
											word += c;
											if (c == 'e')
												{
												get();
												if (is_alphanum(c))
													{
													word += c;
													goto RGW;
													}
												else return token(nullptr, tok::_false, line);
												}
											else goto RGW;
											}
										else goto END_W;
										}
									else goto RGW;
									}
								else goto END_W;
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else if (c == 'l')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 'o')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									if (c == 'a')
										{
										get();
										if (is_alphanum(c))
											{
											word += c;
											if (c == 't')
												{
												get();
												if (is_alphanum(c))
													{
													word += c;
													goto RGW;
													}
												else return token(nullptr, tok::_float, line);
												}
											else goto RGW;
											}
										else goto END_W;
										}
									else goto RGW;
									}
								else goto END_W;
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else if (c == 'o')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 'r')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									goto RGW;
									}
								else return token(nullptr, tok::_for, line);
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else goto RGW;
					}
				else goto END_W;
				}
			else if (c == 'g')
				{
				get();
				if (is_alphanum(c))
					{
					word += c;
					if (c == 'o')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 't')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									if (c == 'o')
										{
										get();
										if (is_alphanum(c))
											{
											word += c;
											goto RGW;
											}
										else return token(nullptr, tok::_goto, line);
										}
									else goto RGW;
									}
								else goto END_W;
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else goto RGW;
					}
				else goto END_W;
				}
			else if (c == 'i')
				{
				get();
				if (is_alphanum(c))
					{
					word += c;
					if (c == 'f')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							goto RGW;
							}
						else return token(nullptr, tok::_if, line);
						}
					else if (c == 'n')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 't')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									goto RGW;
									}
								else return token(nullptr, tok::_int, line);
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else goto RGW;
					}
				else goto END_W;
				}
			else if (c == 'n')
				{
				get();
				if (is_alphanum(c))
					{
					word += c;
					if (c == 'e')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 'w')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									goto RGW;
									}
								else return token(nullptr, tok::_new, line);
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else if (c == 'o')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 't')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									goto RGW;
									}
								else return token(nullptr, tok::_not, line);
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else goto RGW;
					}
				else goto END_W;
				}
			else if (c == 'o')
				{
				get();
				if (is_alphanum(c))
					{
					word += c;
					if (c == 'r')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							goto RGW;
							}
						else return token(nullptr, tok::_or, line);
						}
					else goto RGW;
					}
				else goto END_W;
				}
			else if (c == 'p')
				{
				get();
				if (is_alphanum(c))
					{
					word += c;
					if (c == 'r')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 'i')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									if (c == 'n')
										{
										get();
										if (is_alphanum(c))
											{
											word += c;
											if (c == 't')
												{
												get();
												if (is_alphanum(c))
													{
													word += c;
													goto RGW;
													}
												else return token(nullptr, tok::_print, line);
												}
											else goto RGW;
											}
										else goto END_W;
										}
									else goto RGW;
									}
								else goto END_W;
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else goto RGW;
					}
				else goto END_W;
				}
			else if (c == 'r')
				{
				get();
				if (is_alphanum(c))
					{
					word += c;
					if (c == 'e')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 't')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									if (c == 'u')
										{
										get();
										if (is_alphanum(c))
											{
											word += c;
											if (c == 'r')
												{
												get();
												if (is_alphanum(c))
													{
													word += c;
													if (c == 'n')
														{
														get();
														if (is_alphanum(c))
															{
															word += c;
															goto RGW;
															}
														else return token(nullptr, tok::_return, line);
														}
													else goto RGW;
													}
												else goto END_W;
												}
											else goto RGW;
											}
										else goto END_W;
										}
									else goto RGW;
									}
								else goto END_W;
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else goto RGW;
					}
				else goto END_W;
				}
			else if (c == 's')
				{
				get();
				if (is_alphanum(c))
					{
					word += c;
					if (c == 't')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 'r')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									if (c == 'i')
										{
										get();
										if (is_alphanum(c))
											{
											word += c;
											if (c == 'n')
												{
												get();
												if (is_alphanum(c))
													{
													word += c;
													if (c == 'g')
														{
														get();
														if (is_alphanum(c))
															{
															word += c;
															goto RGW;
															}
														else return token(nullptr, tok::_string, line);
														}
													else goto RGW;
													}
												else goto END_W;
												}
											else goto RGW;
											}
										else goto END_W;
										}
									else goto RGW;
									}
								else goto END_W;
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else if (c == 'w')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 'i')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									if (c == 't')
										{
										get();
										if (is_alphanum(c))
											{
											word += c;
											if (c == 'c')
												{
												get();
												if (is_alphanum(c))
													{
													word += c;
													if (c == 'h')
														{
														get();
														if (is_alphanum(c))
															{
															word += c;
															goto RGW;
															}
														else return token(nullptr, tok::_switch, line);
														}
													else goto RGW;
													}
												else goto END_W;
												}
											else goto RGW;
											}
										else goto END_W;
										}
									else goto RGW;
									}
								else goto END_W;
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else goto RGW;
					}
				else goto END_W;
				}
			else if (c == 't')
				{
				get();
				if (is_alphanum(c))
					{
					word += c;
					if (c == 'r')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 'u')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									if (c == 'e')
										{
										get();
										if (is_alphanum(c))
											{
											word += c;
											goto RGW;
											}
										else return token(nullptr, tok::_true, line);
										}
									else goto RGW;
									}
								else goto END_W;
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else goto RGW;
					}
				else goto END_W;
				}
			else if (c == 'v')
				{
				get();
				if (is_alphanum(c))
					{
					word += c;
					if (c == 'a')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 'r')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									goto RGW;
									}
								else return token(nullptr, tok::_var, line);
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else if (c == 'o')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 'i')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									if (c == 'd')
										{
										get();
										if (is_alphanum(c))
											{
											word += c;
											goto RGW;
											}
										else return token(nullptr, tok::_void, line);
										}
									else goto RGW;
									}
								else goto END_W;
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else goto RGW;
					}
				else goto END_W;
				}
			else if (c == 'w')
				{
				get();
				if (is_alphanum(c))
					{
					word += c;
					if (c == 'h')
						{
						get();
						if (is_alphanum(c))
							{
							word += c;
							if (c == 'i')
								{
								get();
								if (is_alphanum(c))
									{
									word += c;
									if (c == 'l')
										{
										get();
										if (is_alphanum(c))
											{
											word += c;
											if (c == 'e')
												{
												get();
												if (is_alphanum(c))
													{
													word += c;
													goto RGW;
													}
												else return token(nullptr, tok::_while, line);
												}
											else goto RGW;
											}
										else goto END_W;
										}
									else goto RGW;
									}
								else goto END_W;
								}
							else goto RGW;
							}
						else goto END_W;
						}
					else goto RGW;
					}
				else goto END_W;
				}
			else goto RGW;
			}

		RGW:
			//read generic word
			while (true)
				{
				get();
				if (is_alphanum(c))
					{
					word += c;
					}
				else
					{
					break;
					}
				}
		END_W:
			//create identifier
			st->insert(word);
			return token(&(*(st->find(word))), type::identifier, line);
			}

		token tokenizer::read_num()
			{
			bool dot = false;
			std::string word = "";
			word += c;
			while (true)
				{
				get();
				if (is_num(c))
					{
					word += c;
					}
				else if (!dot && c == '.')
					{
					dot = true;
					word += c;
					}
				else
					{
					break;
					}
				}
			//create number
			std::stringstream ss;
			ss << word;
			if (dot)
				{
				double val;
				ss >> val;
				return token(val, line);
				}
			else
				{
				long int val;
				ss >> val;
				return token(val, line);
				}
			}

		token tokenizer::read_string()
			{
			std::string string = "";
			while (true)
				{
				get();
				switch (c)
					{
					case '\\':
						get();
						switch (c)
							{
							case 'a': string += '\a';
							case 'b': string += '\b';
							case 'e': string += '\e';
							case 'f': string += '\f';
							case 'n': string += '\n';
							case 'r': string += '\r';
							case 't': string += '\t';
							case 'v': string += '\v';
							case '\\': string += '\\';
							case '\'': string += '\'';
							case '\"': string += '\"';
							case '\?': string += '\?';
							}
						break;
						
					case '\"': get();
					case EOF: goto OUT;
					default: string += c;
					}
				}
		OUT:

			st->insert(string);
			return token(&(*(st->find(string))), line);
			}


		void tokenizer::comment_skip_line()
			{
			do { get(); } while (c != '\n');
			}

		void tokenizer::comment_skip_section()
			{
			while (true)
				{
				get();
				if (c == '*')
					{
					get();
					if (c == '/')
						{
						break;
						}
					}
				}
			get();
			}
		}
	}
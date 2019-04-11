#pragma once
#include <vector>
#include <map>
#include <string>
#include <stack>

namespace mylang
	{
	/*namespace act
		{
		class root 
			{
			public:
			};

		namespace var
			{
			enum type{ null, num, fun };

			class root : public act::root
				{
				private:
					type t = null;
				public: root(type t);
				};

			class number : public root
				{
				public:
					double n;
					number();
				};
			class function : public root
				{
				private:
					std::vector<stm::root*>* stms;
				public: function(std::vector<stm::root*>* stms);
				};
			}

		namespace exp
			{
			class root : public act::root {};
			}

		namespace stm
			{
			class root : public act::root {};

			namespace dec
				{
				class root : public stm::root
					{
					private:
						var::type type;
						std::string* id;
					public: root(var::type type, std::string* id);
					};

				class num : public root
					{
					private:
						exp::root* exp;
					public: num(std::string* id, exp::root* exp) : root(var::type::num, id), exp(exp) {}
					};
				class fun : public root
					{
					private:
						var::function* f;
					public: fun(std::string* id, var::function* f) : root(var::type::num, id), f(f) {}
					};
				}
			}

		class scope : public root
			{
			};
		class function_scope : public scope
			{};
		class global_scope : public scope
			{
			private:
				std::map<std::string*,act::var::root*> vars_table;
				std::vector<stm::dec::root*>* stms;
			public:
				global_scope(std::vector<stm::dec::root*>* statements);
			};
		}*/
	};


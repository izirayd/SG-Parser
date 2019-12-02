#pragma once

#include "SGParser/words_parser.hpp"
#include "SGParser/base_parser.hpp"
#include "SGParser/block_parser.hpp"
#include "SGParser/template_parser.hpp"
#include "SGParser/json_base.hpp"

namespace parser
{
	namespace executive
	{
		enum parser_flag_t : std::flag32_t
		{
			// Неизвестный опкод
			parser_unknow      = 1 << 0,
			// Следующий объект может быть любого типа
			parser_any_type    = 1 << 1,
			// Следующий объект это слово [value]: word
			parser_value       = 1 << 2,
			// Следующий объект это символо [symbol]: symbol
			parser_symbol      = 1 << 3,
			// Следующий объект это блоково-рекурсивная область от символа или слова open_block до close_block
			parser_open_block  = 1 << 4,
			// Следующий объект это блоково-рекурсивная область от символа или слова open_block до close_block
			parser_close_block = 1 << 5,
			// Следующий объектом будет конструкция уже определенного типа, выполняется подбор её правил
			parser_type        = 1 << 6,
			// Данный тип и его сигнатуры будут повторяться
			parser_repeat      = 1 << 7,
			// Данный тип и его сигнатуры будуи повторяться через указанный объект, слово или символ
			parser_repeat_next = 1 << 8,
			// Означает сигнатура нет, при этом цепочка сигнатур будет исполнена
			parser_empty       = 1 << 9
		};

		class type_element_t;

		class command_t
		{
		  public:
			  parser_flag_t   type;
			  std::string     value;

			  bool is_word() { return word; }
			  bool is_any()  { return any; }
			  bool any  = false;
			  bool word = false;

			  type_element_t* type_element;
		};

		class commands_t
		{
		   public:
			   bool is_status_parser = true;
			   std::vector<command_t> data;
		};

		class type_element_t
		{
		  public:

			  bool is_ex = false; // является просто объявленным типом, или учавствует в парсинге
			  std::string name;
			  std::vector<commands_t> all_commands;
		};

		class type_elements_t
		{
		  public:
			  std::vector<type_element_t> elements;
		};

		// because type_element_t *
		class type_elements_point_t
		{
		public:
			std::vector<type_element_t *> elements;
		};

		// Все типы, которые были созданы
		type_elements_t       global_types; 
		// Это типы, которые учавствуют в парсинге, сопаставляются от global_types, могут содержать в себе указание на global_types
		type_elements_point_t executive_types;


		void test_namespace_declaration2()
		{
			commands_t     commands;
			type_element_t type_element;


			type_element.is_ex = true;
			type_element.name = "class_declaration";

			{
				command_t command;

				command.type = parser_flag_t::parser_value;
				command.value = "class";
				command.any = false;

				commands.data.push_back(command);
			}

			{
				command_t command;

				command.type = parser_flag_t::parser_value;

				command.any  = true;
				command.word = true;

				commands.data.push_back(command);
			}

			{
				command_t command;

				command.type = parser_flag_t::parser_open_block;
				command.any = false;
				command.value = "{";

				commands.data.push_back(command);
			}

			//{
			//	command_t command;

			//	command.type = parser_flag_t::parser_any_type;
			//	command.any = true;

			//	commands.data.push_back(command);
			//}

			{
				command_t command;

				command.type = parser_flag_t::parser_close_block;
				command.any = false;
				command.value = "}";

				commands.data.push_back(command);
			}

			type_element.all_commands.push_back(commands);

			global_types.elements.push_back(type_element);
		}

		type_element_t* get_type(const std::string& name_type)
		{
			for (size_t i = 0; i < global_types.elements.size(); i++)
			{
				if (global_types.elements[i].name == name_type)
				{
					return &global_types.elements[i];
				}
			}

			return nullptr;
		}

		void final_init()
		{
			for (size_t i = 0; i < global_types.elements.size(); i++)
			{
				for (auto& commands : global_types.elements[i].all_commands)
				{
					for (auto& command : commands.data)
					{
						if (command.type == parser_flag_t::parser_type)
							command.type_element = get_type(command.value);
					}
				}
			}

			executive_types.elements.clear();

			for (size_t i = 0; i < global_types.elements.size(); i++)
			{
				if (global_types.elements[i].is_ex)
				  executive_types.elements.push_back(&global_types.elements[i]);
			}
		}


		void modificator_const() {

			commands_t     commands;
			type_element_t type_element;

			type_element.is_ex = false;
			type_element.name = "const";

			{
				command_t command;

				command.type  = parser_flag_t::parser_value;
				command.value = "const";
				command.any   = false;
				command.word  = true;

				commands.data.push_back(command);
			}

			type_element.all_commands.push_back(commands);
			global_types.elements.push_back(type_element);
		}

		void modificator_static() {

			commands_t     commands;
			type_element_t type_element;

			type_element.is_ex = false;
			type_element.name = "static";

			{
				command_t command;

				command.type  =  parser_flag_t::parser_value;
				command.value = "static";
				command.any   =  false;
				command.word  =  true;

				commands.data.push_back(command);
			}

			type_element.all_commands.push_back(commands);
			global_types.elements.push_back(type_element);
		}



		void modificators()
		{
			commands_t     commands;
			type_element_t type_element;

			type_element.is_ex = false;
			type_element.name = "modificator";

			{
				command_t command;

				command.type  = parser_flag_t::parser_type;
				command.value = "const";
				commands.data.push_back(command);
			}

			{
				command_t command;

				command.type  = parser_flag_t::parser_type;
				command.value = "static";

				commands.data.push_back(command);
			}

			type_element.all_commands.push_back(commands);
			global_types.elements.push_back(type_element);
		}

		void types()
		{
			commands_t     commands;
			type_element_t type_element;

			type_element.is_ex = false;
			type_element.name  = "type";

			{
				command_t command;

				command.type     = parser_flag_t::parser_type;
				command.value    = "modificator";

				commands.data.push_back(command);
			}

			{
				command_t command;

				command.type = parser_flag_t::parser_value;

				command.value = "any";

				command.any  = true;
				command.word = true;

				commands.data.push_back(command);
			}

			type_element.all_commands.push_back(commands);
			global_types.elements.push_back(type_element);
		}

		void init_vars()
		{
			commands_t     commands;
			type_element_t type_element;

			type_element.is_ex = true;
			type_element.name  = "var";

			{
				command_t command;

				command.type  = parser_flag_t::parser_type;
				command.value = "type";

				commands.data.push_back(command);
			}

			{
				command_t command;

				command.type = parser_flag_t::parser_value;

				command.value = "any";

				command.any = true;
				command.word = true;

				commands.data.push_back(command);
			}

			{
				command_t command;

				command.type = parser_flag_t::parser_value;

				command.value = ";";

				command.any  = false;
				command.word = false;

				commands.data.push_back(command);
			}

			type_element.all_commands.push_back(commands);
			global_types.elements.push_back(type_element);
		}

		//type namespace_declaration: [value: "namespace"] [value: "any"] [open_block: "{"] [any] [open_block: "{"];
		void test_namespace_declaration()
		{
			modificator_const();
			modificator_static();
			modificators();
			types();
			init_vars();


			test_namespace_declaration2();

			commands_t     commands;
			type_element_t type_element;
		
			type_element.is_ex = true;
			type_element.name = "namespace_declaration";

			{
				command_t command;

				command.type  =  parser_flag_t::parser_value;
				command.value = "namespace";
				command.any   =  false;

				commands.data.push_back(command);
			}

			{
				command_t command;

				command.type  =  parser_flag_t::parser_value;
				command.value = "any";

				command.any   =  true;
				command.word  =  true;

				commands.data.push_back(command);
			}

			{
				command_t command;
				
				command.type  =  parser_flag_t::parser_open_block;
				command.any   =  false;
				command.value = "{";

				commands.data.push_back(command);
			}

			//{
			//	command_t command;

			//	command.type = parser_flag_t::parser_any_type;
			//	command.any = true;
			//
			//	commands.data.push_back(command);
			//}

			{
				command_t command;

				command.type  =  parser_flag_t::parser_close_block;
				command.any   =  false;
				command.value = "}";

				commands.data.push_back(command);
			}

			type_element.all_commands.push_back(commands);

			global_types.elements.push_back(type_element);

			final_init();
		}


		class current_commands_t
		{
		public:
			std::vector<commands_t*> commands;

			void clear()
			{
				commands.clear();
			}
		};

		class current_type_t {

		public:
			current_commands_t current_commands;

			type_element_t* type_element = nullptr;

			void clear() {
				current_commands.clear();
				type_element = nullptr;
			}
		};

		class current_types_t {

		public:
			std::vector<current_type_t> current_type;

			void clear() {

				for (auto& it : current_type)
				{
					it.clear();
				}

			}
		};

		struct base_arg_t;
	
		class block_depth_base_t {
		public:
			std::flag32_t base_flag;
			std::flag32_t event_flag;

			type_elements_point_t* main_elements    = nullptr;;
			type_elements_point_t* current_elements = nullptr;; // Текущие правила цепочки сигнатур

			commands_t* current_commands     = nullptr;

			current_types_t current_types;

			int32_t signature_position = 0;

			void clear()
			{	
				std::clear_flag(base_flag);
				std::clear_flag(event_flag);
			}
		};

		class block_depth_t
		{
		public:
			std::vector<block_depth_base_t> block;

			block_depth_base_t* get_block(int32_t level) {

				if (level > 0)
					level--;

				// Нельзя убрать
				if (level < 0)
					level = 0;

				if (block.size() > level)
					return &block[level];

				int delta = level - block.size();

				block_depth_base_t null;

				if (delta == 0)
				{
					null.clear();

					block.push_back(null);
					return &block[level];
				}

				for (size_t i = 0; i < delta; i++)
					block.push_back(null);

				return &block[level];
			}
		};

		struct base_arg_t
		{
			std::size_t    size;

			block_depth_base_t* region;
			block_depth_base_t* parent_region;

			words_base_t* element;
			tree_words_t* word;

			bool is_global() {

				bool result = false;

				if (word)
					if (word->parent)
						result = word->parent->parent->is_root;

				return result;
			}
		};



		class base_parser_t
		{
		public:
			base_arg_t      base_arg;
			block_depth_t   block_depth;
	
			bool find_signature(base_arg_t* arg)
			{
				if (!arg)
					return false;

				arg->region->current_types.clear();

				type_elements_point_t* current_elements = nullptr;

				if (arg->region->current_elements)
					current_elements = arg->region->current_elements;
				else
					current_elements = arg->region->main_elements;

				if (!current_elements)
					return false;

				int i = 0;
				for (auto& types: current_elements->elements)
				{
					//printf("Check type: %s\n", types->name.c_str());
					
					current_type_t current_type;

					for (auto& commands : types->all_commands)
					{
						// commands это так же список
						if (!commands.data.empty()) {
							
							command_t* current_command = &commands.data[0];

							commands.is_status_parser = true;

							if (current_command->type == parser_flag_t::parser_value)
							{
								if (!current_command->is_any()) {
									if (current_command->value == arg->element->data)
										current_type.current_commands.commands.push_back(&commands);
								}
								else				
									current_type.current_commands.commands.push_back(&commands);			
							}

							if (current_command->type == parser_flag_t::parser_type)
							{

							}
						}
					}

					if (!current_type.current_commands.commands.empty())
					{
						current_type.type_element = current_elements->elements[i];
						arg->region->current_types.current_type.push_back(current_type);
					}

					i++;
				}

				return !arg->region->current_types.current_type.empty();
			}

			void parse_element_base_event(
				base_arg_t* arg
			)
			{
				if (!arg)
					return;

				if (arg->region->current_types.current_type.empty())
				{
					if (find_signature(arg))
					{
						arg->region->signature_position = 1;
					}
					else
					{
						if (arg->element->is_word() || arg->element->is_symbol())
						  printf("No data about it syntax: %s\n", arg->element->data.c_str());
					}
				}
				else
				{
					if (arg->element->is_word() || arg->element->is_symbol()) {

						for (auto &type: arg->region->current_types.current_type)
						{
							for (auto& commands : type.current_commands.commands)
							{
								if (commands)
								{
									if (commands->is_status_parser && commands->data.size() > arg->region->signature_position) {

										bool is_successfully_signature = false;
										command_t* command = &commands->data[arg->region->signature_position];

										if (command)
										{
											if (command->type == parser_flag_t::parser_value)
											{
												if (command->is_any())
												{
													if (command->is_word()) {
														printf("type: %s value: %s\n", type.type_element->name.c_str(), arg->element->data.c_str());
														is_successfully_signature = true;
													}
													else
													{
														printf("type: %s value: %s\n", type.type_element->name.c_str(), arg->element->data.c_str());
														is_successfully_signature = true;
													}
												}
												else
												{
													if (command->value == arg->element->data)
													{
														printf("%s\n", arg->element->data.c_str());
														is_successfully_signature = true;
													}
												}
											}

											if (command->type == parser_flag_t::parser_open_block || command->type == parser_flag_t::parser_close_block)
											{
												if (command->value == arg->element->data)
												{
													is_successfully_signature = true;
												}
											}

							/*				if (command->type == parser_flag_t::parser_any_type)
											{
												is_successfully_signature = true;
											}*/
										}

										if (!is_successfully_signature)
										{
											commands->is_status_parser = false;
											printf("Not signature %s, what is: \"%s\"?\n", type.type_element->name.c_str(), arg->element->data.c_str());
											arg->region->current_types.current_type.clear();
											break;
										}

										if ((commands->data.size() - 1) == arg->region->signature_position && commands->is_status_parser)
										{
											commands->is_status_parser = false;
											printf("It signature type %s!\n", type.type_element->name.c_str());
											arg->region->current_types.current_type.clear();
											break;
										}
									}
								}
							}
						}

						arg->region->signature_position++;
					}
				}
			}

			void parse_element(tree_words_t* word)
			{
				if (!word)
					return;

				block_depth_base_t* region        = block_depth.get_block(word->level);
				block_depth_base_t* parent_region = block_depth.get_block(word->level - 1);  // Выровнит до 1

				if (!region)
					return;

				region->main_elements = &executive_types;

				if (parent_region)
				  parent_region->main_elements = &executive_types;

				base_arg.region = region;
				base_arg.parent_region = parent_region;

				base_arg.element = word->get_value();
				base_arg.size    = word->size();

				base_arg.word    = word;

				parse_element_base_event(&base_arg);

			}
		};
	}
}

namespace parser
{
	namespace executive
	{
		class parser_t : public parser::executive::base_parser_t, public parser::block_parser_t, public parser::words_parser_t
		{
			public:

				std::string               code;
				words_t                   words;
				tree_words_t              tree;
				int32_t                   level = -1;

				void process_parse()
				{
					parser::words_parser_t::process_parse(code, words);
				}

				regulations::ex_parser_data_t *regulations_parser = nullptr;

				void parse_tree()
				{
					tree.process_function["base"] = detail::bind_function(&parser_t::parse_element, this, std::placeholders::_1);
					tree.start_process();
				}

				void test() <%  %>

				void init(regulations::ex_parser_data_t *r)
				{
					regulations_parser = r;
					init_event_process();
				}

				error_t init_event_process()
				{
					error_t error;

					if (!regulations_parser)
						return error.format("No regulations parser\n");

					return error;
				}
		};
    }
}
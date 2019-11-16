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
		using base_element_t  = cpp_element_t;
		using base_elements_t = cpp_elements_t;

		struct base_arg_t;

			struct element_t
			{
				element_t() 
				{

				}

				element_t(const std::string& new_name, const std::flag32_t& f) { base_flag = f; }

				std::string   name;
				std::flag32_t base_flag;
			};

			struct event_t {

				event_t() {}

				event_t(const std::string& new_name, std::flag32_t flag, std::function<void(base_arg_t*)> f, bool symbol, std::flag32_t ev) {
					element.name      = new_name;
					element.base_flag = flag;
					func              = f;
					is_symbol         = symbol;
					event_flag        = ev;
				}

				element_t     element;
				std::flag32_t event_flag;

				bool is_symbol = false;
				std::function<void(base_arg_t*&)> func = nullptr;
			};

			class event_process_t
			{
			public:
				std::vector<event_t> list_events;
				event_t any_value_read;
			};
		

		class block_depth_base_t {
		public:
			std::flag32_t base_flag;
			std::flag32_t event_flag;

			base_element_t* cpp_element    = nullptr;
			base_element_t* parent_element = nullptr;

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

			event_process_t* event_process;

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
			base_arg_t base_arg;

			block_depth_t   block_depth;
			event_process_t event_process;
	
			void parse_element_base_event(
				base_arg_t* arg
			)
			{
				if (!arg)
					return;

				arg->region->event_flag = arg->region->base_flag;

				if (arg->element->is_word())
				{
					bool is_element_find = false;
					for (size_t i = 0; i < arg->event_process->list_events.size(); i++)
					{
						if (arg->element->data == arg->event_process->list_events[i].element.name) {

							std::clear_flag(arg->region->base_flag);
							std::add_flag(arg->region->base_flag, arg->event_process->list_events[i].element.base_flag);
							is_element_find = true;

							break;
						}
					}

					if (!is_element_find)
					{
						std::add_flag(arg->region->base_flag, parser::regulations::value_flag);

						if (arg->event_process->any_value_read.func)
							arg->event_process->any_value_read.func(arg);
					}

					if (((arg->region->event_flag != arg->region->base_flag) && ((std::flag32_t) arg->region->event_flag > 0))) {

						for (size_t i = 0; i < arg->event_process->list_events.size(); i++)
						{
							if (std::check_flag(arg->region->event_flag, arg->event_process->list_events[i].element.base_flag) && std::check_flag(arg->region->base_flag, parser::regulations::value_flag)
								&& std::check_flag(arg->event_process->list_events[i].event_flag, event_flag_t::event_read_value)
							 )
							{
								// здесь вызов callback
								if (arg->event_process->list_events[i].func)
									arg->event_process->list_events[i].func(arg);
							}
						}
					}
				}

				if (arg->element->is_symbol())
				{
					for (size_t i = 0; i < arg->event_process->list_events.size(); i++)
					{
						if (arg->element->data == arg->event_process->list_events[i].element.name)
						{
							if (std::check_flag(arg->event_process->list_events[i].event_flag, event_flag_t::event_read_compared))
							{
								if (arg->event_process->list_events[i].func)
									arg->event_process->list_events[i].func(arg);
							}
							else if (std::check_flag(arg->event_process->list_events[i].event_flag, event_flag_t::event_read_value))
							{
								if (std::check_flag(arg->region->event_flag, arg->event_process->list_events[i].element.base_flag))
								{

									if (arg->event_process->list_events[i].func)
										arg->event_process->list_events[i].func(arg);
								}
							}
						}
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

				base_arg.region = region;
				base_arg.parent_region = parent_region;

				base_arg.element = word->get_value();
				base_arg.size    = word->size();

				base_arg.word    = word;

				base_arg.event_process = &event_process;

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

				void parse_keywrod(base_arg_t* arg) {
				
				}

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

					for (auto &element : regulations_parser->keywords)
					{
						if (element.is_value)
						    event_process.list_events.push_back({ element.keyword, element.unique_flag, std::bind(&parser_t::parse_keywrod, this, std::placeholders::_1), false, event_flag_t::event_read_value });
						else
						{
							event_process.list_events.push_back({ element.keyword, element.unique_flag, std::bind(&parser_t::parse_keywrod, this, std::placeholders::_1), false, event_flag_t::event_unknow });
						}					
					}

					return error;
				}
		};
    }
}
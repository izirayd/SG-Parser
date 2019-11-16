#include <iostream>

#include "SGParser/flag.hpp"
#include "SGParser/tree.hpp"
#include "SGParser/path.hpp"
#include "SGParser/file.hpp"

#include "SGParser/words_parser.hpp"
#include "SGParser/base_parser.hpp"
#include "SGParser/block_parser.hpp"

#include "SGParser/template_parser.hpp"

// надо распарсить это xtr1common

class parser_cpp_t : public parser::base_parser_t, public parser::block_parser_t, public parser::words_parser_t
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

	class value_list_t
	{
	public:
		std::vector<words_base_t*> list;
	};

	enum cpp_flag_value_t : std::flag32_t
	{
		cpp_var      = 1 << 0,
		cpp_new_type = 1 << 1,
	};

	void parse_tree()
	{
		tree.process_function["base"] = detail::bind_function(&parser_cpp_t::parse_element, this, std::placeholders::_1);
		tree.start_process();
	}

	cpp_element_t *last_cpp_element = nullptr;

	bool is_comment(std::flag32_t cpp_flag)
	{
		return (
			    std::check_flag(cpp_flag, cpp_flag_t::cpp_comment_line)    ||
			    std::check_flag(cpp_flag, cpp_flag_t::cpp_global_comments)
			   );
	}

	void parse_block(base_arg_t* arg)
	{
		if (std::check_flag(arg->region->cpp_flag, cpp_flag_t::cpp_block_read)) {

			if (arg->element->data == "{")
				if (arg->region->cpp_element)
				{
					if (arg->region->cpp_element->element)
						arg->region->cpp_element->element->start_index = arg->element->start_index;

					arg->region->cpp_element->open_block = arg->word;
				}

			if (arg->element->data == "}")
			{
				if (arg->region->cpp_element)
					if (arg->region->cpp_element->element)
						arg->region->cpp_element->element->end_index = arg->element->end_index;
				
				if (
					std::check_flag(arg->region->cpp_flag, cpp_flag_t::cpp_namespace)
					  ||
					std::check_flag(arg->region->cpp_flag, cpp_flag_t::cpp_class)
				   )
				{
					arg->region->cpp_element->close_block = arg->word;

					cpp_context.add_element(arg->region->cpp_element);
					std::clear_flag(arg->region->cpp_flag);
				}
			}
		}
	}

	void parse_namespace_keywrod(base_arg_t* arg) {
				
		if (!std::check_flag(arg->region->cpp_flag, cpp_flag_t::cpp_block_read) && !std::check_flag(arg->parent_region->cpp_flag, cpp_flag_t::cpp_template))
		{
			cpp_element_t* cpp_element = new cpp_element_t;

			if (std::check_flag(arg->region->cpp_flag, cpp_flag_t::cpp_namespace)) {
				
				cpp_element->namespace_value = new namespace_t;

				cpp_element->namespace_value->name  = arg->element->data.c_str();
				cpp_element->namespace_value->level = arg->word->level;
			}

			if (std::check_flag(arg->region->cpp_flag, cpp_flag_t::cpp_class)) {

				class_t* m_class = new class_t;

				m_class->name  = arg->element->data.c_str();
				m_class->level = arg->word->level;

				cpp_element->class_value = m_class;
			}

			cpp_element->element = arg->element;
			cpp_element->level   = arg->word->level;

			arg->region->cpp_element = cpp_element;
			last_cpp_element = cpp_element;

			std::del_flag(arg->region->cpp_flag, cpp_flag_t::cpp_value);
			std::add_flag(arg->region->cpp_flag, cpp_flag_t::cpp_block_read);
		}
	}

	// empty func, cuz no value
	void parse_template_keywrod(base_arg_t* arg)
	{
	}

	template_parser_t template_parser;

	void template_parser_process(base_arg_t* arg)
	{
		if (arg) {
			template_parser.start_process(arg);
		}
	}

	void parse_template_block(base_arg_t* arg)
	{
		if (std::check_flag(arg->region->cpp_flag, cpp_flag_t::cpp_template))
		{
			//printf("Template: %s\n", arg->element->data.c_str());
			if (arg->element->data == "<")
			{
				cpp_element_t* cpp_element  = new cpp_element_t;
				
				cpp_element->template_value = new template_t;

				cpp_element->template_value->name  = "template"; // вообще должно быть пусто
				cpp_element->template_value->level = arg->word->level;

				cpp_element->element = arg->element;
				cpp_element->level   = arg->word->level;

				arg->region->cpp_element = cpp_element;
				 
				last_cpp_element = cpp_element;
				
				if (arg->region->cpp_element)
				{
					if (arg->region->cpp_element->element)
						arg->region->cpp_element->element->start_index = arg->element->start_index;

					arg->region->cpp_element->open_block = arg->word;
				}
			}

			if (arg->element->data == ">")
			{
				if (arg->region->cpp_element)
					if (arg->region->cpp_element->element)
						arg->region->cpp_element->element->end_index = arg->element->end_index;

				arg->region->cpp_element->close_block = arg->word;

				cpp_context.add_element(arg->region->cpp_element);
				std::clear_flag(arg->region->cpp_flag);	

				template_parser_process(arg);
			}
		}		
	}

	template<typename type_flag_t>
	type_flag_t flag_sum(type_flag_t a, type_flag_t b)
	{
		std::flag32_t result = 0;

		std::add_flag(result, a);
		std::add_flag(result, b);

		return (type_flag_t) result;
	}

	void init_event_process()
	{
		event_process.list_events.push_back({ "namespace", cpp_flag_t::cpp_namespace, std::bind(&parser_cpp_t::parse_namespace_keywrod, this, std::placeholders::_1), false, event_flag_t::event_read_value });

		event_process.list_events.push_back({ "{",  flag_sum(cpp_flag_t::cpp_namespace, cpp_flag_t::cpp_block_read), std::bind(&parser_cpp_t::parse_block, this, std::placeholders::_1), true, event_flag_t::event_read_value });
		event_process.list_events.push_back({ "}",  flag_sum(cpp_flag_t::cpp_namespace, cpp_flag_t::cpp_block_read), std::bind(&parser_cpp_t::parse_block, this, std::placeholders::_1), true, event_flag_t::event_read_value });

		event_process.list_events.push_back({ "class", cpp_flag_t::cpp_class, std::bind(&parser_cpp_t::parse_namespace_keywrod, this, std::placeholders::_1), false, event_flag_t::event_read_value });

		event_process.list_events.push_back({ "{",  flag_sum(cpp_flag_t::cpp_class, cpp_flag_t::cpp_block_read), std::bind(&parser_cpp_t::parse_block, this, std::placeholders::_1), true , event_flag_t::event_read_value });
		event_process.list_events.push_back({ "}",  flag_sum(cpp_flag_t::cpp_class, cpp_flag_t::cpp_block_read), std::bind(&parser_cpp_t::parse_block, this, std::placeholders::_1), true, event_flag_t::event_read_value });

		event_process.list_events.push_back({ "template",  cpp_flag_t::cpp_template, std::bind(&parser_cpp_t::parse_template_keywrod, this, std::placeholders::_1), false , event_flag_t::event_read_value });

		event_process.list_events.push_back({ "<",  flag_sum(cpp_flag_t::cpp_template, cpp_flag_t::cpp_template_block_read), std::bind(&parser_cpp_t::parse_template_block, this, std::placeholders::_1), true , event_flag_t::event_read_value });
		event_process.list_events.push_back({ ">",  flag_sum(cpp_flag_t::cpp_template, cpp_flag_t::cpp_template_block_read), std::bind(&parser_cpp_t::parse_template_block, this, std::placeholders::_1), true, event_flag_t::event_read_value });
	}
};
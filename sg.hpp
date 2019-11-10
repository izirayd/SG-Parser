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

/*	void parse_element_base(
		std::flag32_t&  cpp_flag,
		std::flag32_t&  cpp_event,
		words_base_t*   element,
		std::size_t     size,
		tree_words_t*   word,
		cpp_element_t*& last_element_region,
		cpp_element_t*& parent_element_region
	)
	{
		uint64_t last_position = size, position = 0;
		bool is_last_word = false;

		if (std::check_flag(cpp_flag, cpp_flag_t::cpp_ignore_next_iteration))
		{
			std::del_flag(cpp_flag, cpp_flag_t::cpp_ignore_next_iteration);
			return;
		}

		if (std::check_flag(cpp_flag, cpp_flag_t::cpp_ignore_next_iteration2))
		{
			std::del_flag(cpp_flag, cpp_flag_t::cpp_ignore_next_iteration2);
			return;
		}

		if (last_position == position)
			is_last_word = true;

		cpp_event = cpp_flag;

			//process_comment_parse(&it, next_word, cpp_flag, comment_line, is_last_word);

			if (std::check_flag(cpp_flag, cpp_flag_t::cpp_ignore_next_iteration))
			{
				std::del_flag(cpp_flag, cpp_flag_t::cpp_ignore_next_iteration);
				return;
			}

			if (std::check_flag(cpp_flag, cpp_flag_t::cpp_ignore_next_iteration2))
			{
				std::del_flag(cpp_flag, cpp_flag_t::cpp_ignore_next_iteration2);
				return;
			}

			if (!is_comment(cpp_flag)) 
			{
				if (element->is_word())
				{
					// будем искать классы
					 
					     if (element->data == "template")  { std::clear_flag(cpp_flag); std::add_flag(cpp_flag, cpp_flag_t::cpp_template);  }
					else if (element->data == "class")     { std::clear_flag(cpp_flag); std::add_flag(cpp_flag, cpp_flag_t::cpp_class);     }
					else if (element->data == "struct")    { std::clear_flag(cpp_flag); std::add_flag(cpp_flag, cpp_flag_t::cpp_struct);    }
					else if (element->data == "union")     { std::clear_flag(cpp_flag); std::add_flag(cpp_flag, cpp_flag_t::cpp_union);     }
					else if (element->data == "namespace") { std::clear_flag(cpp_flag); std::add_flag(cpp_flag, cpp_flag_t::cpp_namespace); }
					else if (element->data == "typedef")   {
						std::clear_flag(cpp_flag); std::add_flag(cpp_flag, cpp_flag_t::cpp_typedef);
					}
					else
					{
						//std::clear_flag(cpp_flag);
						std::add_flag(cpp_flag, cpp_flag_t::cpp_value);
					}

					if (((cpp_event != cpp_flag) && ((std::flag32_t) cpp_event > 0))) {

						if (std::check_flag(cpp_event, cpp_flag_t::cpp_class) && std::check_flag(cpp_flag, cpp_flag_t::cpp_value))
						{
							bool is_global = false;

							if (word)
							{
								if (word->parent)
								{
									is_global = word->parent->parent->is_root;
								}
							}
							
							class_t *m_class = new class_t;

							m_class->name   = element->data.c_str();
							m_class->level  = word->level;

							cpp_element_t *cpp_element = new cpp_element_t;

							cpp_element->class_value = m_class;
							cpp_element->element     = element;
							cpp_element->level       = word->level;

							last_element_region = cpp_element;
							last_cpp_element    = cpp_element;

							std::del_flag(cpp_flag, cpp_flag_t::cpp_value);
							std::add_flag(cpp_flag, cpp_flag_t::cpp_block_read);
						}

						if (std::check_flag(cpp_event, cpp_flag_t::cpp_namespace) && std::check_flag(cpp_flag, cpp_flag_t::cpp_value))
						{
							bool is_global = false;

							if (word)
							{
								if (word->parent)
								{
									is_global = word->parent->parent->is_root;
								}
							}

							if (!last_element_region) {

								
							}		

							cpp_element_t* cpp_element   = new cpp_element_t;
							cpp_element->namespace_value = new namespace_t;

							cpp_element->namespace_value->name = element->data.c_str();
							cpp_element->namespace_value->level = word->level;

							cpp_element->element = element;
							cpp_element->level   = word->level;

							last_element_region = cpp_element;
							last_cpp_element    = cpp_element;

						    //printf("namespace: %s\n", element->data.c_str());

							std::del_flag(cpp_flag, cpp_flag_t::cpp_value);
							std::add_flag(cpp_flag, cpp_flag_t::cpp_block_read);
						}

						if (std::check_flag(cpp_event, cpp_flag_t::cpp_struct) && std::check_flag(cpp_flag, cpp_flag_t::cpp_value))
						{
							if (std::check_flag(cpp_event, cpp_flag_t::cpp_typedef))
							{
								printf("New type: %s\n", element->data.c_str());
							}
							else
							{
								printf("struct: %s\n", element->data.c_str());
							}

							std::clear_flag(cpp_flag);
						}
						if (std::check_flag(cpp_event, cpp_flag_t::cpp_union)  && std::check_flag(cpp_flag, cpp_flag_t::cpp_value))
						{
							std::clear_flag(cpp_flag);
							printf("union: %s\n", element->data.c_str());
						}


						if (std::check_flag(cpp_event, cpp_flag_t::cpp_template) && std::check_flag(cpp_flag, cpp_flag_t::cpp_value))
						{
							std::clear_flag(cpp_flag);
							printf("template: %s\n", element->data.c_str());
						}

					}

					if (((cpp_event == cpp_flag) && ((std::flag32_t) cpp_event > 0))) {
						//value_list.list.push_back(it.words->data);
					}

					if ((std::flag32_t) cpp_flag > 0)
					{
						if (
							(std::only_flag(cpp_flag, cpp_flag_t::cpp_value)) ||
							(std::only_flag(cpp_flag, cpp_flag_t::cpp_typedef)) ||
							(std::only_flag(cpp_flag, cpp_flag_t::cpp_typedef + cpp_flag_t::cpp_value))

							)
						{
							//value_list.list.push_back(element);
						}
					}
				}

				if (element->is_symbol() && std::check_flag(cpp_flag, cpp_flag_t::cpp_block_read) && element->data == ";")
				{
					if (std::check_flag(cpp_flag, cpp_flag_t::cpp_block_read))
					{
						if (last_element_region)
						{
							last_element_region->free();
							delete last_element_region;
						}

						std::clear_flag(cpp_flag);
						printf("Cannot parse element\n");
					}
				}

				if (element->is_symbol() && std::check_flag(cpp_flag, cpp_flag_t::cpp_block_read) && element->data == "{")
				{
					if (std::check_flag(cpp_flag, cpp_flag_t::cpp_block_read))
					{
						if (last_element_region)
						{
							if (last_element_region->element)
							    last_element_region->element->start_index = element->start_index;

							last_element_region->open_block = word;

						}
					}
				}

				if (element->is_symbol() && std::check_flag(cpp_flag, cpp_flag_t::cpp_block_read) && element->data == "}")
				{
					if (std::check_flag(cpp_flag, cpp_flag_t::cpp_block_read))
					{
						if (last_element_region)
						{
							if (last_element_region->element)
							    last_element_region->element->end_index = element->end_index;
						}

						if (std::check_flag(cpp_flag, cpp_flag_t::cpp_class))
						{
							last_element_region->close_block = word;

							cpp_context.add_element(last_element_region);
							std::clear_flag(cpp_flag);
						}

						if (std::check_flag(cpp_flag, cpp_flag_t::cpp_namespace))
						{
							last_element_region->close_block = word;

							//printf("cpp_namespace: %s {}:[%d][%d] open block: %d close block: %d\n", last_element_region->namespace_value->name.c_str(), last_element_region->element->start_index, last_element_region->element->end_index,
	      //                      last_element_region->open_block->index,
	      //                      last_element_region->close_block->index
	      //                    );

							cpp_context.add_element(last_element_region);
							std::clear_flag(cpp_flag);
						}
					}
				}

				if (element->is_symbol()) 
				{
					if (element->data != ";") 
					{
						//value_list.list.push_back(it.words);
					}
				}

				if (element->is_symbol() && element->data == ";")
				{
					//if (!value_list.list.empty())
					//{
					//	value_parse(&value_list);
					//	value_list.list.clear();

					//	std::clear_flag(cpp_flag);
					//}
				}
			}
	}*/

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
		event_process.list_events.push_back({ "namespace", cpp_flag_t::cpp_namespace, std::bind(&parser_cpp_t::parse_namespace_keywrod, this, std::placeholders::_1), false });

		event_process.list_events.push_back({ "{",  flag_sum(cpp_flag_t::cpp_namespace, cpp_flag_t::cpp_block_read), std::bind(&parser_cpp_t::parse_block, this, std::placeholders::_1), true });
		event_process.list_events.push_back({ "}",  flag_sum(cpp_flag_t::cpp_namespace, cpp_flag_t::cpp_block_read), std::bind(&parser_cpp_t::parse_block, this, std::placeholders::_1), true });

		event_process.list_events.push_back({ "class", cpp_flag_t::cpp_class, std::bind(&parser_cpp_t::parse_namespace_keywrod, this, std::placeholders::_1), false });

		event_process.list_events.push_back({ "{",  flag_sum(cpp_flag_t::cpp_class, cpp_flag_t::cpp_block_read), std::bind(&parser_cpp_t::parse_block, this, std::placeholders::_1), true });
		event_process.list_events.push_back({ "}",  flag_sum(cpp_flag_t::cpp_class, cpp_flag_t::cpp_block_read), std::bind(&parser_cpp_t::parse_block, this, std::placeholders::_1), true });

		event_process.list_events.push_back({ "template",  cpp_flag_t::cpp_template, std::bind(&parser_cpp_t::parse_template_keywrod, this, std::placeholders::_1), false });

		event_process.list_events.push_back({ "<",  flag_sum(cpp_flag_t::cpp_template, cpp_flag_t::cpp_template_block_read), std::bind(&parser_cpp_t::parse_template_block, this, std::placeholders::_1), true });
		event_process.list_events.push_back({ ">",  flag_sum(cpp_flag_t::cpp_template, cpp_flag_t::cpp_template_block_read), std::bind(&parser_cpp_t::parse_template_block, this, std::placeholders::_1), true });
	}
};
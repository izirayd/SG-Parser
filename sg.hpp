#include <iostream>

#include "SGParser/flag.hpp"
#include "SGParser/tree.hpp"
#include "SGParser/path.hpp"
#include "SGParser/file.hpp"

#include "SGParser/cpp_elements.hpp"
#include "SGParser/event_process.hpp"

// надо распарсить это xtr1common

class parser_cpp_t
{
public:
	
	std::string               code;
	words_t                   words;
	tree_words_t              tree;

	bool block_parse(
		tree_words_t* mother_tree,
		uint32_t position_start,
		uint32_t position_end,
		int32_t& change_position,
		const std::vector<std::pair<std::string, std::string>> &array_symbols,
		words_t* base_words
	)
	{
		if (!mother_tree || !base_words)
			return false;

		int32_t max_size_words  = position_end;// lol base_words->words.size();

		bool is_last_word       = false;
		words_base_t* next_word = nullptr;
		bool is_comment_line    = false;
		bool is_global_comments = false;
		bool is_ignore_next     = false;

		for (int32_t i = position_start; i < position_end; i++)
		{
			if (max_size_words == (i + 1))
				is_last_word = true;

			if (!is_last_word)
				next_word = &base_words->words[i + 1];
			else next_word = nullptr;

			if (max_size_words <= i)
				return true;

			if (base_words->words[i].data == "/" && next_word && !is_comment_line && !is_global_comments)
			{
				if (next_word->data == "/")
				{
					is_comment_line = true;
				}
			}

			if (base_words->words[i].data == "/" && next_word && !is_comment_line && !is_global_comments)
			{
				if (next_word->data == "*")
				{
					is_global_comments = true;
				}
			}

			if (!is_comment_line && !is_global_comments)
				if (!is_ignore_next)
					if (base_words->words[i].is_symbol() && !base_words->words[i].is_group_symbol())
					{

						for (size_t k = 0; k < array_symbols.size(); k++)
						{
							if (base_words->words[i].data == array_symbols[k].first)
							{
								tree_words_t* new_tree = new tree_words_t;

								mother_tree->sync(new_tree);

								int32_t new_position = -1;

								base_words->words[i].start_index = i;
								base_words->words[i].index_pair  = k;

								new_tree->set_value(&base_words->words[i]);

								if (block_parse(new_tree, i + 1, position_end, new_position, array_symbols, base_words))
								{
									if (new_position > -1) {
										i = new_position;
										k = 0;
									}

								}
								else break;//return false;
							}

							if (max_size_words <= i)
								return true;
					
							if (mother_tree->parent && mother_tree->get_value())
							{
								if ((mother_tree->get_value()->data == "<" && base_words->words[i].data == ";"))
								{
									delete mother_tree;

									change_position = i + 1;
									return false;
								}
							
									if ((base_words->words[i].data == array_symbols[k].second))
									{
									  if (array_symbols[mother_tree->get_value()->index_pair].second == base_words->words[i].data) {

											tree_words_t* new_tree = new tree_words_t;

											mother_tree->sync(new_tree);

											base_words->words[i].start_index = mother_tree->get_value()->start_index;
											base_words->words[i].end_index = i;

											new_tree->set_value(&base_words->words[i]);

											mother_tree->get_value()->end_index = i;

											new_tree->get_value()->my_pair = mother_tree;
											mother_tree->get_value()->my_pair = new_tree;

											mother_tree->parent->push(mother_tree);
											mother_tree->parent->push(new_tree);


											change_position = i + 1;
											return true;
										
									}

								}
							}

						}
					}

			if (is_ignore_next)
			{
				is_ignore_next = false;
			}

			if (base_words->words[i].is_new_line() && is_comment_line) {
				is_comment_line = false;
				//is_ignore_next  = true;
			}

			if (base_words->words[i].data == "*" && next_word && is_global_comments)
			{
				if (next_word->data == "/")
				{
					is_global_comments = false;
					is_ignore_next = true;
				}
			}

			mother_tree->push(&base_words->words[i]);
		}

		return true;

	}

	void process_parse()
	{
		std::flag16_t    parse_flag;
		std::flag16_t    parse_event_change_state;  // ¬ случае изменени€, получи флаг, который был изменен, что бы обработчик мог его обработать

		std::clear_flag(parse_flag);
		std::clear_flag(parse_event_change_state);

		words_base_t word;
		words_base_t word_for_one_symbol;

		for (const auto& symbol : code)
		{
			parse_event_change_state = parse_flag;

			if (it_space_or_tab(symbol)) {
				std::clear_flag(parse_flag);
				std::add_flag(parse_flag, parse_flag_t::space_tab);
			}
			else if (it_new_line(symbol)) {
				std::clear_flag(parse_flag);
				std::add_flag(parse_flag, parse_flag_t::new_line);
			}
			else if (it_word_symbol(symbol)) {
				std::clear_flag(parse_flag);
				std::add_flag(parse_flag, parse_flag_t::word);
			}
			else if (it_keyword_symbol(symbol)) {
				std::clear_flag(parse_flag);
				std::add_flag(parse_flag, parse_flag_t::symbol);
			}
			else
			{
				std::clear_flag(parse_flag);
				std::add_flag(parse_flag, parse_flag_t::unknow);
			}

			// ѕроизошло изменени€ буквы, теперь это что-то новое, но мы обработаем старое
			if (((parse_event_change_state != parse_flag) && ((std::flag16_t) parse_event_change_state > 0)) && !std::check_flag(parse_event_change_state, parse_flag_t::symbol)) {
				words.push(word);
				word.clear();
			}

			if ((std::flag16_t) parse_flag > 0) {

				if (std::check_flag(parse_flag, parse_flag_t::symbol))
				{
					word_for_one_symbol.type = parse_flag;
					word_for_one_symbol.data += symbol;

					words.push(word_for_one_symbol);

					word_for_one_symbol.clear();

					/*	std::clear_flag(word.type);
						std::add_flag(word.type, parse_flag_t::group_symbol);
						word.data += symbol;*/
				}
				else
				{
					word.type = parse_flag;
					word.data += symbol;
				}
			}

		}

		if (!word.data.empty())
		{
			words.push(word);
			word.clear();
		}
	}

	int32_t level = -1;

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
		//tree.func = std::bind(&parser_cpp_t::parse_element, this, std::placeholders::_1);
		//tree.process();

		tree.process_function["base"] = detail::bind_function(&parser_cpp_t::parse_element, this, std::placeholders::_1);
		tree.start_process();
	}


	cpp_element_t *last_cpp_element = nullptr;

	class block_depth_t
	{
	public:
		std::vector<block_depth_base_t> block;

		block_depth_base_t* get_block(int32_t level){
			
			if (level > 0)
			  level--;

			// Ќельз€ убрать
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

	block_depth_t block_depth;
	cpp_context_t cpp_context;

	void parse_element_base_event(
		base_arg_t* arg
	)
	{
		if (!arg)
			return;

		arg->region->cpp_event = arg->region->cpp_flag;

		if (arg->element->is_word())
		{
			// будем искать классы
			bool is_element_find = false;
			for (size_t i = 0; i < arg->event_process->list_events.size(); i++)
			{
				if (arg->element->data == arg->event_process->list_events[i].element.name) {
					
					std::clear_flag(arg->region->cpp_flag);
					std::add_flag(arg->region->cpp_flag, arg->event_process->list_events[i].element.cpp_flag);
					is_element_find = true;

					break;
				}
			}

			if (!is_element_find)
			{
				std::add_flag(arg->region->cpp_flag, cpp_flag_t::cpp_value);
			}

			if (((arg->region->cpp_event != arg->region->cpp_flag) && ((std::flag32_t) arg->region->cpp_event > 0))) {

				for (size_t i = 0; i < arg->event_process->list_events.size(); i++)
				{
					if (std::check_flag(arg->region->cpp_event, arg->event_process->list_events[i].element.cpp_flag)  && std::check_flag(arg->region->cpp_flag, cpp_flag_t::cpp_value))
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
				if (arg->element->data == arg->event_process->list_events[i].element.name && std::check_flag(arg->region->cpp_event, arg->event_process->list_events[i].element.cpp_flag)) {

					if (arg->event_process->list_events[i].func)
						arg->event_process->list_events[i].func(arg);
				}
			}
		}
	}

	void parse_element_base(
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
				{/*
					if (!value_list.list.empty())
					{
						value_parse(&value_list);
						value_list.list.clear();

						std::clear_flag(cpp_flag);
					}*/
				}
			}
	}

	bool is_comment(std::flag32_t cpp_flag)
	{
		return (
			    std::check_flag(cpp_flag, cpp_flag_t::cpp_comment_line)    ||
			    std::check_flag(cpp_flag, cpp_flag_t::cpp_global_comments)
			   );
	}

	base_arg_t base_arg;

	parser::event_process_t event_process;

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


	void print_space(int count, bool is_elements)
	{
		if (is_elements && count == 0)
		{
			printf("+");
			return;
		}

		int tmp_count = count;

		for (size_t i = 0; i < tmp_count; i++)
		{
			printf("|");
		}

		if (is_elements)
		{
			printf("+");
		}
		else
		{
			printf("-");
		}
	}

	void process_template_parser(tree_words_t* element, base_arg_t* arg, bool &ignore_first)
	{
		if (!element)
			return;

		if (element->get_value()->is_space_tab() || element->get_value()->is_new_line())
			return;

		if (ignore_first)
		{
			ignore_first = false;
			return;
		}

		print_space(element->level, element->is_have_sub_elemets());

		printf(" %s\n", element->get_value()->data.c_str());
	}

	void template_parser(base_arg_t* arg)
	{
		if (arg) {
			printf("+ Template elements\n");
			bool ignore_first = true;
			arg->region->cpp_element->open_block->process_function["base"] = detail::bind_function(&parser_cpp_t::process_template_parser, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			arg->region->cpp_element->open_block->start_process(arg, ignore_first);
			arg->region->cpp_element->open_block->process_root_enable();
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

				template_parser(arg);
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

	void parse_element(tree_words_t *word)
	{
		if (!word)
			return;

		//printf("parse element: %s\n", word->get_value()->data.c_str());

		block_depth_base_t* region        = block_depth.get_block(word->level);
		block_depth_base_t* parent_region = block_depth.get_block(word->level - 1);  // ¬ыровнит до 1

		if (!region)
			return;

		//parse_element_base(region->cpp_flag, region->cpp_event, word->get_value(), word->size(), word, region->cpp_element, parent_region->parent_element);

		base_arg.region        = region;
		base_arg.parent_region = parent_region;

		base_arg.element               = word->get_value();
		base_arg.size                  = word->size();

		base_arg.word                  = word;

		base_arg.event_process         = &event_process;

		parse_element_base_event(&base_arg);
	}
};
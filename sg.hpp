#include <iostream>

#include "SGParser/flag.hpp"
#include "SGParser/tree.hpp"
#include "SGParser/path.hpp"
#include "SGParser/file.hpp"

// надо распарсить это xtr1common

const std::string keyword_symbols_cpp = "{}()[]<>;:,./|\"\"\\\n=-+*?~`\'#!@$%^&\t\r";

bool it_keyword_symbol(const char symbol) {
	for (const auto& keyword_symbol : keyword_symbols_cpp)
		if (keyword_symbol == symbol)
			return true;

	return false;
}
bool it_keyword_symbol_or_space(const char symbol) { return it_keyword_symbol(symbol) || symbol == ' '; }
bool it_word_symbol(const char symbol) { return !it_keyword_symbol_or_space(symbol); }
bool it_space_or_tab(const char symbol) { return symbol == ' ' || symbol == '\t'; }
bool it_space_or_tab_or_end(const char symbol) { return symbol == ' ' || symbol == '\n' || symbol == '\t' || symbol == '\r'; }
bool it_new_line(const char symbol) { return symbol == '\n' || symbol == '\r'; }

enum class parse_flag_t : std::flag16_t
{
	unknow = 1 << 0,
	word = 1 << 1,
	space_tab = 1 << 2,
	symbol = 1 << 3,
	group_symbol = 1 << 4,
	new_line = 1 << 5
};

struct words_base_t
{
	words_base_t() { std::clear_flag(type); };
	words_base_t(std::flag16_t t, const std::string& str) { type = t; data = str; }

	std::flag16_t type;
	std::string   data;

	void clear() { std::clear_flag(type); data.clear(); }

	bool is_space_tab()    { return std::check_flag(type, parse_flag_t::space_tab);    }
	bool is_word()         { return std::check_flag(type, parse_flag_t::word);         }
	bool is_group_symbol() { return std::check_flag(type, parse_flag_t::group_symbol); }
	bool is_new_line()     { return std::check_flag(type, parse_flag_t::new_line);     }
	bool is_symbol()       { return std::check_flag(type, parse_flag_t::symbol);       }

	int start_index = -1;
	int end_index   = -1;

	int index_pair = -1;

	tree_t<words_base_t *> *my_pair = nullptr;
};

typedef tree_t<words_base_t*> tree_words_t;

class words_t
{
public:
	std::vector<words_base_t> words;
	void push(const words_base_t& data) { words.push_back(data); }
	void clear() { words.clear(); }
};

enum property_type_flag : std::flag32_t
{
	type_base_type = 1 << 0,
	type_pointer = 1 << 1,
	type_link = 1 << 2,
	type_const = 1 << 3,
	type_class = 1 << 4,
	type_namespace = 1 << 5,
	type_new_type = 1 << 6,
	type_void = 1 << 7,
	type_callback = 1 << 8,
	type_volatile = 1 << 9,
	type_register = 1 << 10,
	type_auto = 1 << 11,
	type_static = 1 << 12,
	type_extern = 1 << 13,
	type_restrict = 1 << 14,
	type_template = 1 << 15,
	type_array = 1 << 16
};

class class_t;

// ћожет €вл€тьс€ как собственным типом, так и классом так и шаблоном
// “ак же может быть переопределенн от другого класса или типа
class type_t
{
public:

	type_t() {}
	type_t(const std::string& n) { name = n; }
	type_t(const std::string& n, int32_t s) {
		full_name = n;
		name = n;
		size_type = s;
	}

	int32_t            size_type;

	std::string        name;
	std::string        namespace_name;
	std::string        sub_name;  // pointer, link
	std::string        full_name;

	std::flag32_t      property_type;

	type_t* base_type = nullptr;
	class_t* base_class = nullptr;

	bool is_namespace() { return std::check_flag(property_type, property_type_flag::type_namespace); }
	bool is_class() { return std::check_flag(property_type, property_type_flag::type_class); }
	bool is_base_type() { return std::check_flag(property_type, property_type_flag::type_base_type); }
	bool is_new_type() { return std::check_flag(property_type, property_type_flag::type_new_type); }
	bool is_void() { return std::check_flag(property_type, property_type_flag::type_void); }
	bool is_const() { return std::check_flag(property_type, property_type_flag::type_const); }
	bool is_pointer() { return std::check_flag(property_type, property_type_flag::type_pointer); }
	bool is_link() { return std::check_flag(property_type, property_type_flag::type_link); }
	bool is_callback() { return std::check_flag(property_type, property_type_flag::type_callback); }
	bool is_volatile() { return std::check_flag(property_type, property_type_flag::type_volatile); }

	bool is_restrict() { return std::check_flag(property_type, property_type_flag::type_restrict); }

	//  лассы хранени€
	bool is_auto() { return std::check_flag(property_type, property_type_flag::type_auto); }
	bool is_register() { return std::check_flag(property_type, property_type_flag::type_register); }
	bool is_static() { return std::check_flag(property_type, property_type_flag::type_static); }
	bool is_extern() { return std::check_flag(property_type, property_type_flag::type_extern); }

	bool is_template() { return std::check_flag(property_type, property_type_flag::type_template); }

	void set_namespace() { std::add_flag(property_type, property_type_flag::type_namespace); }
	void set_class() { std::add_flag(property_type, property_type_flag::type_class); }
	void set_base_type() { std::add_flag(property_type, property_type_flag::type_base_type); }
	void set_new_type() { std::add_flag(property_type, property_type_flag::type_new_type); }
	void set_void() { std::add_flag(property_type, property_type_flag::type_void); }
	void set_const() { std::add_flag(property_type, property_type_flag::type_const); }
	void set_pointer() { std::add_flag(property_type, property_type_flag::type_pointer); }
	void set_link() { std::add_flag(property_type, property_type_flag::type_link); }
	void set_callback() { std::add_flag(property_type, property_type_flag::type_callback); }
	void set_volatile() { std::add_flag(property_type, property_type_flag::type_volatile); }

	void set_restrict() { std::add_flag(property_type, property_type_flag::type_restrict); }

	//  лассы хранени€
	void set_auto() { std::add_flag(property_type, property_type_flag::type_auto); }
	void set_register() { std::add_flag(property_type, property_type_flag::type_register); }
	void set_static() { std::add_flag(property_type, property_type_flag::type_static); }
	void set_extern() { std::add_flag(property_type, property_type_flag::type_extern); }

	void set_template() { std::add_flag(property_type, property_type_flag::type_template); }
};

class var_t
{
public:
	type_t             type;
	std::string        name;
};

class argument_t : public var_t
{
public:

};

class template_type_t
{
public:
	std::string        name;
	std::flag32_t      property_type;
};

class method_t
{
public:
	std::string name;
	type_t      return_type;
	std::vector<argument_t>      arguments;
	std::vector<template_type_t> template_types;
	bool is_template = false;
	bool is_const = false;
};

class func_t
{
public:
	std::string name;

	type_t      return_type;
	std::vector<argument_t>      arguments;
	std::vector<template_type_t> template_types;
	bool is_template = false;
	bool is_const = false;
};

class operator_t
{
public:
};



class namespace_t
{
public:
	std::string name;
	std::string full_name = "::";

	int         level = 1; // global

	namespace_t*  parent       = nullptr;

	tree_words_t* open_block   = nullptr;
	tree_words_t* close_block  = nullptr;


	bool is_global() { return level == 1; }
};

class class_t
{
public:
	std::string name;

	std::vector<class_t>    child_class;
	std::vector<class_t*>   inheritance;
	std::vector<method_t>   methods;
	std::vector<operator_t> operators;
	std::vector<var_t>      vars;

	int level        = -1;

	bool is_template = false;
	bool is_const    = false;
};

class cpp_data_t
{
public:
	std::vector<class_t> classes;
	std::vector<type_t>  types;

	void init()
	{

	}

	void init_base_types()
	{
		types.push_back({ "int",                    sizeof(int) });
		types.push_back({ "float",                  sizeof(float) });
		types.push_back({ "double",                 sizeof(double) });
		types.push_back({ "long",                   sizeof(long) });

		types.push_back({ "bool",                   sizeof(bool) });
		types.push_back({ "char",                   sizeof(char) });
		types.push_back({ "wchar_t",                sizeof(wchar_t) });

		types.push_back({ "unsigned char",          sizeof(unsigned char) });

		types.push_back({ "signed char",            sizeof(signed char) });

		types.push_back({ "char16_t",               sizeof(char16_t) });
		types.push_back({ "char32_t",               sizeof(char32_t) });

		types.push_back({ "short",                  sizeof(short) });
		types.push_back({ "short int",              sizeof(short int) });
		types.push_back({ "signed short int",       sizeof(signed short int) });
		types.push_back({ "signed short",           sizeof(signed short) });
		types.push_back({ "unsigned short",         sizeof(unsigned short) });
		types.push_back({ "unsigned short int",     sizeof(unsigned short int) });


		types.push_back({ "signed int",             sizeof(signed int) });
		types.push_back({ "signed",                 sizeof(signed) });

		types.push_back({ "unsigned int",           sizeof(unsigned int) });
		types.push_back({ "unsigned",               sizeof(unsigned) });

		types.push_back({ "long int",               sizeof(long int) });
		types.push_back({ "signed long",            sizeof(signed long) });
		types.push_back({ "signed long int",        sizeof(signed long int) });

		types.push_back({ "unsigned long",          sizeof(unsigned long) });
		types.push_back({ "unsigned long int",      sizeof(unsigned long int) });

		types.push_back({ "long long",              sizeof(long long) });

		types.push_back({ "long long int",          sizeof(long long int) });
		types.push_back({ "signed long long int",   sizeof(signed long long int) });
		types.push_back({ "signed long long",       sizeof(signed long long) });

		types.push_back({ "unsigned long long",     sizeof(unsigned long long) });
		types.push_back({ "unsigned long long int", sizeof(unsigned long long int) });

		types.push_back({ "long double",            sizeof(long double) });

		types.push_back({ "void",                   sizeof(void*) });

		types.push_back({ "nullptr_t",              sizeof(std::nullptr_t) });

	}
};

class cpp_element_t
{
 public:
	/*
	   Ћюбой элемент изначально глобален
	   ≈го требуетс€ принудительно перемещать на другой уровень
	*/
    namespace_t   *namespace_value;

	class_t*      class_value = nullptr;
	words_base_t* element     = nullptr; // Ќельз€ уничтожать

	int           level       = -1;

	tree_words_t* open_block;
	tree_words_t* close_block;

	void free()
	{
		if (class_value) {
			delete class_value;
			class_value = nullptr;
		}

		if (namespace_value) {
			delete namespace_value;
			namespace_value = nullptr;
		}
	}

	// TODO: flags
	bool is_class()     { return class_value != nullptr; }
	bool is_namespace() { return namespace_value != nullptr; }
};

typedef tree_t<cpp_element_t*> tree_element_t;

class cpp_elements_t
{
public:
	tree_element_t data;
	std::vector<cpp_element_t*> no_sort_list;

	void process_sort(cpp_element_t* element)
	{
		// Just global object
		if (element->level == 1)
		{
			data.push(element);
			return;
		}

		result_tree = nullptr;

		tree_element_t* result = find_block(element);

		if (!result)
		{
			printf("WTF?!\n");
			return;
		}

		result->push(new tree_element_t(element));
	}

	void print()
	{
		printf("Result parse:\n");

		data.func = std::bind(&cpp_elements_t::process_print, this, std::placeholders::_1);
		data.process();
	}

	void print_space(int count)
	{
		if (count == 1)
		{
			printf("+");

		}
		else
			printf("|");

		for (size_t i = 1; i < count; i++)
		{
			printf("-");
		}
	}

	void process_print(tree_element_t *element)
	{
		print_space(element->level);
		
		if (element->get_value()->is_class())
		{
			printf("type: class name: %s\n", element->get_value()->class_value->name.c_str());
		}

		if (element->get_value()->is_namespace())
		{
			printf("type: namespace name: %s\n", element->get_value()->namespace_value->name.c_str());
		}
	}

	void process_sort()
	{
		// so sad, € не хотел делать перебор по ширине
		for (size_t i = 0; i < max_level; i++)
		{
			for (size_t k = 0; k < no_sort_list.size(); k++)
			{	
				if (no_sort_list[k]->level == (i + 1))
				  process_sort(no_sort_list[k]);
			}
		}	
	}

	int max_level = 1;

	void add_element(cpp_element_t* element)
	{
		if (element->level < 1)
		{
			// lol
			printf("Error level element!\n");
			return;
		}

		if (element->level > max_level)
			max_level = element->level;

		no_sort_list.push_back(element);
	}


	tree_element_t* find_block(cpp_element_t* element) {
		element_for_find = element;
		data.func = std::bind(&cpp_elements_t::process, this, std::placeholders::_1);
		data.process();
		return result_tree;
	}

	tree_element_t* result_tree      = nullptr;
	cpp_element_t*  element_for_find = nullptr;

	void process(tree_element_t* tree_element)
	{
		if (!element_for_find) {
			tree_element->stop_process();
			return;
		}

		if (!element_for_find->open_block) {
			tree_element->stop_process();
			return;
		}

		if (!element_for_find->open_block->parent) {
			tree_element->stop_process();
			return;
		}

		if (tree_element->get_value()->open_block->index == element_for_find->open_block->parent->index)
		{
			result_tree = tree_element;
			tree_element->stop_process();
			return;
		}
	}
};

class cpp_context_t
{
 public:
	 cpp_elements_t elements; // final cpp tree meh

	 void add_element(cpp_element_t* element)
	 {
		 elements.add_element(element);
	 }

};

enum cpp_flag_t : std::flag32_t
{
	unknow = 1 << 0,
	value = 1 << 1,
	cpp_class = 1 << 2,
	cpp_struct = 1 << 3,
	cpp_union = 1 << 4,
	cpp_namespace = 1 << 5,
	cpp_comment_line = 1 << 6,
	cpp_comment_line_end = 1 << 7,
	cpp_global_comments = 1 << 8,
	cpp_global_comments_end = 1 << 9,
	cpp_ignore_next_iteration = 1 << 10,
	cpp_ignore_next_iteration2 = 1 << 11,
	cpp_typedef = 1 << 12,
	cpp_block_read = 1 << 13,
	cpp_template = 1 << 14
};

struct base_arg_t;

namespace parser
{
	struct element_t
	{
		element_t() {

		}
		element_t(const std::string &new_name, const cpp_flag_t &f) { cpp_flag = f; }
			std::string   name;
			cpp_flag_t    cpp_flag;
	};

	struct event_t {

		    event_t() {}

			event_t(const std::string &new_name, cpp_flag_t flag, std::function<void(base_arg_t*)> f, bool symbol) {
				element.name     = new_name;
				element.cpp_flag = flag;
				func             = f;
				is_symbol        = symbol;
			}

		    element_t element;
			bool is_symbol = false;
			std::function<void(base_arg_t* &)> func = nullptr;
	};

	class event_process_t
	{
	    public:
			std::vector<event_t> list_events;
	};
}

class block_depth_base_t {
public:
	std::flag32_t cpp_flag;
	std::flag32_t cpp_event;

	std::string comment_line;
	std::string comment_big;

	cpp_element_t* cpp_element = nullptr;
	cpp_element_t* parent_element = nullptr;

	void clear()
	{
		std::clear_flag(cpp_flag);
		std::clear_flag(cpp_event);
	}
};


struct base_arg_t
{
	std::size_t    size;

	block_depth_base_t* region;
	block_depth_base_t* parent_region;

	words_base_t* element;
	tree_words_t* word;

	parser::event_process_t* event_process;
};

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
		tree.func = std::bind(&parser_cpp_t::parse_element, this, std::placeholders::_1);
		tree.process();
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
				std::add_flag(arg->region->cpp_flag, cpp_flag_t::value);
			}

			if (((arg->region->cpp_event != arg->region->cpp_flag) && ((std::flag32_t) arg->region->cpp_event > 0))) {

				for (size_t i = 0; i < arg->event_process->list_events.size(); i++)
				{
					if (std::check_flag(arg->region->cpp_event, arg->event_process->list_events[i].element.cpp_flag) && std::check_flag(arg->region->cpp_flag, cpp_flag_t::value))
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
						std::add_flag(cpp_flag, cpp_flag_t::value);
					}

					if (((cpp_event != cpp_flag) && ((std::flag32_t) cpp_event > 0))) {

						if (std::check_flag(cpp_event, cpp_flag_t::cpp_class) && std::check_flag(cpp_flag, cpp_flag_t::value))
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

							std::del_flag(cpp_flag, cpp_flag_t::value);
							std::add_flag(cpp_flag, cpp_flag_t::cpp_block_read);
						}

						if (std::check_flag(cpp_event, cpp_flag_t::cpp_namespace) && std::check_flag(cpp_flag, cpp_flag_t::value))
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

							std::del_flag(cpp_flag, cpp_flag_t::value);
							std::add_flag(cpp_flag, cpp_flag_t::cpp_block_read);
						}

						if (std::check_flag(cpp_event, cpp_flag_t::cpp_struct) && std::check_flag(cpp_flag, cpp_flag_t::value))
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
						if (std::check_flag(cpp_event, cpp_flag_t::cpp_union)  && std::check_flag(cpp_flag, cpp_flag_t::value))
						{
							std::clear_flag(cpp_flag);
							printf("union: %s\n", element->data.c_str());
						}


						if (std::check_flag(cpp_event, cpp_flag_t::cpp_template) && std::check_flag(cpp_flag, cpp_flag_t::value))
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
							(std::only_flag(cpp_flag, cpp_flag_t::value)) ||
							(std::only_flag(cpp_flag, cpp_flag_t::cpp_typedef)) ||
							(std::only_flag(cpp_flag, cpp_flag_t::cpp_typedef + cpp_flag_t::value))

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

	void parse_namespace(base_arg_t*arg)
	{
		bool is_global = false;

		if (arg->word)
		{
			if (arg->word->parent)
			{
				is_global = arg->word->parent->parent->is_root;
			}
		}

		if (!std::check_flag(arg->region->cpp_flag, cpp_flag_t::cpp_block_read)) {

			cpp_element_t* cpp_element   = new cpp_element_t;

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

			std::del_flag(arg->region->cpp_flag, cpp_flag_t::value);
			std::add_flag(arg->region->cpp_flag, cpp_flag_t::cpp_block_read);
		}
		else
		{
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
				{
					if (arg->region->cpp_element->element)
						arg->region->cpp_element->element->end_index = arg->element->end_index;
				}

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
		event_process.list_events.push_back({ "namespace", cpp_flag_t::cpp_namespace, std::bind(&parser_cpp_t::parse_namespace, this, std::placeholders::_1), false });

		event_process.list_events.push_back({ "{",  flag_sum(cpp_flag_t::cpp_namespace, cpp_flag_t::cpp_block_read), std::bind(&parser_cpp_t::parse_namespace, this, std::placeholders::_1), true });
		event_process.list_events.push_back({ "}",  flag_sum(cpp_flag_t::cpp_namespace, cpp_flag_t::cpp_block_read), std::bind(&parser_cpp_t::parse_namespace, this, std::placeholders::_1), true });

		event_process.list_events.push_back({ "class", cpp_flag_t::cpp_class, std::bind(&parser_cpp_t::parse_namespace, this, std::placeholders::_1), false });

		event_process.list_events.push_back({ "{",  flag_sum(cpp_flag_t::cpp_class, cpp_flag_t::cpp_block_read), std::bind(&parser_cpp_t::parse_namespace, this, std::placeholders::_1), true });
		event_process.list_events.push_back({ "}",  flag_sum(cpp_flag_t::cpp_class, cpp_flag_t::cpp_block_read), std::bind(&parser_cpp_t::parse_namespace, this, std::placeholders::_1), true });

		event_process.list_events.push_back({ "template", cpp_flag_t::cpp_class, std::bind(&parser_cpp_t::parse_namespace, this, std::placeholders::_1), false });

		event_process.list_events.push_back({ "<",  flag_sum(cpp_flag_t::cpp_class, cpp_flag_t::cpp_block_read), std::bind(&parser_cpp_t::parse_namespace, this, std::placeholders::_1), true });
		event_process.list_events.push_back({ ">",  flag_sum(cpp_flag_t::cpp_class, cpp_flag_t::cpp_block_read), std::bind(&parser_cpp_t::parse_namespace, this, std::placeholders::_1), true });
	}

	void parse_element(tree_words_t *word)
	{
		if (!word)
			return;

		block_depth_base_t* region        = block_depth.get_block(word->level);
		block_depth_base_t* parent_region = block_depth.get_block(word->level - 1);  // ¬ыровнит до 1

		if (!region)
			return;

		//parse_element_base(region->cpp_flag, region->cpp_event, word->get_value(), word->size(), word, region->cpp_element, parent_region->parent_element);

		base_arg.region = region;
		base_arg.parent_region = parent_region;

		base_arg.element               = word->get_value();
		base_arg.size                  = word->size();

		base_arg.word                  = word;

		base_arg.event_process         = &event_process;


		parse_element_base_event(&base_arg);
	}
};
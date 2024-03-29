#pragma once

#include "flags_enums.hpp"
#include <string>
#include <vector>
#include <functional>

#include "cpp_element.hpp"

struct base_arg_t;

namespace parser
{
	struct element_t
	{
		element_t() {

		}
		element_t(const std::string& new_name, const std::flag32_t& f) { cpp_flag = f; }

		std::string   name;
		std::flag32_t cpp_flag;
	};

	struct event_t {

		event_t() {}

		event_t(const std::string& new_name, std::flag32_t flag, std::function<void(base_arg_t*)> f, bool symbol, std::flag32_t ev) {
			element.name = new_name;
			element.cpp_flag = flag;
			func = f;
			is_symbol = symbol;
			event_flag = ev;
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
}

class block_depth_base_t {
public:
	std::flag32_t cpp_flag;
	std::flag32_t cpp_event;

	cpp_element_t* cpp_element    = nullptr;
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

	bool is_global() {

		bool result = false;

		if (word)
			if (word->parent)
				result = word->parent->parent->is_root;

		return result;
	}
};


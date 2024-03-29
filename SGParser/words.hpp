#pragma once

#include "flags_enums.hpp"

const std::string keyword_symbols_cpp = "{}()[]<>;:,./|\"\"\\\n=-+*?~`\'#!@$%^&\t\r";

bool it_keyword_symbol(const char symbol) {
	for (const auto& keyword_symbol : keyword_symbols_cpp)
		if (keyword_symbol == symbol)
			return true;

	return false;
}

inline bool it_keyword_symbol_or_space(const char symbol) { return it_keyword_symbol(symbol) || symbol == ' '; }
inline bool it_word_symbol(const char symbol)             { return !it_keyword_symbol_or_space(symbol); }
inline bool it_space_or_tab(const char symbol)            { return symbol == ' ' || symbol == '\t'; }
inline bool it_space_or_tab_or_end(const char symbol)     { return symbol == ' ' || symbol == '\n' || symbol == '\t' || symbol == '\r'; }
inline bool it_new_line(const char symbol)                { return symbol == '\n' || symbol == '\r'; }

struct words_base_t
{
	words_base_t() { std::clear_flag(type); };
	words_base_t(std::flag16_t t, const std::string& str) { type = t; data = str; }

	std::flag16_t type;
	std::string   data;

	inline void clear() { 
		std::clear_flag(type); data.clear(); }

	inline bool is_space_tab()    { return std::check_flag(type, parse_flag_t::space_tab);    }
	inline bool is_word()         { return std::check_flag(type, parse_flag_t::word);         }
	inline bool is_group_symbol() { return std::check_flag(type, parse_flag_t::group_symbol); }
	inline bool is_new_line()     { return std::check_flag(type, parse_flag_t::new_line);     }
	inline bool is_symbol()       { return std::check_flag(type, parse_flag_t::symbol);       }

	int start_index = -1;
	int end_index   = -1;

	int index_pair  = -1;

	tree_t<words_base_t*>* my_pair = nullptr;
};

typedef tree_t<words_base_t*> tree_words_t;

class words_t
{
public:
	std::vector<words_base_t> words;
	inline void push(const words_base_t& data) { words.push_back(data); }
	inline void clear() { words.clear(); }
};
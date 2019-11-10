#pragma once

#include "words.hpp"

namespace parser {
	class words_parser_t
	{
	public:
		void process_parse(const std::string &code, words_t &words)
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

	};
}
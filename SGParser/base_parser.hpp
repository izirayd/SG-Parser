#pragma once

#include "cpp_elements.hpp"
#include "event_process.hpp"

namespace parser
{
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

	class base_parser_t
	{
	public:
		base_arg_t base_arg;

		parser::block_depth_t   block_depth;
		parser::event_process_t event_process;
		cpp_context_t           cpp_context;

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

					if (arg->event_process->any_value_read.func)
						arg->event_process->any_value_read.func(arg);
				}

				if (((arg->region->cpp_event != arg->region->cpp_flag) && ((std::flag32_t) arg->region->cpp_event > 0))) {

					for (size_t i = 0; i < arg->event_process->list_events.size(); i++)
					{
						if (std::check_flag(arg->region->cpp_event, arg->event_process->list_events[i].element.cpp_flag) && std::check_flag(arg->region->cpp_flag, cpp_flag_t::cpp_value))
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
							if (std::check_flag(arg->region->cpp_event, arg->event_process->list_events[i].element.cpp_flag))
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

			//printf("parse element: %s\n", word->get_value()->data.c_str());

			block_depth_base_t* region        = block_depth.get_block(word->level);
			block_depth_base_t* parent_region = block_depth.get_block(word->level - 1);  // Выровнит до 1

			if (!region)
				return;

			base_arg.region        = region;
			base_arg.parent_region = parent_region;

			base_arg.element = word->get_value();
			base_arg.size    = word->size();

			base_arg.word = word;

			base_arg.event_process = &event_process;

			parse_element_base_event(&base_arg);
		}
	};
}

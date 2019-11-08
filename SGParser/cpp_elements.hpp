#pragma once

#include "cpp_element.hpp"

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

		//data.func = std::bind(&cpp_elements_t::process_print, this, std::placeholders::_1);
		//data.process();

		data.process_function["base"] = detail::bind_function(&cpp_elements_t::process_print, this, std::placeholders::_1);
		data.start_process();
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

	void print_element(tree_words_t* tree)
	{
		if (tree->get_value()->is_new_line()
			||
			tree->get_value()->is_space_tab() ||
			tree->get_value()->is_symbol()
			)
			return;

		print_space(tree->level + 1);
		printf("%s\n", tree->get_value()->data.c_str());
	}

	void process_print(tree_element_t* element)
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

		if (element->get_value()->is_template())
		{
			printf("type: template:\n");

			//element->get_value()->open_block->func = std::bind(&cpp_elements_t::print_element, this, std::placeholders::_1);
			//element->get_value()->open_block->process();

			//element->get_value()->open_block->func = std::bind(&cpp_elements_t::print_element, this, std::placeholders::_1);
			//element->get_value()->open_block->process();


			element->get_value()->open_block->process_function["base"] = detail::bind_function(&cpp_elements_t::print_element, this, std::placeholders::_1);
			element->get_value()->open_block->start_process();
		}
	}

	void process_sort()
	{
		// so sad, я не хотел делать перебор по ширине
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
		//data.func = std::bind(&cpp_elements_t::process, this, std::placeholders::_1);
		//data.process();

		data.process_function["base"] = detail::bind_function(&cpp_elements_t::process, this, std::placeholders::_1);
		data.start_process();

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
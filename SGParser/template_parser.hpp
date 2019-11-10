#pragma once

#include "cpp_elements.hpp"
#include "event_process.hpp"

class template_parser_t
{
   public:
	   void start_process(base_arg_t* arg) {

		   printf("\n");
		   printf("**************************************\n");
		   printf("*   TEMPLATE ANALISIS                *\n");
		   printf("**************************************\n");
		   printf("\n");

		   printf("+ Template elements\n");

		   bool ignore_first = true;
		   arg->region->cpp_element->open_block->process_function["base"] = detail::bind_function(&template_parser_t::process_template_parser, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		   arg->region->cpp_element->open_block->start_process(arg, ignore_first);
		   arg->region->cpp_element->open_block->process_root_enable();
	   }

	   enum class parse_flag_t : std::flag16_t
	   {
		   template_unknow       = 1 << 0,
		   template_typename     = 1 << 1,
		   template_class        = 1 << 2,
		   template_value        = 1 << 3,
		   template_type         = 1 << 4
	   };

	   void process_template_parser(tree_words_t* element, base_arg_t* arg, bool& ignore_first)
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

		   if (element->get_value()->data == "class")   {}
		   if (element->get_value()->data == "typename") {}

	   }
};
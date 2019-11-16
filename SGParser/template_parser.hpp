#pragma once

#include "cpp_elements.hpp"
#include "event_process.hpp"
#include "benchmark.hpp"

class template_parser_t : public parser::base_parser_t
{
   public:

	   benchmark_t benchmark;

	   void start_process(base_arg_t* arg) {

		
		   printf("\n");
		   printf("**************************************\n");
		   printf("*   TEMPLATE ANALISIS                *\n");
		   printf("**************************************\n");
		   printf("\n");

		 //  printf("+ Template elements\n");

		   benchmark.start();

		   init_event_process();

		   bool ignore_first = true;
		   arg->region->cpp_element->open_block->process_function["base"] = detail::bind_function(&template_parser_t::process_template_parser, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		   arg->region->cpp_element->open_block->start_process(arg, ignore_first);
		   arg->region->cpp_element->open_block->process_root_enable();

		   benchmark.end();

		   benchmark.print("\nTime analisis templates: ");
	   }

	   enum class template_parse_flag_t : std::flag32_t
	   {
		   template_unknow       = 1 << 0,
		   template_value        = 1 << 1,
		   template_typename     = 1 << 2,
		   template_class        = 1 << 3,
		   template_next         = 1 << 4,
		   template_read         = 1 << 5
	   };

	   base_arg_t* main_arg;

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

		  // print_space(element->level, element->is_have_sub_elemets());

		  // printf(" %s\n", element->get_value()->data.c_str());

		   //if (element->get_value()->data == "class")    {}
		   //if (element->get_value()->data == "typename") {}

		   main_arg = arg;
		   parse_element(element);
	   }

	   void parse_typename_class_keywrod(base_arg_t* arg) {

		   if (std::check_flag(arg->region->cpp_flag, template_parse_flag_t::template_typename))
		   {
			   printf("template typename: %s\n", arg->element->data.c_str());
		   }

		   if (std::check_flag(arg->region->cpp_flag, template_parse_flag_t::template_class))
		   {
			   printf("template class: %s\n", arg->element->data.c_str());
		   }

		   std::del_flag(arg->region->cpp_flag, cpp_flag_t::cpp_value);
	   }

	   void parse_next_keywrod(base_arg_t* arg)     {
		   std::clear_flag(arg->region->cpp_flag);
	   }
	   void parse_compared_keywrod(base_arg_t* arg) {
		   std::clear_flag(arg->region->cpp_flag);
	   }

	   void parse_read_type_keywrod(base_arg_t* arg) {
	   
	   }
	   void parse_any_value_keywrod(base_arg_t* arg) {

	   }

	   void init_event_process()
	   {
		   event_process.any_value_read.func = std::bind(&template_parser_t::parse_any_value_keywrod, this, std::placeholders::_1);
		   event_process.list_events.push_back({ "class", (std::flag32_t) template_parse_flag_t::template_class, std::bind(&template_parser_t::parse_typename_class_keywrod, this, std::placeholders::_1), false, event_flag_t::event_read_value });
		   event_process.list_events.push_back({ "typename", (std::flag32_t) template_parse_flag_t::template_typename, std::bind(&template_parser_t::parse_typename_class_keywrod, this, std::placeholders::_1), false, event_flag_t::event_read_value });
		   event_process.list_events.push_back({ ",", (std::flag32_t) template_parse_flag_t::template_next, std::bind(&template_parser_t::parse_next_keywrod, this, std::placeholders::_1), false, event_flag_t::event_read_compared });
		   event_process.list_events.push_back({ "=", (std::flag32_t) template_parse_flag_t::template_read, std::bind(&template_parser_t::parse_compared_keywrod, this, std::placeholders::_1), false, event_flag_t::event_read_compared });
	   }
};
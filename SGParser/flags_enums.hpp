#pragma once
#include "flag.hpp"

enum class parse_flag_t : std::flag16_t
{
	unknow = 1 << 0,
	word = 1 << 1,
	space_tab = 1 << 2,
	symbol = 1 << 3,
	group_symbol = 1 << 4,
	new_line = 1 << 5
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

enum cpp_flag_t : std::flag32_t
{
	cpp_unknow = 1 << 0,
	cpp_value = 1 << 1,
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
	cpp_template = 1 << 14,
	cpp_template_block_read = 1 << 15,
};
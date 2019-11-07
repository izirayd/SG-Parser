#pragma once

#include <string>

#include "flags_enums.hpp"

#include "tree.hpp"
#include "words.hpp"

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

	type_t*  base_type  = nullptr;
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

	namespace_t*  parent = nullptr;

	tree_words_t* open_block = nullptr;
	tree_words_t* close_block = nullptr;


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

	int level = -1;

	bool is_template = false;
	bool is_const = false;
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

typedef tree_t<type_t> tree_types_t;

class template_t
{
public:
	std::string name;

	int           level = 1;

	tree_types_t   tree_types;

	template_t* parent = nullptr;

	tree_words_t* open_block = nullptr;
	tree_words_t* close_block = nullptr;

	bool is_global() { return level == 1; }
};

class comment_line_t
{
public:
	std::string name;
};

class coomment_big_t
{
public:
	std::string name;
};
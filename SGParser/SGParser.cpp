#include <iostream>
#include <chrono>

#include "flag.hpp"
#include "tree.hpp"
#include "path.hpp"
#include "file.hpp"

#include <sg.hpp>

#include "json_base.hpp"
#include "executive_parser.hpp"

using namespace std::chrono_literals;

template<template<typename> class C>
void a(){}

void test(tree_words_t* tree)
{
	if (tree->get_value()->is_new_line()
		||
		tree->get_value()->is_space_tab()
		)
		return;

	print_space(tree->level, tree->is_have_sub_elemets());
	printf("%d %s %d parent index: %d\n", tree->level, tree->get_value()->data.c_str(), tree->index, tree->parent->index);
}

int main()
{
	printf("**************************************\n");
	printf("*   SGParser DEV v0.1                *\n");
	printf("**************************************\n");
	printf("\n");

	/*printf("Version: 0.1\n");
	printf("Base parser cpp: std17\n");*/

	json_data_t json_data_cpp_parser;
	json_data_cpp_parser.open_json(L"\\..\\cpp_parser.json");

	printf("\nVersion: %s name: %s\n\n", json_data_cpp_parser.header.version.c_str(), json_data_cpp_parser.header.name.c_str());

	for (auto& it : json_data_cpp_parser.parser_data.keywords)
	{
		printf("Keyword: %s flag: %u\n", it.keyword.c_str(), it.unique_flag);
	}

	file_t cpp_file;
	path_t cpp_path;

	parser::executive::parser_t parser_cpp;

	parser_cpp.init(&json_data_cpp_parser.parser_data);

	cpp_file.OpenFile(dir_t(L"\\..\\test.cpp"));

	if (cpp_file)
	{
		int size_file = cpp_file.GetSizeFile();

		char* buffer_code = new char[size_file + 1];

		cpp_file.FullReadFile(buffer_code, 1);
		buffer_code[size_file] = 0x00;

		parser_cpp.code = buffer_code;

		cpp_file.CloseFile();
	}
	else
	{
		printf("Can`t open file test.cpp\n");
		return 0;
	}

	if (parser_cpp.code.empty())
	{
		printf("File empty test.cpp\n");
		return 0;
	}

	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

	parser_cpp.process_parse();

	int32_t new_position = -1;
	std::vector<std::pair<std::string, std::string>> array_symbols;

	for (auto& it : json_data_cpp_parser.parser_data.recursive_blocks)	{
		array_symbols.push_back({ it.open, it.close });
	}

	parser_cpp.block_parse(&parser_cpp.tree, 0, parser_cpp.words.words.size(), new_position, array_symbols, &parser_cpp.words);

	parser_cpp.parse_tree();

	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double, std::milli> result = end - start;

	char title[256] = { 0 };
	sprintf(title, "title Process parse end: %2.fms\n", result.count());

	std::system(title);

	printf("\n");
	printf("**************************************\n");
	printf("*   DEBUG WORDS TREE                 *\n");
	printf("**************************************\n");
	printf("\n");

	printf("Words tree: \n+ %s\n", json_data_cpp_parser.header.name.c_str());

	parser_cpp.tree.process_function["base"] = test;
	parser_cpp.tree.start_process();

	printf("\n");
	printf("**************************************\n");
	printf("*   ELEMENTS ANALYSIS RESULT         *\n");
	printf("**************************************\n");
	printf("\n");

	//parser_cpp.cpp_context.elements.print();

	for (;;) {
	
	}

/*	file_t cpp_file;
	path_t cpp_path;

	parser_cpp_t parser_cpp;

	cpp_file.OpenFile(dir_t(L"\\..\\test.cpp"));

	if (cpp_file)
	{
		int size_file = cpp_file.GetSizeFile();

		char* buffer_code = new char[size_file + 1];

		cpp_file.FullReadFile(buffer_code, 1);
		buffer_code[size_file] = 0x00;

		parser_cpp.code = buffer_code;

		cpp_file.CloseFile();
	}
	else
	{
		printf("Can`t open file test.cpp\n");
		return 0;
	}

	if (parser_cpp.code.empty())
	{
		printf("File empty test.cpp\n");
		return 0;
	}

	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

	parser_cpp.process_parse();

	int32_t new_position = -1;
	std::vector<std::pair<std::string, std::string>> array_symbols;

	array_symbols.push_back({"{", "}"});
	array_symbols.push_back({"(", ")"});
	array_symbols.push_back({"[", "]"});
	array_symbols.push_back({"<", ">"});

	parser_cpp.block_parse(&parser_cpp.tree, 0, parser_cpp.words.words.size(), new_position, array_symbols, &parser_cpp.words);

	parser_cpp.init_event_process();

	parser_cpp.parse_tree();

	parser_cpp.cpp_context.elements.process_sort();

	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double, std::milli> result = end - start;


	char title[256] = { 0 };
	sprintf(title, "title Process parse end: %2.fms\n", result.count());

	std::system(title);

	printf("\n");
	printf("**************************************\n");
	printf("*   DEBUG WORDS TREE                 *\n");
	printf("**************************************\n");
	printf("\n");

	printf("Words tree: \n+ CppRoot\n");
	parser_cpp.tree.process_function["base"] = test;
	parser_cpp.tree.start_process();

	printf("\n");
	printf("**************************************\n");
	printf("*   ELEMENTS ANALYSIS RESULT         *\n");
	printf("**************************************\n");
	printf("\n");

	parser_cpp.cpp_context.elements.print();

	for (;;)
	{

	}*/
}

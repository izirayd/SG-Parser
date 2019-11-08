#include <iostream>
#include <chrono>

#include "flag.hpp"
#include "tree.hpp"
#include "path.hpp"
#include "file.hpp"

#include <sg.hpp>

using namespace std::chrono_literals;

void print_space(int count)
{
	if (count == 0)
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

void test(tree_words_t* tree)
{
	if (tree->get_value()->is_new_line()
		||
		tree->get_value()->is_space_tab()
		)
		return;

	print_space(tree->level);
	printf("%d %s %d parent index: %d\n", tree->level, tree->get_value()->data.c_str(), tree->index, tree->parent->index);
}

int main()
{
	file_t cpp_file;
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

	printf("Process parse end: %2.fms\n", result.count());

	parser_cpp.tree.process_function["base"] = test;
	parser_cpp.tree.start_process();

	parser_cpp.cpp_context.elements.print();

	for (;;)
	{

	}
}

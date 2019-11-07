#pragma once

#include "cpp_objects.hpp"

class cpp_element_t
{
public:
	/*
	   Любой элемент изначально глобален
	   Его требуется принудительно перемещать на другой уровень
	*/
	namespace_t*    namespace_value        = nullptr;
	template_t*     template_value         = nullptr;
	class_t*        class_value            = nullptr;
	comment_line_t* comment_line_value     = nullptr;
	coomment_big_t* comment_big_value      = nullptr;

	words_base_t* element = nullptr; // Нельзя уничтожать

	int           level = -1;

	tree_words_t* open_block;
	tree_words_t* close_block;

	void free()
	{
		if (class_value) {
			delete class_value;
			class_value = nullptr;
		}

		if (namespace_value) {
			delete namespace_value;
			namespace_value = nullptr;
		}

		if (template_value) {
			delete template_value;
			template_value = nullptr;
		}
	}

	// TODO: flags
	bool is_class()     { return class_value     != nullptr; }
	bool is_namespace() { return namespace_value != nullptr; }
	bool is_template()  { return template_value  != nullptr; }
};

typedef tree_t<cpp_element_t*> tree_element_t;
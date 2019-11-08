#pragma once

#include <vector>
#include <string>
#include <functional>

#include "function_wrapper.hpp"

namespace std { template<class T> inline void clear(std::vector<T>& vec) { std::vector<T>().swap(vec); } }

template <typename data_value_t>
class tree_t
{
private:

	data_value_t          value;

	std::vector<tree_t<data_value_t>*> tree;

	bool    is_value = false;

	tree_t<data_value_t>* push_tree(tree_t<data_value_t>* tree) {

		tree->root  = root;

		if (tree->index == 0) {
			tree->index = root->last_index;
			root->last_index++;
		}

		tree->is_root = false;
		tree->parent  = this;

		this->tree.push_back(tree);
		return tree;
	}

public:
	
	void sync(tree_t<data_value_t>* tree)
	{
		tree->is_value = false;
		tree->root     = root;
		tree->parent   = this;

		if (index == 0)
		{
			tree->index = root->last_index;
			root->last_index++;
		}
	}

	data_value_t *last_element() {
	
		if (tree.empty())
			return nullptr;

		return &tree.back()->get_value();
	}


	tree_t<data_value_t>* parent = nullptr; // родитель
	tree_t<data_value_t>* root   = nullptr;
	bool    is_root    = true;
	int     last_index = 0;
	int     index      = 0;

	tree_t() {
		root   = this; 
		parent = root;
	}

	tree_t(const data_value_t& data) {
		value = data;
		is_value = true;
		root = this;
		parent = root;
	}

	tree_t(const std::initializer_list<data_value_t>& v)
	{
		root = this;
		parent = root;

		for (const auto& itm : v)
			push(itm);
	}

	inline tree_t<data_value_t>* push(const std::initializer_list<data_value_t>& v) {

		tree_t<data_value_t>* last_element = nullptr;

		for (const auto& itm : v)
			last_element = push(itm);

		return last_element;
	}

	inline tree_t<data_value_t>* push() { return nullptr; }

	template <class... next_t> inline tree_t<data_value_t>* push(next_t... args) {
		return push(args...);
	}

	inline tree_t<data_value_t>* push(const data_value_t& data) {
		return push_tree(new tree_t<data_value_t>(data));
	}

	inline tree_t<data_value_t>* push(tree_t<data_value_t>* child) {
		return push_tree(child);
	}

	template <class first_t, class... next_t>
	inline tree_t<data_value_t>* push(const first_t& first, const next_t& ... next) {
		push_tree(new tree_t<data_value_t>(first));
		return push(next...);
	}

	std::function<void(tree_t<data_value_t>*)> func;

	detail::function_wrapper_t process_function;

	std::size_t size() { return tree.size(); }

	void stop_process()
	{
		root->is_process = false;
	}

	template <typename... arguments>
	void start_process(arguments&& ... args)
	{
		root->is_process = true;

		int lvl = 0;
		process(this, lvl, (args)...);

		root->is_process = false;
	}

	template <typename... arguments>
	void process(tree_t* tree, int lvl, arguments&& ... args)
	{
		if (!root->is_process)
			return;

		if (!tree)
			return;

		tree->level = lvl;

		if (tree->is_value)
			process_function.call("base", *tree, (args)...);
		
		for (size_t i = 0; i < tree->tree.size(); i++)
		{
			if (tree->tree[i])
			{
				lvl++;
				process(tree->tree[i], lvl, (args)...);
				lvl--;
			}
		}
	}

	int level = 0;

	void delete_tree()
	{
		for (size_t i = 0; i < tree.size(); i++)
		{
			if (tree[i]) {

				tree[i]->delete_tree();

				//printf("delete: %d [%d]\n", tree[i]->value, tree[i]->level);

				delete tree[i];
				tree[i] = nullptr;
			}
		}

		std::clear(tree);
	}

	void* user_data;

	data_value_t  get_value() const  { return value; }
	data_value_t& get_value() { return value; }

	void set_value(const data_value_t& new_value) {
		value = new_value;
		is_value = true;
	}

	private:
		bool is_process = false;

};

#pragma once
#include "json.hpp"
#include "file.hpp"
#include "path.hpp"

using nlohmann::json;	

class error_t
{
public:
	error_t() = default;
	~error_t() = default;

	bool        isError = false;
	std::string TextError = "";

	error_t(std::string Text_Error) { isError = true;  TextError = Text_Error; }

	inline operator bool() { return isError; }
	inline operator const char* ()       const { return TextError.c_str(); }
	inline operator const std::string() const { return TextError; }

	error_t& operator = (std::string Text_Error) {
		return *this = Text_Error.c_str();
	}
	error_t& operator = (const char* Text_Error) {
		isError = true;
		TextError = Text_Error;
#if defined(PLATFORM_ANDROID)
		LOGE("%s", TextError.c_str());
#endif
#if defined(PLATFORM_WINDOWS)
		printf("%s\n", TextError.c_str());
#endif
		return *this;
	}
	error_t& operator = (const bool  newStatus) { isError = newStatus;                    return *this; }

	error_t format(const char* text, ...)
	{
		char Buffer[512];

		va_list vl;
		va_start(vl, text);
		VSNPRINTF(Buffer, 511, text, vl);
		va_end(vl);

		*this = Buffer;

		return *this;
	}

};

namespace parser
{
	namespace regulations
	{
		struct recursive_block_t 
		{
			std::string name;
			std::string open;
			std::string close;
		};

		using recursive_blocks_t = std::vector<recursive_block_t>;

		struct result_t {
			std::vector<std::string> data;
		};

		struct property_t
		{                
			std::string type;
			/* name */    /* value */
			std::vector<std::pair<std::string,  std::string>> data;
		};

		struct property_parser_t {
			std::vector<property_t> data;
		};

		struct associated_with_t {
			std::vector<std::pair<std::string, std::string>> data;
		};

		int last_flag_keyword = 0;

		std::flag32_t value_flag = 1;
		std::flag32_t block_flag = 2;

		struct keyword_t
		{			
			std::flag32_t unique_flag = 0;

			void generate_flag() {
				int last_flag = last_flag_keyword;
				last_flag_keyword++;

				std::add_flag(unique_flag, 1 << last_flag);

				std::clear_flag(value_flag);
				std::clear_flag(block_flag);

				std::add_flag(value_flag, 1 << (last_flag + 1));
				std::add_flag(block_flag, 1 << (last_flag + 2));
			}

			std::string name;
			std::string keyword;
			std::string type;
			std::string block;

			result_t          result;
			property_parser_t property_parser;
			associated_with_t associated_with;

			bool is_value = false;
		};

		class ex_parser_data_t
		{
		  public:
			recursive_blocks_t             recursive_blocks;
			std::vector<keyword_t>         keywords;
		};
	}
}


class json_data_t
{
public:
	struct header_t
	{
		std::string type_file;
		std::string version;
		std::string name;
		std::string descriptor;
	};

	header_t header;
	parser::regulations::ex_parser_data_t parser_data;

	public:
		error_t get_json(const std::wstring& path_to_json, json& json_code, bool is_local_directory = true)
		{
			error_t result;

			if (path_to_json.empty())
				return result = "Path to json script is empty";

			path_t path;
			file_t file;

			if (is_local_directory)
				file.OpenFile(path.GetPathDir(path_to_json.c_str()), file_status_t::READ, table_encoding_t::ORI_UTF8);
			else
				file.OpenFile(path_to_json.c_str(), file_status_t::READ, table_encoding_t::ORI_UTF8);

			if (!file)
			{
				return result.format("No json file %s", file.cfilename.c_str());
			}

			if (file.GetSizeFile() == 0)
			{
				return result.format("Json file %s is empty", file.cfilename.c_str());
			}

			char* buffer_for_parse = new char[file.Size + 1];

			file.FullReadFile(buffer_for_parse, 1);
			file.CloseFile();

			buffer_for_parse[file.Size] = '\0';

			json_code.clear();

			try {
				json_code = json::parse(buffer_for_parse, buffer_for_parse + strlen(buffer_for_parse));
			}
			catch (const json::parse_error& error_text) {

				delete[] buffer_for_parse;
				return result.format("Error parse json file: %s because: %s\n", file.cfilename.c_str(), error_text.what());
			}

			delete[] buffer_for_parse;
			return result;
		}

		error_t get_element(std::string& data_str, const std::string& key, json& element)
		{
			error_t error;

			try
			{
				data_str = element[key];
			}
			catch (const json::type_error& error_text)
			{
				error.format("Error data! %s for key[%s]", error_text.what(), key.c_str());
			}
			
			return error;
		}

		void open_json(const std::wstring& file_path) {

			error_t error;
			json    json_result;

			get_json(file_path.c_str(), json_result);

			try {
				header.type_file  = json_result["type_file"];
				header.version    = json_result["version"];
				header.name       = json_result["name"];
				header.descriptor = json_result["descriptor"];
			}
			catch (const json::type_error& error_text) { error.format("Error data! %s", error_text.what()); }

			try
			{
				for (auto& element : json_result["recursive blocks"])
				{
					parser::regulations::recursive_block_t rb;

					get_element(rb.name,  "name",  element);
					get_element(rb.open,  "open",  element);
					get_element(rb.close, "close", element);

					parser_data.recursive_blocks.push_back(rb);
				}
			}
			catch (const json::type_error& error_text) 	{
				error.format("Error data! %s", error_text.what());
			}

			try
			{
				for (auto& element : json_result["keywords"])
				{
					parser::regulations::keyword_t keyword;

					get_element(keyword.name,    "name",    element);
					get_element(keyword.keyword, "keyword", element);
					get_element(keyword.type,    "type",    element);
					get_element(keyword.block,   "block",   element);

					keyword.result.data.clear();

					for (auto& result_element : json(element["result"]))
						keyword.result.data.push_back({ result_element.get<std::string>() });

					for (auto& result_element : element["property parser"].items()) {
					
						std::string arg1;
						std::string arg2;
						
						arg1 = result_element.key();

						if (arg1 == "contains")
						{
							parser::regulations::property_t p;

							for (auto& compared_element : element["property parser"]["contains"].items()) {
								p.type = arg1;
								p.data.push_back({ compared_element.key(), compared_element.value() });
							}

							keyword.property_parser.data.push_back(p);
						}
						else
						if (arg1 == "compared")
						{
							parser::regulations::property_t p;

							for (auto& compared_element : element["property parser"]["compared"].items()) { 
						  	  p.type = arg1;
							  p.data.push_back({ compared_element.key(), compared_element.value() });
							}

							keyword.property_parser.data.push_back(p);
						}
						else
						{
							arg2 = result_element.value().get<std::string>();

							if (arg2 == "value")
							{
								keyword.is_value = true;
							}

							parser::regulations::property_t p;

							p.type = arg1;
							p.data.push_back({ result_element.key(), result_element.value() });

							keyword.property_parser.data.push_back(p);
						}
					}

					keyword.generate_flag();
					parser_data.keywords.push_back(keyword);
				}
			}
			catch (const json::type_error& error_text) {
				error.format("Error data! %s", error_text.what());
			}
		}
};
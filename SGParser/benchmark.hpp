#pragma once
#include <chrono>
#include <string>

using namespace std::chrono_literals;

class benchmark_t
{
	public:
		void start()
		{
			start_timer = std::chrono::high_resolution_clock::now();
		}

		void end()
		{
			end_timer = std::chrono::high_resolution_clock::now();
			result = end_timer - start_timer;
		}

		std::chrono::duration<double, std::milli> get_result() {
			return result;
		}

		void print(const std::string &start_text)
		{
			printf("%s%2.fms\n", start_text.c_str(), get_result().count());
		}

	private:
		std::chrono::high_resolution_clock::time_point start_timer;
		std::chrono::high_resolution_clock::time_point end_timer;

		std::chrono::duration<double, std::milli> result;
};
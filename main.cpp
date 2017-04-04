#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <atomic>
#include <thread>	
#include <chrono>
#include <vector>

#include <boost\filesystem.hpp>
#include <boost\filesystem\operations.hpp>

#include "TSSQueue.h";

using uli = unsigned long int;
using clk = std::chrono::steady_clock;

void Search(boost::filesystem::path root_folder);
void Parse();
void Log();

CFParser::TSSQueue files;
std::atomic<bool> parsing = true;

std::atomic<uli> files_cnt;
std::atomic<uli> all_lines_cnt;
std::atomic<uli> blank_lines_cnt;
std::atomic<uli> comment_lines_cnt;

long long parsing_time;
long long searching_time;

int main(int argc, char** argv)
{	
	// argc[0] - system info
	boost::filesystem::path root_folder = (argc > 1) ? argv[1] :  R"(D:\~SS\VS_SS_workspace\CFP_00\CFP_00\FindMe)";

	std::cout << "Root folder: " << root_folder << std::endl;

	unsigned int optimum_thread_count = std::thread::hardware_concurrency();
	std::cout << "Optimum thread count: " << optimum_thread_count << std::endl;
	std::vector<std::thread> parsers;

	clk::time_point begin = clk::now();

	std::thread searcher(Search, root_folder);

	for (unsigned int i = 0; i <= optimum_thread_count - 1; i++)
		parsers.emplace_back(std::thread(Parse));

	Parse();
	searcher.join();

	for (unsigned int i = 0; i <= optimum_thread_count - 1; i++)
		parsers[i].join();

	clk::time_point finish = clk::now();
	parsing_time = std::chrono::duration_cast<std::chrono::microseconds>(finish - begin).count() / CLOCKS_PER_SEC;

	Log();

	system("pause");
	return 0;
}

void Search(boost::filesystem::path root_folder)
{
	std::string temp_file_path = "";
	std::regex rx_cpp(R"(.*\.(c|h|cpp|hpp)$)");
	boost::filesystem::recursive_directory_iterator dir(root_folder), end;

	while (dir != end)
	{
		temp_file_path = dir->path().string();

		if (std::regex_match(temp_file_path, rx_cpp))
		{
			files.Push(temp_file_path);
		}

		++dir;
	}
	parsing = false;
}

void Parse()
{
	std::ifstream parse_stream;
	std::string file_path = "";
	while (parsing || !files.Empty())
	{
		file_path = files.Pop();

		// Prepare for file parsing
		parse_stream.open(file_path);
		if (!parse_stream.is_open())
		{
			std::cout << "Error opening file!" << std::endl;
			system("pause");
			return;
		}

		++files_cnt;

		bool is_multi_comment = false;
		for (std::string line; std::getline(parse_stream, line);)
		{	// Parsing starts here
			++all_lines_cnt;

			// Removing whitespaces
			line.erase(std::remove_if(line.begin(), line.end(),
				/*checking whitespace*/ [](char c) { return (c == '\r' || c == '\t' || c == ' ' || c == '\n'); }), line.end());

			if (line.empty())
			{
				++blank_lines_cnt;
			}
			else if (is_multi_comment)	
			{
				++comment_lines_cnt;
				if (std::string::npos != line.find("*/"))
				{	// Multiline comment ends here
					is_multi_comment = false;
				}
			}
			else if (0 == line.find("//"))	// One-line comment 
			{
				++comment_lines_cnt;
			}
			else if (std::string::npos != line.find("/*"))	 // Multiline comment starts here
			{
				++comment_lines_cnt;
				is_multi_comment = true;
			}
		}
		parse_stream.close();
	}
}

void Log()
{
	std::ofstream prog_log;
	prog_log.open("log.txt");
	if (!prog_log.is_open())
	{
		std::cout << "Error opening log file" << std::endl;
		return;
	}

	prog_log << "Count of proceeded files:\t" << files_cnt << std::endl;
	prog_log << "Count of all lines:\t\t" << all_lines_cnt << std::endl;
	prog_log << "Count of blank lines:\t\t" << blank_lines_cnt << std::endl;
	prog_log << "Count of commented lines:\t" << comment_lines_cnt << std::endl;
	prog_log << "Count of code lines:\t\t" << all_lines_cnt - (blank_lines_cnt + comment_lines_cnt) << std::endl;

	prog_log << "Total time taken:\t\t\t" << parsing_time << " ms" << std::endl;

	prog_log.close();
}

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

struct Data
{
	Data() : files(), searching(true), parsing_time(0), files_cnt(0), all_lines_cnt(0), blank_lines_cnt(0), comment_lines_cnt(0)
	{}

	CFParser::TSSQueue files;
	bool searching;
	long long parsing_time;

	std::atomic<uli> files_cnt;
	std::atomic<uli> all_lines_cnt;
	std::atomic<uli> blank_lines_cnt;
	std::atomic<uli> comment_lines_cnt;
};

void Search(boost::filesystem::path root_folder, Data& data);
void Parse(Data& data);
void SaveResults(const Data& data, std::ostream& s = std::cout);

int main(int argc, char** argv)
{	
	// argc[0] - system info
	boost::filesystem::path root_folder = (argc > 1) ? argv[1] :  R"(D:\~SS\VS_SS_workspace\CFP_00\CFP_00\FindMe)";
	std::string info_file_name = (argc > 2) ? argv[2] : "info.txt";

	std::cout << "Root folder: " << root_folder << std::endl;

	unsigned int optimum_thread_count = std::thread::hardware_concurrency();
	std::cout << "Optimum thread count: " << optimum_thread_count << std::endl;
	
	Data data;
	std::vector<std::thread> parsers;

	clk::time_point begin = clk::now();
	
	std::thread searcher(Search, root_folder, std::ref(data));

	for (unsigned int i = 0; i < optimum_thread_count - 1; i++)
		parsers.emplace_back(std::thread(Parse, std::ref(data)));

	searcher.join();

	for (unsigned int i = 0; i < optimum_thread_count - 1; i++)
		parsers[i].join();

	clk::time_point finish = clk::now();
	data.parsing_time = std::chrono::duration_cast<std::chrono::microseconds>(finish - begin).count() / CLOCKS_PER_SEC;

	// Information console output 
	SaveResults(data);

	// Information saving to file
	std::ofstream save_info;
	save_info.open(info_file_name);
	if (!save_info.is_open())
	{
		std::cout << "Error opening log file" << std::endl;
	}
	else
	{
		SaveResults(data, save_info);
		save_info.close();
	}

	system("pause");
	return 0;
}

void Search(boost::filesystem::path root_folder, Data& data)
{
	std::string temp_file_path = "";
	std::regex rx_cpp(R"(.*\.(c|h|cpp|hpp)$)");

	for(boost::filesystem::recursive_directory_iterator dir(root_folder), end; dir != end; dir++)
	{
		temp_file_path = dir->path().string();

		if (std::regex_match(temp_file_path, rx_cpp))
		{
			data.files.Push(temp_file_path);
		}
	}
	data.searching = false;

	// start parsing in thread
	Parse(data);
}

void Parse(Data& data)
{
	std::ifstream parse_stream;
	std::string file_path = "";
	while (data.searching || !data.files.Empty())
	{
		file_path = data.files.Pop();

		// Prepare for file parsing
		parse_stream.open(file_path);
		if (!parse_stream.is_open())
		{
			std::cout << "Error opening file \"" << file_path << "\"" << std::endl;
			continue;
		}

		++data.files_cnt;

		std::string line;
		bool is_multi_comment = false;
		while(std::getline(parse_stream, line))
		{	// Parsing starts here
			++data.all_lines_cnt;

			// Removing whitespaces
			line.erase(std::remove_if(line.begin(), line.end(),
				/*checking whitespace*/ [](char c) { return (c == '\r' || c == '\t' || c == ' ' || c == '\n'); }), line.end());

			if (line.empty())
			{
				++data.blank_lines_cnt;
			}
			else if (is_multi_comment)	
			{
				++data.comment_lines_cnt;
				if (std::string::npos != line.find("*/"))
				{	// Multiline comment ends here
					is_multi_comment = false;
				}
			}
			else if (0 == line.find("//"))	// One-line comment 
			{
				++data.comment_lines_cnt;
			}
			else if (std::string::npos != line.find("/*"))	 // Multiline comment starts here
			{
				if (line.find("/*") < 1)	// Comment from starting of a line
					++data.comment_lines_cnt;
											// Otherwise, code - than comment, than we won't increment comment lines
				
				if(std::string::npos == line.find("*/"))	// If it finishes in this line
					is_multi_comment = true;
			}
		}
		parse_stream.close();
	}
}

void SaveResults(const Data& data, std::ostream& s)
{
	s << "Count of proceeded files:\t" << data.files_cnt << std::endl;
	s << "Count of all lines:\t\t" << data.all_lines_cnt << std::endl;
	s << "Count of blank lines:\t\t" << data.blank_lines_cnt << std::endl;
	s << "Count of commented lines:\t" << data.comment_lines_cnt << std::endl;
	s << "Count of code lines:\t\t" << data.all_lines_cnt - (data.blank_lines_cnt + data.comment_lines_cnt) << std::endl;
	
	s << "Total time taken:\t\t" << data.parsing_time << " ms" << std::endl;
}

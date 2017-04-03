// standart
#include <iostream>
#include <string>
#include <regex>
#include <atomic>
#include <thread>	

//boost
#include <boost\filesystem.hpp>
#include <boost\filesystem\operations.hpp>

#include "TSSQueue.h";

using uli = unsigned long int;

void Search(boost::filesystem::path root_folder);
void Parse();

CFParser::TSSQueue files;
std::atomic<bool> parsing = true;

uli files_cnt;
uli all_lines_cnt;
uli blank_lines_cnt;
uli comment_lines_cnt;

int main(int argc, char** argv)
{
	boost::filesystem::path root_folder{ R"(D:\Libraries\boost_1_63_0\boost)" };
	
	std::thread parser(Parse);
	std::thread searcher(Search, root_folder);	

	searcher.join();
	parser.join();

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
			// found :)
			files.Push(temp_file_path);

			// DELETE ME
			//std::cout << temp_file_path << std::endl;
		}
		++dir;
	}

	parsing = false;
}

void Parse()
{
	// TODO chrono
	clock_t start = clock();

	std::ifstream parse_stream;
	std::string path_file_to_parse = "";
	while (parsing || !files.Empty())
	{
		if (files.Empty())
			continue;

		path_file_to_parse = files.Pop();

		// Prepare for file parsing
		parse_stream.open(path_file_to_parse);
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
			else if (is_multi_comment)		// Check if we're in multiline comment
			{
				++comment_lines_cnt;
				if (std::string::npos != line.find("*/"))
				{	// If multiline comment ends here
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

	std::cout << "Count of proceeded files:\t" << files_cnt << std::endl;
	std::cout << "Count of all lines:\t\t" << all_lines_cnt << std::endl;
	std::cout << "Count of blank lines:\t\t" << blank_lines_cnt << std::endl;
	std::cout << "Count of commented lines:\t" << comment_lines_cnt << std::endl;
	std::cout << "Count of code lines:\t\t" << all_lines_cnt - (blank_lines_cnt + comment_lines_cnt) << std::endl;

	printf("Time taken: %.3fs\n", (double)(clock() - start) / CLOCKS_PER_SEC);
}

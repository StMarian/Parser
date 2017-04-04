#include "functions.h"

namespace CFParser
{

void Search(boost::filesystem::path root_folder, Data & data)
{
	for (boost::filesystem::recursive_directory_iterator dir(root_folder), end; dir != end; dir++)
	{
		std::string temp_file_path = dir->path().string();

		if (std::regex_match(temp_file_path, data.rx))
		{
			data.files.Push(temp_file_path);
		}
	}
	data.searching = false;

	// start parsing in thread
	Parse(data);
}

void Parse(Data & data)
{
	while (data.searching || !data.files.Empty())
	{
		std::string file_path = data.files.Pop();

		// Prepare for file parsing
		std::ifstream parse_stream;
		parse_stream.open(file_path);
		if (!parse_stream.is_open())
		{
			std::cout << "Error opening file \"" << file_path << "\"" << std::endl;
			continue;
		}

		++data.files_cnt;

		std::string line;
		bool is_multi_comment = false;
		while (std::getline(parse_stream, line))
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

				if (std::string::npos == line.find("*/"))	// If it finishes in this line
					is_multi_comment = true;
			}
		}
		parse_stream.close();
	}
}

void SaveResults(const Data & data, std::ostream & s)
{
	s << "Count of proceeded files:\t" << data.files_cnt << std::endl;
	s << "Count of all lines:\t\t" << data.all_lines_cnt << std::endl;
	s << "Count of blank lines:\t\t" << data.blank_lines_cnt << std::endl;
	s << "Count of commented lines:\t" << data.comment_lines_cnt << std::endl;
	s << "Count of code lines:\t\t" << data.all_lines_cnt - (data.blank_lines_cnt + data.comment_lines_cnt) << std::endl;

	s << "Total time taken:\t\t" << data.parsing_time << " ms" << std::endl;
}

}

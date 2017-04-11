#include "Data.h"

namespace CFParser
{

void Data::Search(const boost::filesystem::path& root_folder)
{
	for (boost::filesystem::recursive_directory_iterator dir(root_folder), end; dir != end; dir++)
	{
		std::string temp_file_path = dir->path().string();

		if (std::regex_match(temp_file_path, rx_))
		{
			files_.Push(temp_file_path);
		}
	}

	iam_searching_ = false;

	Parse();
}

void Data::Parse()
{
	while (iam_searching_ || !files_.IsEmpty())
	{
		std::string file_path = files_.Pop();

		// Prepare for file parsing
		std::ifstream parse_stream;
		parse_stream.open(file_path);
		if (!parse_stream.is_open())
		{
			std::cout << "Error opening file \"" << file_path << "\"" << std::endl;
			continue;
		}

		++files_cnt_;

		std::string line;
		bool is_multi_comment = false;
		while (std::getline(parse_stream, line))
		{	// Parsing starts here
			++all_lines_cnt_;

			// Removing whitespaces
			line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());

			if (line.empty())
			{
				++blank_lines_cnt_;
			}
			else if (is_multi_comment)
			{
				++comment_lines_cnt_;
				if (std::string::npos != line.find("*/"))
				{	// Multiline comment ends here
					is_multi_comment = false;
				}
			}
			else if (0 == line.find("//"))	// One-line comment 
			{
				++comment_lines_cnt_;
			}
			else if (std::string::npos != line.find("/*"))	 // Multiline comment starts here
			{
				if (line.find("/*") < 1)	// Comment from starting of a line
					++comment_lines_cnt_;
				// Otherwise, code - than comment, than we won't increment comment lines

				if (std::string::npos == line.find("*/"))	// If it finishes in this line
					is_multi_comment = true;
			}
		}
		parse_stream.close();
	}
}

void Data::SaveResults(std::ostream& s)
{
	s << "Count of proceeded files:\t" << files_cnt_ << std::endl
	  << "Count of all lines:\t\t" << all_lines_cnt_ << std::endl
	  << "Count of blank lines:\t\t" << blank_lines_cnt_ << std::endl
	  << "Count of commented lines:\t" << comment_lines_cnt_ << std::endl
	  << "Count of code lines:\t\t" << all_lines_cnt_ - (blank_lines_cnt_ + comment_lines_cnt_) << std::endl
	  << "Total time taken:\t\t" << total_time_ << " ms" << std::endl;
}

}

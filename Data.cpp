#include "Data.h"

namespace CFParser
{

void Data::Search(boost::filesystem::path root_folder)
{
	for (boost::filesystem::recursive_directory_iterator dir(root_folder), end; dir != end; dir++)
	{
		std::string temp_file_path = dir->path().string();

		if (std::regex_match(temp_file_path, m_rx))
		{
			m_files.Push(temp_file_path);
		}
	}
}

void Data::Parse()
{
	while (!m_files.Empty())
	{
		std::string file_path = m_files.Pop();

		// Prepare for file parsing
		std::ifstream parse_stream;
		parse_stream.open(file_path);
		if (!parse_stream.is_open())
		{
			std::cout << "Error opening file \"" << file_path << "\"" << std::endl;
			continue;
		}

		++m_files_cnt;

		std::string line;
		bool is_multi_comment = false;
		while (std::getline(parse_stream, line))
		{	// Parsing starts here
			++m_all_lines_cnt;

			// Removing whitespaces
			//line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
			line.erase(std::remove_if(line.begin(), line.end(),
				/*checking whitespace*/ [](char c) { return (c == '\r' || c == '\t' || c == ' ' || c == '\n'); }), line.end());

			if (line.empty())
			{
				++m_blank_lines_cnt;
			}
			else if (is_multi_comment)
			{
				++m_comment_lines_cnt;
				if (std::string::npos != line.find("*/"))
				{	// Multiline comment ends here
					is_multi_comment = false;
				}
			}
			else if (0 == line.find("//"))	// One-line comment 
			{
				++m_comment_lines_cnt;
			}
			else if (std::string::npos != line.find("/*"))	 // Multiline comment starts here
			{
				if (line.find("/*") < 1)	// Comment from starting of a line
					++m_comment_lines_cnt;
				// Otherwise, code - than comment, than we won't increment comment lines

				if (std::string::npos == line.find("*/"))	// If it finishes in this line
					is_multi_comment = true;
			}
		}
		parse_stream.close();
	}
}

void Data::SaveResults(std::ostream & s)
{
	s << "Count of proceeded files:\t" << m_files_cnt << std::endl;
	s << "Count of all lines:\t\t" << m_all_lines_cnt << std::endl;
	s << "Count of blank lines:\t\t" << m_blank_lines_cnt << std::endl;
	s << "Count of commented lines:\t" << m_comment_lines_cnt << std::endl;
	s << "Count of code lines:\t\t" << m_all_lines_cnt - (m_blank_lines_cnt + m_comment_lines_cnt) << std::endl;
	s << "Total time taken:\t\t" << m_total_time << " ms" << std::endl;
}

bool Data::set_TotalTime(long long time)
{
	if (time < 0)
		return false;

	m_total_time = time;
	return true;
}

}

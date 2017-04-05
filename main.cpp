#include <thread>	
#include <chrono>
#include <vector>

#include "Data.h"
using namespace CFParser;

int main(int argc, char** argv)
{	
	// argc[0] - system info
	boost::filesystem::path root_folder = (argc > 1) ? argv[1] :  R"(D:\~SS\VS_SS_workspace\CFP_00\CFP_00\FindMe)";
	std::string info_file_name = (argc > 2) ? argv[2] : "info.txt";

	std::cout << "Root folder: " << root_folder << std::endl;

	unsigned int optimum_thread_count = std::thread::hardware_concurrency();
	std::cout << "Optimum thread count: " << optimum_thread_count << std::endl;
	
	Data data;
	auto search_fn = std::bind(&Data::Search, std::ref(data), root_folder);
	auto parse_fn = std::bind(&Data::Parse, std::ref(data));
	
	clk::time_point begin = clk::now();

	std::thread searcher(search_fn);
	searcher.join();

	auto search_time = std::chrono::duration_cast<std::chrono::microseconds>(clk::now() - begin).count() / CLOCKS_PER_SEC;
	std::cout << "Time taken for searching: " << search_time << "ms" << std::endl;

	std::vector<std::thread> parsers;
	for (unsigned int i = 0; i < optimum_thread_count; i++)
		parsers.emplace_back(std::thread(parse_fn));

	for (unsigned int i = 0; i < optimum_thread_count; i++)
		parsers[i].join();

	clk::time_point finish = clk::now();
	auto parse_time = (std::chrono::duration_cast<std::chrono::microseconds>(finish - begin).count() / CLOCKS_PER_SEC) - search_time;
	std::cout << "Time taken for parsing: " << parse_time << "ms" << std::endl;
	data.set_TotalTime(parse_time + search_time);

	// Information console output 
	data.SaveResults();

	// Information saving to file
	std::ofstream save_info;
	save_info.open(info_file_name);
	if (!save_info.is_open())
	{
		std::cout << "Error opening log file" << std::endl;
	}
	else
	{
		data.SaveResults(save_info);
		save_info.close();
	}

	system("pause");
	return 0;
}
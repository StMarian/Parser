#include <thread>	
#include <chrono>
#include <vector>

#include "functions.h"
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
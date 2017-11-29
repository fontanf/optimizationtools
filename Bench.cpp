#include <thread>
#include <iostream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/program_options.hpp>

#include <ctime>

typedef std::string DataFilename;
typedef std::string Key;
typedef std::string Value;

void executeProgram(std::string s)
{
    system(s.c_str());
}

int main(int argc, char *argv[])
{
	// Parse program options
	std::string exec = "";
	std::string output_file = "";
	time_t time_limit = 0;
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message")
		("input-data,i",  boost::program_options::value<std::vector<std::string>>()->required()->multitoken(), "set input data (required)")
		("output-file,o", boost::program_options::value<std::string>(&output_file)->required(), "set output file (required)")
		("exec,e",        boost::program_options::value<std::string>(&exec)->required(), "set exec (required)")
		("time-limit,t",  boost::program_options::value<time_t>(&time_limit), "set time limit")
		;
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	if (vm.count("help")) {
		std::cout << desc << std::endl;;
		return 1;
	}
	try {
		boost::program_options::notify(vm);
	} catch (boost::program_options::required_option e) {
		std::cout << e.what() << std::endl;
		std::cout << desc << std::endl;;
		return 1;
	}

	// Run algorithm
	std::vector<Key> keys;
	std::map<std::tuple<DataFilename, Key>, Value> res;
	for (std::string data_file: vm["input-data"].as<std::vector<std::string>>()) {
		std::cout << data_file << "... " << std::flush;
		char* output = std::tmpnam(nullptr);
		std::string cmd = exec
				+ " -i " + data_file
				+ " -o " + output;
		if (time_limit != 0)
			cmd += "-t " + std::to_string(time_limit);
		time_t begin = time(NULL);
		std::thread worker(executeProgram, cmd);
		time_t end = time(NULL);
		worker.join();

		if (boost::filesystem::exists(output)) {
			if (std::find(keys.begin(), keys.end(), "Time") == keys.end())
				keys.push_back("Time");
			res[std::make_tuple(data_file, "Time")] = std::to_string(end - begin);
			boost::property_tree::ptree pt;
			boost::property_tree::ini_parser::read_ini(output, pt);
			for (auto& sec: pt) {
				for (auto& kv: sec.second) {
					std::string key   = kv.first;
					std::string value = kv.second.get_value<std::string>();
					if (std::find(keys.begin(), keys.end(), key) == keys.end())
						keys.push_back(key);
					res[std::make_tuple(data_file, key)] = value;
				}
			}
			boost::filesystem::remove(output);
		}
		std::cout << std::endl;
	}

	// Write CSV file
	std::ofstream out;
	out.open(output_file);
	// First Line
	out << "Data";
	for (Key key: keys)
		out << "," << key;
	out << std::endl;
	// Other lines
	for (std::string data_file: vm["input-data"].as<std::vector<std::string>>()) {
		out << data_file;
		for (Key key: keys)
			out << "," << res[std::make_tuple(data_file, key)];
		out << std::endl;
	}
	out.close();

	return 0;
}

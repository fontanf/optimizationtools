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

std::string generateOutputFilename(std::string exec, std::string data_file, time_t time_limit)
{
	std::hash<std::string> hash_fn;
	std::string s;
	s += std::to_string(hash_fn(exec + data_file + std::to_string(time_limit)));
	s +=
		"--" + boost::filesystem::path(exec).filename().string() +
		"--" + boost::filesystem::path(data_file).filename().string() +
		"--" + std::to_string(time_limit) + "s.ini";
	return s;
}

int main(int argc, char *argv[])
{
	// Parse program options
	std::string exec = "";
	std::string output_file = "";
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message")
		("input-data,i",  boost::program_options::value<std::vector<std::string>>()->required()->multitoken(), "set input data (required)")
		("output-file,o", boost::program_options::value<std::string>(&output_file)->required(), "set output file (required)")
		("exec,e",        boost::program_options::value<std::string>(&exec)->required(), "set exec (required)")
		("time-marks,t",  boost::program_options::value<std::vector<time_t>>()->required()->multitoken(), "set time marks")
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
	std::vector<time_t> time_marks = vm["time-marks"].as<std::vector<time_t>>();
	time_t time_limit = time_marks.back();

	for (std::string data_file: vm["input-data"].as<std::vector<std::string>>()) {
		std::cout << data_file << "... " << std::endl;
		std::string output = generateOutputFilename(exec, data_file, time_limit);
		if (boost::filesystem::exists(output))
			continue;
		std::string cmd = exec
				+ " -i " + data_file
				+ " -o " + output
				+ " -t " + std::to_string(time_limit);
		std::thread worker(executeProgram, cmd);
		worker.join();
	}

	// Run algorithm
	std::map<time_t, std::vector<Key>> header;
	std::map<std::tuple<DataFilename, time_t, Key>, Value> res;
	for (std::string data_file: vm["input-data"].as<std::vector<std::string>>()) {
		std::string output = generateOutputFilename(exec, data_file, time_limit);
		if (!boost::filesystem::exists(output))
			continue;
		boost::property_tree::ptree pt;
		boost::property_tree::ini_parser::read_ini(output, pt);
		size_t idx = 0;
		for (time_t t: time_marks) {
			while (pt.find("Solution" + std::to_string(idx+1)) != pt.not_found()
					&& pt.get<time_t>("Solution" + std::to_string(idx+1) + ".Time") <= t) {
				idx++;
			}
			if (pt.find("Solution" + std::to_string(idx)) == pt.not_found())
				continue;
			for (auto& kv: pt.get_child("Solution" + std::to_string(idx))) {
				std::string key   = kv.first;
				std::string value = kv.second.get_value<std::string>();
				std::vector<Key>& keys = header[t];
				if (std::find(keys.begin(), keys.end(), key) == keys.end())
					keys.push_back(key);
				res[std::make_tuple(data_file, t, key)] = value;
			}
		}
	}

	// Write CSV file
	std::ofstream out;
	out.open(output_file);
	// First Line
	out << "Data," << std::flush;
	for (time_t t: time_marks) {
		out << "Time=" << t << "s";
		for (Key key: header[t])
			out << ",";
	}
	out << std::endl;
	// Second line
	for (time_t t: time_marks)
		for (Key key: header[t])
			out << "," << key;
	out << std::endl;
	// Other lines
	for (std::string data_file: vm["input-data"].as<std::vector<std::string>>()) {
		out << data_file;
		for (time_t t: time_marks)
			for (Key key: header[t])
				out << "," << res[std::make_tuple(data_file, t, key)];
		out << std::endl;
	}
	out.close();

	return 0;
}

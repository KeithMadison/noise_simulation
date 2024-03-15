#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/dynamic_bitset.hpp>

#include <functional>
#include <iostream>
#include <vector>
#include <random>
#include <queue>

#include "antenna.h"

Antenna::Antenna(const char* descriptor, std::function<double(double)> freqResponse): gen(rd()){

try{

	boost::property_tree::ptree json;
	boost::property_tree::read_json(descriptor, json);

	double fHigh = json.get<double>("filter.fHigh");
	double fLow  = json.get<double>("filter.fLow");
	
	// Read filter type, and use to compute bandwidth
	std::string filterType = json.get<std::string>("filter.type");

	if(filterType == "simple")
		bandwidth = fHigh - fLow;
	else if (filterType == "butterworth")
		bandwidth = fHigh - fLow;
	else if (filterType == "custom")
		bandwidth = fHigh - fLow;
	else
		throw std::runtime_error("Invalid filter type");

	channelThreshold = json.get<double>("channelThreshold");
	
	samplingRate = json.get<int>("samplingRate")  * 1E+9;
	windowSize   = json.get<double>("windowSize") * samplingRate;
	writeDelay   = json.get<double>("writeDelay") * samplingRate;
	channels     = json.get<int>("channels");

	resistance   = json.get<double>("resistance");
	gain	     = json.get<double>("gain");
	noiseFig     = json.get<double>("noiseFig");

} catch (const boost::property_tree::json_parser::json_parser_error& ex){

		std::cout << "JSON parsing error: " << ex.what() << std::endl;
		throw;

}
	
}

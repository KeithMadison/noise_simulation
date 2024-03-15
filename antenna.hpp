#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/dynamic_bitset.hpp>

#include <functional>
#include <iostream>
#include <vector>
#include <random>
#include <queue>

#ifndef antenna_H
#define antenna_H

class Antenna{

public:
	Antenna(const char* descriptor,
		std::function<double(double)> freqResponse = nullptr);

	std::uint64_t getTriggerRateNumerical(double threshold, double temperature);
	std::uint64_t getTriggerRateProbabilistic(double threshold, double temperature);
	double	      getThermalNoisePower(double temp);
	
	double vrms, highestChanSNR;

private:
	double noiseFig, resistance, gain, bandwidth;
        int windowSize, channels,
            channelThreshold, writeDelay;

        long int samplingRate;

        std::random_device rd;
        std::mt19937 gen;

};

#endif

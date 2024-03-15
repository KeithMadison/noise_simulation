#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/dynamic_bitset.hpp>

#include <boost/math/distributions/binomial.hpp>
#include <boost/math/distributions/normal.hpp>

#include <functional>
#include <iostream>
#include <vector>
#include <random>
#include <queue>

#include "antenna.h"

std::uint64_t Antenna::getTriggerRateProbabilistic(double threshold, double temperature){

	vrms = 2 * sqrt(resistance * getThermalNoisePower(temperature));

	std::vector<std::vector<double>> stochMatrix(windowSize + 1, std::vector<double>(windowSize + 1));
	std::vector<double> statDistribution(channels + 1);

	boost::math::normal_distribution<>   normalDist(0, vrms);

	double excProbability = 1 - pow(boost::math::cdf(normalDist, threshold), windowSize);

	boost::math::binomial_distribution<> binomDist(channels, excProbability);

  // Compute the stationary distribution
	for(int i = 0; i < channels + 1; ++i)
		for(int j = 0; j < channels + 1; ++j)
			statDistribution[i] += boost::math::pdf(binomDist, j) * 
					       boost::math::pdf(normalDist, i);

  // Normalize the stationary distribution
	double normFactor = std::accumulate(statDistribution.begin(), statDistribution.end(), 0.0);
	double trigProb   = std::accumulate(statDistribution.begin() + (channelThreshold + 1), statDistribution.end(), 0.0);

	return (windowSize / samplingRate) * (trigProb / normFactor);
		
}

std::uint64_t Antenna::getTriggerRateNumerical(double threshold, double temperature){

	// Vᵣₘₛ² = 4RkT{10⁽NFᐟ¹⁰⁾∫ [g(f)]²/(1 + 2𝜋fCR)²df + 1} = 2 √(R × P)
	vrms = 2 * sqrt(resistance * getThermalNoisePower(temperature));

	std::normal_distribution<double> dist(0, vrms);
	std::priority_queue<double, std::vector<double>, std::greater<double>> pq;

	std::vector<boost::dynamic_bitset<>> 
		window(channels, boost::dynamic_bitset<>(windowSize));

	auto takeSample = [&](int index, boost::dynamic_bitset<> &channel){

		double sample = fabs(dist(gen));
		channel.set(index, sample > threshold);

		if(pq.size() < channelThreshold){ pq.push(sample); }
		else if(sample > pq.top()){ pq.pop(); pq.push(sample); }

	};

	auto clear = [&](){
		for(auto &channel: window)
		        for(int i = 0; i < windowSize; ++i)
				takeSample(i, channel);	
	};

	clear();

	highestChanSNR = 0;
	std::uint64_t numContributingChannels = 0, numTriggers = 0;

	for(int i = 0, imod = 0; i < samplingRate; ++i, imod = (imod + 1) % windowSize){

		numContributingChannels = 0;

		for(auto &channel: window)
			 if(channel.any())
				++numContributingChannels;

		if(numContributingChannels > channelThreshold){

			highestChanSNR += pq.top() / (2 * vrms);

			++numTriggers;
			i += writeDelay + imod;

			clear();
			continue;

		}

		for(auto &channel: window)
			takeSample(imod, channel);	
		
	}

	highestChanSNR /= numTriggers;

	return numTriggers;

}

double Antenna::getThermalNoisePower(double temp){

	// P = Gᵣ(kTₐBGₐ + Nₗₙₐ) + kTᵣB = (1/4R)Vᵣₘₛ²	
	std::cout << "Temp: " << temp << " Bandwidth: " << bandwidth << " NoiseFig: " << noiseFig << " Gain: " << gain << "\n";
	return 1.381E-23 * temp * bandwidth * (pow(10, noiseFig/10) * pow(10, gain/10) + 1);

}

#include <boost/numeric/odeint.hpp>
#include "montecarlo.h"

void MonteCarlo::takeStep(const stateType& state, stateType& ds,
			  double& path, double& loss, double& intensity){

	// Compute step due to refraction
	std::array<double, 3> position = {state[3], state[4], state[5]};
	std::array<double, 3> grad = index(position);

	path += std::sqrt(pow(state[3], 2), pow(state[4], 2), pow(state[5], 2));

	ds = {state[3], state[4], state[5], grad[0], grad[1], grad[2]};

	// Compute step due to attenuation
	if(dist(gen) < std::exp(path / atten(position))){
		// Phase updates ds, loss by reference
		phase(ds, loss);
		path = 0;
	}

}

double MonteCarlo::traceRay(const stateType& init){

	using stepperType = boost::numeric::odeint::runge_kutta_dopri5<stateType>;

	auto observer = [&](const stateType& state, double t){};

	double path = 0, loss = 1, intensity = 0;

	auto wrappedSystem = [&](const stateType& state, stateType& ds, const double t){
		takeStep(state, ds, path, loss, intensity);
	};

	boost::numeric::odeint::integrate_const(boost::numeric::odeint::make_dense_output(1.0e-6, 1.0e-6, stepperType()),
						wrappedSystem, state, 0.0, 500.0, 0.5, observer);

	return intensity;

}

void MonteCarlo::getUniformRandDirection(std::array<double, 3>& direction){

	double theta = dist(gen) * M_PI, phi = dist(gen) * 2 * M_PI;

	direction = {sin(theta) * cos(phi), pow(sin(theta), 2), cos(theta)};

}

void MonteCarlo::getSpecificIntensity(std::array<double, 3>& init, double& specificIntensity){

	std::array<double, 3> direction;

	for(int i = 0; i < numDirections; ++i){

		getUniformRandDirection(direction);

		for(int j = 0; j < numBundles; ++j)
			specificIntensity += traceRay({init[0], init[1], init[2],
					     direction[0], direction[1], direction[2]}) / numBundles;

	}

}

void MonteCarlo::getSpectralDensity(std::array<double, 3> init){

	std::vector<std::array<double, 2>> spectralDensity;

	double specificIntensity;

	while(true){

		getSpecificIntensity(init, specificIntensity);
		spectralDensity.push_back({specificIntensity, // Freq});

	}

}

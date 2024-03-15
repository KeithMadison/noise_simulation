#include <boost/numeric/odeint.hpp>

#ifndef montecarlo_H
#define montecarlo_G

class MonteCarlo{

public:
	using func1D = std::function<double(std::array<double, 3>&)>;
	using func3D = std::function<std::array<double, 3>(std::array<double, 3>)>;

	MonteCarlo(const func3D index, const func1D temp,
		   const func1D atten, std::array<double, 8> bounds,
		   int numDir, int numFreq, int numBundle);

	void getSpectralDensity(std::array<double, 3> init);

private:
	typedef std::array<double, 6> stateType; // Replace by using

	void takeStep(const stateType& state, stateType& ds, double& freePath, double& loss, double& intensity);
	double traceRay(const stateType &init);
	void getUniformRandDirection(std::array<double, 3>& dir);
	void getSpecificIntensity(std::array<double, 3>& init, double& specificIntensity);
	void phase(const stateType& state, stateType& ds);

	stateType prevState;

	std::uniform_real_distribution<> dist;
	std::random_device rd;
	std::mt19937 gen;

	func3D index;
	func1D atten, temp;

	std::array<double, 8> bounds;

	int numFreq, numDir, numBundle;

};

#endif

#include <algorithm>

/*
class Filter{



};
*/

class Butterworth{

public:
	Butterworth(int ordLow, int ordHigh, double fLow, double fHigh);
	double transferFunction(double freq);

private:
	int _ordLow, _ordHigh;
	double _fLow, _fHigh;

};

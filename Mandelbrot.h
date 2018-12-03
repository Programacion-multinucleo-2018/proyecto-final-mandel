#ifndef MANDELBROT_H_
#define MANDELBROT_H_

#include <complex>

using namespace std;

namespace mandelbrot {

class Mandelbrot {
public:
	static const int MAX_ITERATIONS = 1000000;

public:
	Mandelbrot();
	virtual ~Mandelbrot();

	static int getIterations(double x, double y);
	int iterations();
};

Mandelbrot::Mandelbrot() {
	// TODO Auto-generated constructor stub

}

Mandelbrot::~Mandelbrot() {
	// TODO Auto-generated destructor stub
}

int Mandelbrot::getIterations(double x, double y) {

	complex<double> z = 0;
	complex<double> c(x, y);

	int iterations = 0;

	while(iterations < MAX_ITERATIONS) {
		z = z*z + c;

		if(abs(z) > 2) {
			break;
		}

		iterations++;
	}

	return iterations;
}

int Mandelbrot::iterations(){
	return MAX_ITERATIONS;
}

} /* namespace mandelbrot */

#endif /* MANDELBROT_H_ */

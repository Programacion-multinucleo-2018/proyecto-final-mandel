#ifndef FRACTALCREATOR_H_
#define FRACTALCREATOR_H_

#include <string>
#include <cstdint>
#include <memory>
#include <fstream>
#include <math.h>
#include <vector>
#include <iostream>
#include <assert.h>
#include <omp.h>
#include "Zoom.h"
#include "RGB.h"
#include "mandelbrotKernel.h"

using namespace std;

namespace mandelbrot {

class FractalCreator {
private:
	int m_width;
	int m_height;
	//unique_ptr<int[]> m_histogram;
	//unique_ptr<int[]> m_fractal;
	//int * m_histogram;
	//int * m_fractal;
	Zoom m_Zoom;
	int m_total{0};

	vector<int> m_ranges;
	vector<RGB> m_colors;
	vector<int> m_rangeTotals;

	bool m_bGotFirstRange{false};
	void calculateIteration();
	void calculateIterationOMP();
	void calculateIterationCUDA();
	void calculateTotalIterations();
	void calculateTotalIterationsOMP();
	void calculateRangeTotals();
	void calculateRangeTotalsOMP();
	int getRange(int iterations) const;

public:
	int * m_histogram;
	int * m_fractal;

	FractalCreator(int width, int height);
	virtual ~FractalCreator();
	void addColorRange(double rangeEnd, const RGB& rgb);
	void addZoom(int x, int y, double scale);
	void run();
	void drawFractal();

};

void FractalCreator::run() {

	//printf("Runtime.\n");
	//float cpuTime = 0.0;
	//auto start =  chrono::high_resolution_clock::now();
	//calculateIteration();
	//calculateIterationOMP();
	//auto end =  chrono::high_resolution_clock::now();
	//chrono::duration<float, std::milli> duration_ms = end - start;
	//cpuTime = duration_ms.count();
	//printf("Runtime: %f\n", cpuTime);

	calculateIterationCUDA();
	printf("IT CALC\n" );
	calculateTotalIterations();
	printf("IT CALC2\n" );
	calculateRangeTotalsOMP();
	printf("IT CALC3\n" );
}

FractalCreator::FractalCreator(int width, int height) :
		m_width(width), m_height(height), m_Zoom(
				m_width, m_height) {

	int fractalBytes = M_WIDTH * M_HEIGHT * sizeof(int);
	int histogramBytes = Mandelbrot::MAX_ITERATIONS * sizeof(int);

	m_fractal = (int *)malloc(fractalBytes);
	m_histogram = (int *)malloc(histogramBytes);

	memset(m_fractal, 0, fractalBytes);
	memset(m_histogram, 0, histogramBytes);

	m_Zoom.add(m_width / 2, m_height / 2, 4.0 / m_width);
}

FractalCreator::~FractalCreator() {
	free(m_fractal);
	free(m_histogram);
}

void FractalCreator::calculateIteration() {
	for (int y = 0; y < m_height; y++) {
		for (int x = 0; x < m_width; x++) {
			pair<double, double> coords = m_Zoom.doZoom(x, y);

			int iterations = Mandelbrot::getIterations(coords.first,
					coords.second);

			m_fractal[y * m_width + x] = iterations;

			if (iterations != Mandelbrot::MAX_ITERATIONS) {
				m_histogram[iterations]++;
			}

		}
	}
}

void FractalCreator::calculateIterationOMP() {

	int y;

	#pragma omp parallel for private(y) shared(m_fractal,m_histogram)
	for (y = 0; y < m_height; y++) {
		for (int x = 0; x < m_width; x++) {
			pair<double, double> coords = m_Zoom.doZoom(x, y);

			int iterations = Mandelbrot::getIterations(coords.first,
					coords.second);

			m_fractal[y * m_width + x] = iterations;

			if (iterations != Mandelbrot::MAX_ITERATIONS) {
				#pragma omp atomic
				m_histogram[iterations]++;
			}

		}
	}
}



void FractalCreator::calculateIterationCUDA() {
	runCuda(m_fractal, m_histogram, m_Zoom.m_scale, m_Zoom.m_xCenter, m_Zoom.m_yCenter);
}

void FractalCreator::calculateRangeTotals(){
	int rangeIndex = 0;

	for (int i = 0; i < Mandelbrot::MAX_ITERATIONS; i++) {
		int pixels = m_histogram[i];

		if (i >= m_ranges[rangeIndex + 1])
			rangeIndex++;

		m_rangeTotals[rangeIndex] += pixels;
	}
}

void FractalCreator::calculateRangeTotalsOMP(){
	cout << "Entra" << endl;
	int rangeIndex = 0;
	int i = 0;

	#pragma omp parallel for private(i,rangeIndex) shared(m_ranges,m_rangeTotals, m_histogram)
	for (i = 0; i < Mandelbrot::MAX_ITERATIONS; i++) {
		int pixels = m_histogram[i];

		if (i >= m_ranges[rangeIndex + 1])
			rangeIndex++;

		m_rangeTotals[rangeIndex] += pixels;
	}
}

void FractalCreator::calculateTotalIterations() {
	for (int i = 0; i < Mandelbrot::MAX_ITERATIONS; i++)
		m_total += m_histogram[i];
}

void FractalCreator::drawFractal() {

	for (int y = 0; y < m_height; y++) {
		for (int x = 0; x < m_width; x++) {

			int iterations = m_fractal[y * m_width + x];

			int range = getRange(iterations);
			int rangeTotal = m_rangeTotals[range];
			int rangeStart = m_ranges[range];

			RGB& startColor = m_colors[range];
			RGB& endColor = m_colors[range + 1];
			RGB colorDiff = endColor - startColor;

			float red = 0;
			float green = 0;
			float blue = 0;

			if (iterations != Mandelbrot::MAX_ITERATIONS) {

				//double hue = 0.0;
				int totalPixels = 0;

				for (int i = rangeStart; i <= iterations; i++)
					totalPixels += m_histogram[i];

				red = (float)(startColor.r + colorDiff.r * (double)totalPixels/rangeTotal)/255;
				green = (float)(startColor.g + colorDiff.g * (double)totalPixels/rangeTotal)/255;
				blue = (float)(startColor.b + colorDiff.b * (double)totalPixels/rangeTotal)/255;
				//printf("%f %f %f\n",red/255, green/255, blue/255 );
			}
			plot(x,y,red,green,blue);
		}
	}
	cout << "FP" << endl;
}

void FractalCreator::addColorRange(double rangeEnd, const RGB& rgb){
	m_ranges.push_back(rangeEnd * Mandelbrot::MAX_ITERATIONS);
	m_colors.push_back(rgb);

	if (m_bGotFirstRange) {
		m_rangeTotals.push_back(0);
	}

	m_bGotFirstRange = true;
}

int FractalCreator::getRange(int iterations) const {
	int range = 0;

	for (int i = 1; i < m_ranges.size(); i++) {
		range = i;
		if (m_ranges[i] > iterations)
			break;
	}

	range --;
	assert(range > -1);
	assert(range < m_ranges.size());

	return range;
}


void FractalCreator::addZoom(int x, int y, double scale) {
	m_Zoom.add(x,y,scale);
}

} /* namespace mandelbrot */

#endif /* FRACTALCREATOR_H_ */

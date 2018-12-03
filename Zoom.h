#ifndef ZOOM_H_
#define ZOOM_H_

#include <utility>

using namespace std;

namespace mandelbrot {

class Zoom {
private:
	int m_width{0};
	int m_height{0};

public:
	double m_xCenter{0};
	double m_yCenter{0};
	double m_scale{1.0};
	Zoom(int width, int height);
	void add(int x, int y, double scale);
	pair<double, double> doZoom(int x, int y);
};

Zoom::Zoom(int width, int height) :
		m_width(width), m_height(height) {
}

void Zoom::add(int x, int y, double scale) {
	m_xCenter += (x - m_width / 2) * m_scale;
	m_yCenter += -(y - m_height / 2) * m_scale;
	m_scale *= scale;
}

pair<double, double> Zoom::doZoom(int x, int y) {
	double xFractal = (x - m_width / 2) * m_scale + m_xCenter;
	double yFractal = (y - m_height / 2) * m_scale + m_yCenter;
	return pair<double, double>(xFractal, yFractal);
}

} /* namespace mandelbrot */

#endif /* ZOOM_H_ */

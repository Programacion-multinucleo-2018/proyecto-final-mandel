#ifndef RGB_H_
#define RGB_H_

namespace mandelbrot{

struct RGB{
  double r;
  double g;
  double b;
  RGB(double r, double g, double b);
};


RGB operator-(const RGB& first, const RGB& second);

RGB::RGB(double r, double g, double b): r(r), g(g), b(b) {
}

RGB operator-(const RGB& first, const RGB& second) {
	return RGB(first.r - second.r, first.g - second.g, first.b - second.b);
}
}

#endif /* RGB_H_ */

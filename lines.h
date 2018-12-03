#include "helper_includes.h"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <math.h>

#define M_WIDTH 1000
#define M_HEIGHT 800

using namespace std;

#ifndef Lines_h
#define Lines_h

void plot(int x, int y, double r, double g, double b){
    glColor3f(r, g, b);
    glPointSize(1.0);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

#endif /* Lines_h */

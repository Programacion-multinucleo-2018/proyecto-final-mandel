#include <iostream>
#include "FractalCreator.h"
#include <math.h>
#include <complex>

using namespace mandelbrot;

FractalCreator fractalCreator(M_WIDTH, M_HEIGHT);


using namespace std;

void inicializar(void){
    glClearColor (0.0, 0.0, 0.0, 0.0);
    //glShadeModel (GL_FLAT);
}


void display_mandelbrot(){
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glPointSize(1.0);
		fractalCreator.drawFractal();
    glutSwapBuffers();
}

void scene(int w, int h){
    glViewport (0, 0, (GLsizei) M_WIDTH, (GLsizei) M_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
		gluOrtho2D(0, M_WIDTH, M_HEIGHT, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard_mandelbrot(unsigned char key, int x_, int y_){
    switch (key) {
        case 'q':
            exit(0);
            break;

        default:
            break;
    }
}

void mouseClick(int button, int state, int x, int y){

  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      fractalCreator.addZoom(x, M_HEIGHT - y, 0.1);
      fractalCreator.run();
      glutPostRedisplay();
    }
  }

}


int main(int argc, char** argv) {


	//FractalCreator fractalCreator(M_WIDTH, M_HEIGHT);

	fractalCreator.addColorRange(0.0, RGB(0,0,0));
  fractalCreator.addColorRange(0.2, RGB(255,0,0));
  fractalCreator.addColorRange(0.4, RGB(255,0,255));
  fractalCreator.addColorRange(0.6, RGB(0,255,255));
  fractalCreator.addColorRange(0.8, RGB(255,255,0));
  fractalCreator.addColorRange(1.0, RGB(255,255,255));


	//fractalCreator.addZoom(313, 330, 0.1);
  //fractalCreator.addZoom(825, 130, 0.1);
  fractalCreator.run();
  //fractalCreator.drawFractal();

/**/
  int max = 0;
  int maxi_c = 0;
  for (int i = 0; i < M_WIDTH * M_HEIGHT; i++) {
    if (fractalCreator.m_fractal[i] > max) {
      max = fractalCreator.m_fractal[i];
      maxi_c = 0;
    } else if (fractalCreator.m_fractal[i] == max){
      maxi_c++;
    }
  }
  int tot = 0;
  int max_histo = 0;
  int max_histo_rep = 0;
  for (int i = 0; i < Mandelbrot::MAX_ITERATIONS + 1; i++) {
    if (fractalCreator.m_histogram[i] > max_histo) {
      max_histo = fractalCreator.m_histogram[i];
      max_histo_rep = 1;
    } else if (fractalCreator.m_histogram[i] == max){
      max_histo_rep++;
    }
    tot += fractalCreator.m_histogram[i];
  }

  printf("maximo %d %d, max %d\n", max,fractalCreator.m_histogram[max], maxi_c);
  printf("Total: %d    Mas rep: %d    rep max: %d\n",tot, max_histo, max_histo_rep );
/**/



	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(M_WIDTH, M_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("MANDELBROT SET");
	inicializar();

	glutKeyboardFunc(keyboard_mandelbrot);
  glutMouseFunc(mouseClick);
	glutDisplayFunc(display_mandelbrot);
	glutReshapeFunc(scene);
	glutMainLoop();

	printf("Finished.\n");
	return 0;
}

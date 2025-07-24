//Window class -- handles window size, shape
//Hurrah, hurrah.

#include "window.h"
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

window::window()
{
  winh = 300;
  winw = 400;
  started = false;
  stopped = false;
}

void window::init(int width, int height, char* title)
{
  winh = height;  winw = width;
  glutInitWindowSize(winw, winh);
  glutInitWindowPosition(0, 0);
  glutCreateWindow(title);
  glViewport(0, 0, GLsizei(winw), GLsizei(winh));  //set viewport

  //this function draws on Dr. Mount's "sample3D.cpp"
}

void window::setWindowsize(int width, int height)
{
  winh = height;  winw = width;
  glViewport(0, 0, winw, winh);
  //I hope to god this is self-explanatory.
  glutPostRedisplay();
}


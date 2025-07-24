//Shannon Woods
//Camera .cpp file
//mental note:  this should be fleshed out later with functions to increment
//and decrement the angle and distance

#include "camera.h"
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

camera::camera ()
{
  zangle = atan2(25,30);
  angle = 0;
  distance = 30;
  height = 0;
}

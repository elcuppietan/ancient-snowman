//Shannon Woods
//Class definition for environent class (controls ground and lights)

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

struct obstacle {
  float x1;
  float y1; //bottom-left of bottom coords
  float x2; 
  float y2; //top-right of bottom coords
  float height; //height of prism
} ;

class environment {
  public:
    environment();
    void init();
    void drawGround();
    void updateLights();
    obstacle blocks[4];
    void fogOn() { fogg = true; }
    void fogOff() { fogg = false; }
    bool fogState() { return fogg; }
  private:
    bool fogg;
};

#endif

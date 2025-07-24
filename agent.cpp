//Shannon Woods
//Here is the class source for the snowman agent
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <iostream>

#include "agent.h"

#define PI 3.14159

using namespace std;

agent::agent()
{
 init();
}

void agent::init()
{
  speed = 0;
  headingx = 0;
  headingy = 0;
  posx = 0;  posy = 0;
  facing = 0;
  jumping = false;
}

void agent::drawAgent(float cameraangle)
{
  float relfacing;
  relfacing = (cameraangle) + facing;
  //relfacing is the angle of facing with respect to the camera rather than
  //the coordinate set.
  //Note:  snowman is 13.25 units tall
  GLfloat snowWhite[] = {.8, .8, .8, 1};
  GLfloat carrotOr[] = {.7, .5, .5, 1};
  GLfloat coalBlack[] = {0, 0, 0, 1};
  glMaterialfv(GL_FRONT_AND_BACK,
	       GL_AMBIENT_AND_DIFFUSE,
	       snowWhite);
  glPushMatrix();
    //drawing snowman
  glTranslatef(posx, posy, posz);
  glTranslatef(0, 0, 3);
  glutSolidSphere(4, 20, 20);
  glTranslatef(0,0,4.5);
  glutSolidSphere(3, 20, 20);
  glTranslatef(0, 0, 3.75);
  glutSolidSphere(2, 20, 20);
  glPushMatrix();
    //drawing left eye
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, coalBlack);
  glRotatef(-(relfacing * (180 / PI)), 0, 0, 1); // rotate to where facing

  glTranslatef(2 * sin(25*PI/180), -2 * sin(55*PI/180), 2 * cos(55*PI/180));
  glutSolidSphere(.3, 10, 10);
  glPopMatrix();
  glPushMatrix();
    //drawing right eye
  glRotatef(-(relfacing * (180 / PI)), 0, 0, 1); // rotate to where facing
  glTranslatef(-2 * sin(25*PI/180), -2 * sin(55*PI/180), 2 * cos(55*PI/180));
  glutSolidSphere(.3, 10, 10);
  glPopMatrix();
    //drawing carrot nose
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, carrotOr);
  glRotatef(-(relfacing * (180 / PI)), 0, 0, 1);
  glRotatef(90, 1, 0, 0);
  glutSolidCone(.5, 4, 20, 20);
  glPopMatrix();
}

void agent::changeSpeed(int newspeed)
{
  speed = newspeed;
}

void agent::moveAgent()
{
;
}

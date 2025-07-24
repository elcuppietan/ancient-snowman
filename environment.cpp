//Shannon Woods
//Environment .cpp file

#include "environment.h"
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

environment::environment()
{
//should probably call init but do I really want to call it twice?
}
void environment::init()
{
  GLfloat lightAmb[] = {0.15, 0.15, 0.15, 1};
  GLfloat lightDiff[] = {.5, .5, .5, 1};
  glEnable(GL_LIGHTING);
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);
  glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmb);
  glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiff);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT2);
  //turn on two lights (tried four, it's too many)
  fogg = false;
  //init obstacles
  obstacle temp;
  temp.x1 = -50;  temp.y1 = -40;
  temp.x2 = -35;  temp.y2 = -25;
  temp.height = 7;
  blocks[0] = temp;
  temp.x1 = 80;  temp.y1 = 10;
  temp.x2 = 95;  temp.y2 = 25;
  temp.height = 3;
  blocks[1] = temp;
  temp.x1 = -50;  temp.y1 = 50;
  temp.x2 = -35;  temp.y2 = 65;
  temp.height = 9;
  blocks[2] = temp;
  temp.x1 = 70;  temp.y1 = -70;
  temp.x2 = 85;  temp.y2 = -55;
  temp.height = 5;
  blocks[3] = temp;
}
void environment::drawGround()
{
  GLfloat floorWhite[] = {1, 1, 1, 1}; 
  GLfloat jelloRed[] = {.9, .1, .1, .75}; 

  //this renders each square as a separate surface, which is really stupid
  //and should be changed to texture map in the next project.  And that's why
  //the floor is so small:  it slows the damn thing down too much
/*  for(int i = -100; i < 100; i = i + 3)
  {
    for(int j = -100; j < 100; j = j + 3)
    {
      if((i + j) % 2 == 0)
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, floorWhite);
      else
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, floorBlack);

      //draw floor?
      glBegin(GL_QUADS);
        glNormal3f(0,0,1);  glVertex3f(i, j, 0);
        glNormal3f(0,0,1);  glVertex3f(i + 3, j, 0);
        glNormal3f(0,0,1);  glVertex3f(i + 3, j + 3, 0);
        glNormal3f(0,0,1);  glVertex3f(i, j + 3, 0);
      glEnd();
    }
  } */
  glEnable(GL_TEXTURE_2D);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, floorWhite);
  glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);  glTexCoord2f(0, 0);  glVertex3f(-100, -100, 0);
    glNormal3f(0, 0, 1);  glTexCoord2f(40, 0); glVertex3f(100, -100, 0);
    glNormal3f(0, 0, 1);  glTexCoord2f(40, 40); glVertex3f(100, 100, 0);
    glNormal3f(0, 0, 1);  glTexCoord2f(0, 40); glVertex3f(-100, 100, 0);
  glEnd();
  glDisable(GL_TEXTURE_2D);

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, jelloRed);

  //now draw rectangular prism obstacles
  for(int m = 0; m < 4; m++)
  {
    glBegin(GL_QUAD_STRIP);
      glNormal3f(0, -1, 0);  glVertex3f(blocks[m].x1, blocks[m].y1, 
					blocks[m].height);
      glNormal3f(0, -1, 0);  glVertex3f(blocks[m].x1, blocks[m].y1, 
					0);
      glNormal3f(0, -1, 0);  glVertex3f(blocks[m].x2, blocks[m].y1, 
					blocks[m].height);
      glNormal3f(0, -1, 0);  glVertex3f(blocks[m].x2, blocks[m].y1, 0);
      glNormal3f(1, 0, 0);   glVertex3f(blocks[m].x2, blocks[m].y2, 
					blocks[m].height);
      glNormal3f(1, 0, 0);   glVertex3f(blocks[m].x2, blocks[m].y2, 0);
      glNormal3f(0, 1, 0);   glVertex3f(blocks[m].x1, blocks[m].y2, 
					blocks[m].height);
      glNormal3f(0, 1, 0);   glVertex3f(blocks[m].x1, blocks[m].y2, 
					0);
      glNormal3f(-1, 0, 0);  glVertex3f(blocks[m].x1, blocks[m].y1, 
					blocks[m].height);
      glNormal3f(-1, 0, 0);  glVertex3f(blocks[m].x1, blocks[m].y1,
					0);
    glEnd();
    glBegin(GL_QUADS);
      glNormal3f(0, 0, 1);   glVertex3f(blocks[m].x1, blocks[m].y1,
					blocks[m].height);
      glNormal3f(0, 0, 1);   glVertex3f(blocks[m].x2, blocks[m].y1,
					blocks[m].height);
      glNormal3f(0, 0, 1);   glVertex3f(blocks[m].x2, blocks[m].y2,
					blocks[m].height);
      glNormal3f(0, 0, 1);   glVertex3f(blocks[m].x1, blocks[m].y2,
					blocks[m].height);
    glEnd();
  }
}

void environment::updateLights()
{
  GLfloat light0Pos[] = {1, -1, 1, 0};
  GLfloat light2Pos[] = {-1, -1, 1, 0};
  glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
  glLightfv(GL_LIGHT2, GL_POSITION, light2Pos);
}

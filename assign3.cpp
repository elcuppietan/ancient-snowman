//Shannon Woods
//Assignment 2, CMSC427 Spring 2003
//"Agent Tracker"
//<Blabber>
//this program renders a snowman and makes the snowman follow the mouse pointer
//around a small floor of green checks.  The camera remains centered on the
//snowman, and may be rotated around the snowman, zoomed in, and zoomed out.
//Rev 2 of this program implements a number of things:  uneven flooring made
//of red jello, a cool floor that's supposed to be stone but which looks a lot
//like rough snowplain, jumping, collision detection (done by means of a
//bounding box around the bottom sphere of the snowman), fog, single-step
//animation mode, and other cool stuff.
//</Blabber>

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include "camera.h"
#include "environment.h"
#include "window.h"
#include "agent.h"
#include "RGBpixmap.h"
#include <math.h>

#define PI 3.14159

using namespace std;

//shannon's vector format for easy processing
struct svector {
  float vectx;
  float vecty;
};

void shanKey(unsigned char, int, int);
void shanMouse(int, int, int, int);
void shanDraw(void);
void shanReshape(int, int);
void shanInit(void);
void shanMotion(int, int);
void shanSpecialKey(int, int, int);
void shanTimer(int);
svector angletovector(float);
float vectortoangle(svector);

//happy happy globals!
camera shanCam;
environment shanEnv;
agent snowman;
window shanWin;
RGBpixmap floormap; 

bool singlest;

//callbacks, main loop, initialization
int main(int argc, char ** argv)
{
  singlest = false;
  glutInit(&argc, argv); //initialize glut & gl
  shanInit();

  glutDisplayFunc(shanDraw);
  glutReshapeFunc(shanReshape);
  //glutMouseFunc(shanMouse);
  glutPassiveMotionFunc(shanMotion);
  glutKeyboardFunc(shanKey);
  glutTimerFunc(50, shanTimer, 0);
  glutSpecialFunc(shanSpecialKey);

  glutMainLoop();  //remit control to glut
  return 0;

}

//shanInit: initializes all the ugly sturf
void shanInit()
{
  //this function draws heavily on Dr. Mount's "sample3D.cpp"
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  shanWin.init(400, 300, "Assignment 3 -- Agent Tracker");
  glClearColor (0, 0, 0, 1); //grey bgcolor

  glEnable(GL_NORMALIZE); //Autonormalize
  glEnable(GL_DEPTH_TEST); //hidden surface removal
//  glShadeModel(GL_FLAT); //smooth shading (to turn off, use GL_FLAT)
  glShadeModel(GL_SMOOTH); //smooth shading (to turn off, use GL_FLAT)
  glEnable(GL_TEXTURE_2D); //enable 2d texmapping

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

  if(!floormap.readBMPFile("sp02.bmp", true, true)) {
	cerr << "File sp02.bmp cannot be read or illegal format" << endl;
	exit(1);
  }
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGB,
    floormap.nCols,
    floormap.nRows,
    0,
    GL_RGB,
    GL_UNSIGNED_BYTE,
    floormap.pixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //the above taken from Dr. Mount's readme

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  shanEnv.init();
  snowman.init();

  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity();
  gluPerspective(
    45, //fovy
    1.3333, //aspect ratio
    1, //near clipping plane
    1000); //far clipping plane
  //the above is taken directly from "sample3D.cpp". I hope it works.
  glMatrixMode (GL_MODELVIEW);
  glutPostRedisplay(); 
  
}

//shanKey:  handles keyboard input interrupts/callbacks
void shanKey(unsigned char key, int x, int y)
{
  //cout << "in shankey" << endl;
  if(shanWin.started == false)
  {
    shanWin.started = true;
  }
  if(shanWin.stopped == true)
    exit(0);
  else
  {
    switch(key) {
      case 'q':
  	  shanWin.stopped = true;
	  glDisable(GL_FOG);
	  glDisable(GL_BLEND);
	  glutPostRedisplay();
	  break;  //allow user to quit the program
      case ' ':
          if(!snowman.jumpTest())
	  {
	    snowman.setVel(3);
	    snowman.jumpOn();
	  }
	  break;  //jumping
      case 'f':
	  if (!shanEnv.fogState())
	  {
	    shanEnv.fogOn();
	    glEnable(GL_FOG);
	    glFogi(GL_FOG_MODE, GL_LINEAR);
	    glFogf(GL_FOG_DENSITY, 0);
	    glFogi(GL_FOG_START, 20);
	    glFogi(GL_FOG_END, 200);
            GLfloat foggy[] = {0.7, 0.7, 1.0, .5};
            glFogfv(GL_FOG_COLOR, foggy);
	  }
	  else
	  {
	    shanEnv.fogOff();
	    glDisable(GL_FOG);
	  }
	  break;
      case 's':
	  if(!singlest)
	    singlest = true;
          else
	    shanTimer(5);
	  break;
      case 'r':
	  if(singlest)
	    singlest = false;
	  shanTimer(5);
	  break;
    }
  }
}

//handles arrow key presses
void shanSpecialKey(int key, int x, int y)
{
  if(shanWin.started == true && shanWin.stopped == false)
  {
    switch(key)
    {
      //when the camera rotates, we have to recalculate the direction the
	//snowman faces.
      case GLUT_KEY_LEFT:
        shanCam.setAngle(shanCam.getAngle() + PI / 36); //move five degrees left
        if(shanCam.getAngle() > 2 * PI)
          shanCam.setAngle(shanCam.getAngle() - 2 * PI);
	snowman.setFacing(snowman.getFacing() - PI / 36); //face 5deg right
        if(snowman.getFacing() < 0)
          snowman.setFacing(2 * PI + snowman.getFacing());
        break;
      case GLUT_KEY_RIGHT:
        shanCam.setAngle(shanCam.getAngle() - PI / 36); //move 5 degrees right
        if(shanCam.getAngle() < 0)
          shanCam.setAngle(2 * PI + shanCam.getAngle());
        snowman.setFacing(snowman.getFacing() + PI / 36);//face 5deg left
	if(snowman.getFacing() > 2 * PI)
	  snowman.setFacing(snowman.getFacing() - 2 * PI);
        break;
      case GLUT_KEY_DOWN:
        if(shanCam.getDistance() <= 50)
          shanCam.setDistance(shanCam.getDistance() + 5);
        break;
      case GLUT_KEY_UP:
        if(shanCam.getDistance() >= 35)
  	  shanCam.setDistance(shanCam.getDistance() - 5);
        break;
    }
    glutPostRedisplay();
  }
}

//shanMouse:  handles mouse *click* input interrupts/callbacks
void shanMouse(int button, int state, int x, int y)
{
 ;
}

void shanDraw()
{
  svector camvector;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  camvector = angletovector(shanCam.getAngle());
  //this next line essentially says "look at the snowman from the camera" :)
  gluLookAt(snowman.getX() + (camvector.vectx * shanCam.getDistance()),
	snowman.getY() + (camvector.vecty * shanCam.getDistance()), 
	(shanCam.getDistance() * tan(shanCam.getZangle())) + 
		shanCam.getHeight(),
	snowman.getX(), snowman.getY(), shanCam.getHeight(), 0, 0, 1);
  shanEnv.updateLights();
  shanEnv.drawGround();
  if(shanWin.started == false)
  {
    //happy little introduction screen
    GLfloat blackLet[] = {1, 0, 0, 1};
    glColor4f(1, 0, 0, 1);
    glClear (GL_COLOR_BUFFER_BIT);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blackLet);
    glRasterPos2f(30, -22);
    char string[] =  "Controls:                            ";
    char string2[] = "    Up         Zoom Camera In        ";
    char string3[] = "Left  Right    Rotate Camera         ";
    char string4[] = "   Down        Zoom Camera Out       ";
    char string5[] = "  Mouse        Move Mr. Snowman      ";
    char string6[] = "    Q          Quit                  ";
    char string7[] = "Space Bar      Jump                  ";
    char string8[] = "   S           Enter singlestep mode ";
    char string9[] = "                or advance one step  ";
    char string10[] = "    R          Resume animated mode  ";
    char string11[] = "    F          Toggle fog            ";
    char string12[] = "      Press any key to continue      ";
    for(int i = 0; i < 36; i++)
      glutBitmapCharacter (GLUT_BITMAP_8_BY_13, string[i]);
    glRasterPos2f(20, -19);
    for(int i = 0; i < 36; i++)
      glutBitmapCharacter (GLUT_BITMAP_8_BY_13, string2[i]);
    glRasterPos2f(18, -14);
    for(int i = 0; i < 36; i++)
      glutBitmapCharacter (GLUT_BITMAP_8_BY_13, string3[i]);
    glRasterPos2f(17, -9);
    for(int i = 0; i < 36; i++)
      glutBitmapCharacter (GLUT_BITMAP_8_BY_13, string4[i]);
    glRasterPos2f(16, -5);
    for(int i = 0; i < 37; i++)
      glutBitmapCharacter (GLUT_BITMAP_8_BY_13, string5[i]);
    glRasterPos2f(15, -2);
    for(int i = 0; i < 36; i++)
      glutBitmapCharacter (GLUT_BITMAP_8_BY_13, string6[i]);
    glRasterPos2f(14, 1);
    for(int i = 0; i < 36; i++)
      glutBitmapCharacter (GLUT_BITMAP_8_BY_13, string7[i]);
    glRasterPos2f(13, 4);
    for(int i = 0; i < 36; i++)
      glutBitmapCharacter (GLUT_BITMAP_8_BY_13, string8[i]);
    glRasterPos2f(12, 6);
    for(int i = 0; i < 36; i++)
      glutBitmapCharacter (GLUT_BITMAP_8_BY_13, string9[i]);
    glRasterPos2f(12, 8);
    for(int i = 0; i < 36; i++)
      glutBitmapCharacter (GLUT_BITMAP_8_BY_13, string10[i]);
    glRasterPos2f(12, 10);
    for(int i = 0; i < 36; i++)
      glutBitmapCharacter (GLUT_BITMAP_8_BY_13, string11[i]);
    glRasterPos2f(12, 12);
    for(int i = 0; i < 36; i++)
      glutBitmapCharacter (GLUT_BITMAP_8_BY_13, string12[i]);
    glFlush();
    glutSwapBuffers();
  }
  else if (shanWin.stopped == true)
  {
    glDisable(GL_BLEND);
    GLfloat blackLet[] = {1, 0, 0, 1};
    glColor4f(1, 0, 0, 1);
    glClear (GL_COLOR_BUFFER_BIT);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blackLet);
    glRasterPos2f(25 + snowman.getX(), snowman.getY() - 20);
    char stringS[] =  "         Press any key to end         ";
    for(int i = 0; i < 36; i++)
      glutBitmapCharacter (GLUT_BITMAP_8_BY_13, stringS[i]);
    glFlush();
    glutSwapBuffers();
  }
  else
  {
    snowman.drawAgent(shanCam.getAngle());
    glutSwapBuffers();
  }
  //cout << "About to leave shandraw" << endl;
}

//like it says, convert a float angle to a svector
svector angletovector (float angle)
{
  float angle2;
  svector to_ret;
  //take angle in radians, convert to vector
  if(angle  < PI / 2)
  {
    angle2 = angle;
    to_ret.vectx = sin(angle2);
    to_ret.vecty = cos(angle2);
  }
  else if (angle < PI)
  {
    angle2 = angle - (PI / 2);
    to_ret.vectx = cos(angle2);
    to_ret.vecty = -sin(angle2);
  }
  else if (angle < 3 * PI / 2)
  {
    angle2 = angle - PI;
    to_ret.vectx = -sin(angle2);
    to_ret.vecty = -cos(angle2);
  }
  else
  {
    angle2 = angle - (3 * PI / 2);
    to_ret.vectx = -cos(angle2);
    to_ret.vecty = sin(angle2);
  }
  return to_ret;
}

//convert an svector to float angle
//angle with respect to positive y axis
float vectortoangle(svector v)
{
  float retangle;
  if(v.vectx == 0)
  {
    if(v.vecty >= 0)
      retangle = 0;
    else
      retangle = PI;
  }
  else if(v.vecty == 0)
  {
    if(v.vectx > 0)
      retangle = PI / 2;
    else
      retangle = 3 * PI / 2;
  }
  else if(v.vecty > 0)
  {
    if(v.vectx > 0)
      retangle = atan2(v.vectx, v.vecty);
    else
      retangle = (2 * PI) - atan2(-v.vectx, v.vecty);
  }
  else
  {
    if(v.vectx > 0)
      retangle = (PI / 2) + atan2(-v.vecty, v.vectx);
    else
      retangle = PI + atan2(-v.vectx, -v.vecty);
  }
  return retangle;
}

//resets window size and viewport size and coordinate set on window reshape
void shanReshape(int width, int height)
{
  shanWin.setWindowsize(width, height);
  glViewport(0, 0, width, height);
  glutPostRedisplay();
}

//handles passive mouse motion
void shanMotion(int x, int y)
{
  if((shanWin.started == true && !snowman.jumpTest()) && 
	shanWin.stopped == false)
  {
    //cout << "X: " << x << " Y: " << y << endl;
    float centx, centy, xsq, ysq, length;
    centx = shanWin.getWidth() / 2;
    centy = shanWin.getHeight() / 2;
    xsq = (centx - x) * (centx - x);
    ysq = (centy - y) * (centy - y);
    //find center coords, distance to x, y
    length = sqrt(xsq + ysq);
    if(length <= 50)
      snowman.changeSpeed(0);
    else if (length <= 130)
      snowman.changeSpeed(1);
    else
      snowman.changeSpeed(2);
    //stopped, walking, running (do snowment run?)
    snowman.setHeadx(x - centx);
    snowman.setHeady(centy - y);
    //set new heading ^^
    //cout << "Mouse pointing:  " << x << ", " << y << endl;
    //cout << "Snowman heading: " << snowman.getHeadx() << ", ";
    //cout << snowman.getHeady() << endl;
  }
}

//timer event -- continually animate
void shanTimer(int id)
{
  if(shanWin.started == true && shanWin.stopped == false)
  {
    float baseangle, mouseangle, agentangle;
    baseangle = shanCam.getAngle() - PI;
    svector mousey;
    svector headingv;
    float headangle;
    //this all uses the math mount demonstrated in class
    headingv.vectx = snowman.getHeadx();
    headingv.vecty = snowman.getHeady();
    headangle = vectortoangle(headingv);
    mousey.vectx = snowman.getHeadx();
    mousey.vecty = snowman.getHeady();
    mouseangle = vectortoangle(mousey);
    agentangle = (mouseangle + baseangle);
    //if angle more than 2pi, fix so it's between 0 and 2pi
    if(agentangle >= 2 * PI)
      agentangle = agentangle - 2 * PI;
    svector agentv;
    agentv = angletovector(agentangle);
    //move snowman by amount proportional to speed
    float newxs = snowman.getX() + (agentv.vectx * snowman.getSpeed());
    float newys = snowman.getY() + (agentv.vecty * snowman.getSpeed());
    float newzs = snowman.getZ();
    if(snowman.jumpTest())
    {
      newzs = snowman.getZ() + snowman.getVel();
    }
//detect whether we're about to wander off the floor
    if(newxs - 4 < -100)
    {
      snowman.setX(-100 + 4);
      snowman.changeSpeed(0);
    }
    else if(newxs + 4 > 100)
    {
      snowman.setX(100 - 4);
      snowman.changeSpeed(0);
    }
    if(newys - 4 < -100)
    {
      snowman.setY(-100 + 4);
      snowman.changeSpeed(0);
    }
    else if(newys + 4 > 100)
    {
      snowman.setY(100 - 4);
      snowman.changeSpeed(0);
    }

//detect collision with blocks
    bool collis = false; float xc = 0;  float yc = 0; float zc = 0;
    bool overBlock = false;
    for(int i = 0; i < 4; i++)
    {
      if(
	 (
	 ((newxs - 4 < shanEnv.blocks[i].x2 && 
           newxs - 4 > shanEnv.blocks[i].x1) &&
	 (newys - 4 < shanEnv.blocks[i].y2 && 
	  newys - 4 > shanEnv.blocks[i].y1)) ||
	 ((newxs + 4 < shanEnv.blocks[i].x2 &&
	   newxs + 4 > shanEnv.blocks[i].x1) &&
	 (newys - 4 < shanEnv.blocks[i].y2 &&
	  newys - 4 > shanEnv.blocks[i].y1))
	 ) || 
	 ((
	 ((newxs + 4 < shanEnv.blocks[i].x2 &&
	  newxs + 4 > shanEnv.blocks[i].x1) &&
	 (newys + 4 < shanEnv.blocks[i].y2 &&
	  newys + 4 > shanEnv.blocks[i].y1)) || 
	 ((newxs - 4 < shanEnv.blocks[i].x2 &&
	  newxs - 4 > shanEnv.blocks[i].x1) &&
	 (newys + 4 < shanEnv.blocks[i].y2 &&
	  newys + 4 > shanEnv.blocks[i].y1))
	 ) && !collis)
	)
	{
	  overBlock = true;
          if(newzs < shanEnv.blocks[i].height)
	  {
	    collis = true;
	    if(snowman.getX() < shanEnv.blocks[i].x1)
	      xc = shanEnv.blocks[i].x1;
	    if(snowman.getX() > shanEnv.blocks[i].x2)
	      xc = shanEnv.blocks[i].x2;
	    if(snowman.getY() < shanEnv.blocks[i].y1)
	      yc = shanEnv.blocks[i].y1;
	    if(snowman.getY() > shanEnv.blocks[i].y2)
	      yc = shanEnv.blocks[i].y2;
	    if(snowman.getZ() >= shanEnv.blocks[i].height)
	      zc = shanEnv.blocks[i].height;
	    //cout << "Collis -- xc: " << xc << " yc: " << yc << endl;
	  }
        }
    }
    if(collis)
    {
      if(xc != 0)
      {
	if(xc > snowman.getX())
	  snowman.setX(xc - 4);
	if(xc < snowman.getX())
	  snowman.setX(xc + 4);
      }
      if(yc != 0)
      {
	if(yc > snowman.getY())
	  snowman.setY(yc - 4);
	if(yc < snowman.getY())
	  snowman.setY(yc + 4);
      }
      if(zc != 0)
      {
	snowman.setZ(zc);
	snowman.jumpOff();
        snowman.setVel(0);
        shanCam.setHeight(zc);
      }
      if (zc == 0)
      {
        //cout << "Collision with SIDE" << endl;
	if(snowman.jumpTest())
	{
	  snowman.setZ(snowman.getZ() + snowman.getVel());
	  if(snowman.getZ() < 0)
          {
	    snowman.setZ(0);
	    snowman.setVel(0);
	    snowman.jumpOff();
	    shanCam.setHeight(0);
	  }
	  else
	   snowman.setVel(snowman.getVel() - .5);
	}
      }
	snowman.changeSpeed(0);
    }    
    if(!collis)
    {
	bool flag = false;
	for(int i = 0; i < 4; i++)
       {
       if(
         (
         ((snowman.getX() - 4 < shanEnv.blocks[i].x2 &&
           snowman.getX() - 4 > shanEnv.blocks[i].x1) &&
         (snowman.getY() - 4 < shanEnv.blocks[i].y2 &&
          snowman.getY() - 4 > shanEnv.blocks[i].y1)) ||
         ((snowman.getX() + 4 < shanEnv.blocks[i].x2 &&
           snowman.getX() + 4 > shanEnv.blocks[i].x1) &&
         (snowman.getY() - 4 < shanEnv.blocks[i].y2 &&
          snowman.getY() - 4 > shanEnv.blocks[i].y1))
         ) ||
         ((
         ((snowman.getX() - 4 < shanEnv.blocks[i].x2 &&
          snowman.getX() - 4 > shanEnv.blocks[i].x1) &&
         (snowman.getY() + 4 < shanEnv.blocks[i].y2 &&
          snowman.getY() + 4 > shanEnv.blocks[i].y1)) ||
         ((snowman.getX() + 4 < shanEnv.blocks[i].x2 &&
          snowman.getX() + 4 > shanEnv.blocks[i].x1) &&
         (snowman.getY() + 4 < shanEnv.blocks[i].y2 &&
          snowman.getY() + 4 > shanEnv.blocks[i].y1))
         ) && (!flag && !snowman.jumpTest()))
	 )
        {
	  if((newxs - 4 > shanEnv.blocks[i].x2  || 
		newxs + 4 < shanEnv.blocks[i].x1)
	   || (newys - 4 > shanEnv.blocks[i].y2 || 
		newys  + 4 < shanEnv.blocks[i].y1))
	  {
	    //if snowman has just stepped off block
	    snowman.jumpOn();
	    snowman.setVel(-.5);
	    flag = true;
	  }
	}
      }
    }
    if((newxs - 4 >= -100 && newxs + 4 <= 100) && 
		((newys - 4 >= -100 && newys + 4 <= 100) && !collis))
    {
      snowman.setX(snowman.getX() + (agentv.vectx * snowman.getSpeed()));
      snowman.setY(snowman.getY() + (agentv.vecty * snowman.getSpeed()));

      if(snowman.jumpTest())
      {
	if(snowman.getZ() + snowman.getVel() < 0)
	{
	  snowman.setZ(0);
	  snowman.jumpOff();
	  snowman.setVel(0);
	  shanCam.setHeight(0);
	}
	else
	{
	  snowman.setZ(snowman.getZ() + snowman.getVel());
	  snowman.setVel(snowman.getVel() - .5);
	}
      }
    }
    float sfacing = snowman.getFacing();
    //if the snowman not facing toward the mouse pointer
    if(snowman.getFacing() != headangle)
    {
      bool dright = false;
      //if need turn more than 5 degrees to face the pointer
      if((sfacing > headangle && fabs(sfacing - headangle) > fabs(PI/36)) ||
        (sfacing < headangle && fabs(headangle - sfacing) > fabs(PI / 36)))
      {
        //this crap is all to decide which way is shorter to turn toward mouse
        if(fabs(headangle) > fabs(sfacing))
        {
          if(fabs(headangle - sfacing) > fabs(PI))
            dright = false;
          else
            dright = true;
        }
        else
        {
          if(fabs(sfacing - headangle) > fabs(PI))
	    dright = true;
	  else
	    dright = false;
        }
        if(dright)
          snowman.setFacing(snowman.getFacing() + PI / 36);
        else
	  snowman.setFacing(snowman.getFacing() - PI / 36);
        if(snowman.getFacing() < 0)
          snowman.setFacing(2 * PI + snowman.getFacing());
        if(snowman.getFacing() > fabs(2 * PI))
          snowman.setFacing(snowman.getFacing() - 2 * PI);
      }
      else
        snowman.setFacing(headangle);
    } 
    else
    {
      //cout << "Matched facing: " << snowman.getFacing() << endl;
      //cout << "Heading: " << snowman.getHeadx() << ", " << snowman.getHeady();
      //cout << endl;
    }
    glutPostRedisplay();
  }
  if(!singlest)
  {
    //cout << "Registering timer" << endl;
    glutTimerFunc(50, shanTimer, 0);
  }
}

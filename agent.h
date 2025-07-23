//Shannon Woods
//header file with class definition for my snowman agent
#ifndef AGENT_H
#define AGENT_H

class agent {
  private:
    float posx;
    float posy;
    float posz;
    float headingx;
    float headingy;
    float facing;
    int speed;
    float vertvel;  //vertical velocity
    bool jumping;
  public:
    agent();
    void init();
    void drawAgent(float);
    void changeSpeed(int);
    void moveAgent();
    float getX() { return posx; }
    float getY() { return posy; }
    float getZ() { return posz; }
    void setX(float newx) { posx = newx; }
    void setY(float newy) { posy = newy; }
    void setZ(float newz) { posz = newz; }
    int getSpeed() {return speed;}
    float getHeadx() { return headingx; }
    float getHeady() { return headingy; }
    void setHeadx(float newx) { headingx = newx; }
    void setHeady(float newy) { headingy = newy; }
    void setFacing(float newface) { facing = newface; }
    float getFacing() { return facing; }
    void setVel(float newvel) { vertvel = newvel; }
    float getVel() { return vertvel; }
    void jumpOn() { jumping = true; }
    void jumpOff() { jumping = false; }
    bool jumpTest() { return jumping; }
};

#endif

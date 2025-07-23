//Shannon Woods
//Class definition for camera class (that controls... surprise, the camera)
#ifndef CAMERA_H
#define CAMERA_H

class camera {
  public:
    camera ();
    void setAngle (float ang) { angle = ang; }
    float getAngle () { return angle; }
    void setDistance (float dist) { distance = dist; }
    float getDistance () { return distance; }
    float getZangle () { return zangle; }
    float getHeight () { return height; }
    void setHeight (float newh) { height = newh; }
  private:
    float zangle;
    float angle;
    float distance;
    float height;
};

#endif

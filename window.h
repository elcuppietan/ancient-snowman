//Shannon Woods
//Window class definition

#ifndef WINDOW_H
#define WINDOW_H

class window {
  public:
    window();
    void init(int width, int height, char* title);
    void setWindowsize(int width, int height);
    float getWidth() { return winw; }
    float getHeight() { return winh; }
    bool started;
    bool stopped;
  private:
    int winh; //height of window
    int winw; //width of window
};

#endif

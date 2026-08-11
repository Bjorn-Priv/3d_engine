#ifndef HWINDOWITEM
#define HWINDOWITEM

#include "inputhandler.h"

class WindowItem {
  private: 

  protected:
    int ID;
    //0 = left, 1 = right, 2 = top, 3 = bottom
    std::vector<int> affected[4]{};

    bool initialised;

    int x,y = 0;
    int height = 0;
    int width = 0;

  public:
    WindowItem() = delete;
    WindowItem(int nID) : ID(nID){};

    virtual void update() = 0;
    virtual void render() = 0;

    void updateSize(double widthChange, double heightChange);
    void setSize(int x, int y, int width, int height);

    //getters 
    int getID(){return ID;};
    int getX(){return x;};
    int getY(){return y;};
    int getWidth(){return width;};
    int getHeight(){return height;};

    virtual ~WindowItem(){};
};

template <class T>
class Item3D : public WindowItem {
  using WindowItem::WindowItem;

  private: 
    InputHandler<T> handler;
    GLclampf background[4] = {0.5f, 0.1f, 0.15f, 1.0f};
    
  public: 
    void setBG(GLclampf BG[4]);
    void update();
    void render();
    ~Item3D();
};  

template <class T>
Item3D<T>::~Item3D() {
}

template <class T>
void Item3D<T>::update() {
}

template <class T>
void Item3D<T>::setBG(GLclampf BG[4]) {
  for (int i = 0; i < 4; i++) background[i] = BG[i];
}

template <class T>
void Item3D<T>::render() {
  glViewport(getX(), getY(), getWidth(), getHeight());

  glEnable(GL_SCISSOR_TEST);
  glScissor(getX(), getY(), getWidth(), getHeight());

  glClearColor(
    background[0],
    background[1],
    background[2],
    background[3]
  );

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDisable(GL_SCISSOR_TEST);
}

#endif
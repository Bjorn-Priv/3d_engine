#ifndef HWINDOWITEM
#define HWINDOWITEM

#include "inputhandler.h"

class WindowItem {
  friend class App;
  private: 
    bool initialised;
    bool openGL = true;

    void setSize(int x, int y, int width, int height);
    void changeWidth(int delta) {width += delta;};
    void changeHeight(int delta) {height += delta;}; 

    /*
      Wrapper functions around user defined update and render functions
      Should NEVER be called by users themselves as they are used in the App class
      They are used to set certain OpenGL variables to the specific windows
    */
    void AppUpdate(), AppRender();

    int leftoverWidth() {return width - minWidth;}
    int leftoverHeight() {return height - minHeight;};


    /*
      User defined update and render functions 
      Users have to implement these for their derived classes
    */
    virtual void update() = 0, render() = 0;

  protected:
    //definition: if this guy grows or shifts then all windowItems in this array need to shift their position 

    int x,y = 0;
    int height = 0;
    int width = 0;
    int minHeight = 0;
    int minWidth = 0;
    int ID;

  public:
    WindowItem() = delete;
    WindowItem(int nID) : initialised(true), ID(nID){};

    void setMinSize(int nwMW, int nwMH) {minWidth = nwMW; minHeight = nwMH;};

    virtual ~WindowItem(){};

    /* --------------------------------------------------------
      Getter functions
    */
    int getID(){return ID;}; 
    int getX(){return x;};
    int getY(){return y;};
    int getWidth(){return width;};
    int getHeight(){return height;};
};

template <class T>
class Item3D : public WindowItem {
  using WindowItem::WindowItem;

  private: 
    InputHandler<T> handler;
    GLclampf background[4] = {0.5f, 0.1f, 0.15f, 1.0f};

  public: 
    void update() override;
    void render() override;
    void setBG(GLclampf BG[4]);
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
  glClearColor(
    background[0],
    background[1],
    background[2],
    background[3]
  );
}

#endif
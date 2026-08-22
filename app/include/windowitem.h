#ifndef HWINDOWITEM
#define HWINDOWITEM

#include "inputhandler.h"

class WindowItem {
  friend class App;
  private: 
    bool initialised;
    bool openGL = true;

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

    int* getRight(){return right;};
    int* getLeft(){return left;};
    int* getTop(){return top;};
    int* getBottom(){return bottom;};

    void setRight(int *i){right = i;};
    void setLeft(int *i){left = i;};
    void setTop(int *i){top = i;};
    void setBottom(int *i){bottom = i;};

    bool updateSize() {
      int oldW = *right;
      int oldH = *top;

      *right = *left + width;
      *top = *bottom + height;

      return (*right != oldW) || (*top != oldH);
    };

    void initSize() {
      width = *right - *left;
      height = *top - *bottom;
    }

    /*
      User defined update and render functions 
      Users have to implement these for their derived classes
    */
    virtual void update() = 0, render() = 0;

  protected:
    int *left, *top, *right, *bottom;
    int width = 0, height = 0;
    int minHeight = 50;
    int minWidth = 50;
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
    int getX(){return *left;};
    int getY(){return *bottom;};
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
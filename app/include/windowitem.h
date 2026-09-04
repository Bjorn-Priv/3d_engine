#ifndef HWINDOWITEM
#define HWINDOWITEM

#include "inputhandler.h"

class WindowItem {
  friend class App;
  private: 
    bool initialised;

    /*
      Wrapper functions around user defined update and render functions
      Should NEVER be called by users themselves as they are used in the App class
      They are used to set certain OpenGL variables to the specific windows
    */
    void AppUpdate(), AppRender();

    /*
      Function to change the width of this window item

      Parameters: 
        delta : amount to change the width (can be negative)
    */
    void changeWidth(int delta) {width += delta;};

    /*
      Function to change the height of this window item

      Parameters: 
        delta : amount to change the height (can be negative)
    */
    void changeHeight(int delta) {height += delta;}; 

    /*
      Calculates how much width remains between current width and minimum width for window item

      Returns amount of width leftover until limit
    */
    int leftoverWidth() {return width - minWidth;}

    /*
      Calculates how much height remains between current height and minimum height for window item

      Returns amount of height leftover until limit
    */
    int leftoverHeight() {return height - minHeight;};

    void handleClickDown(SDL_Event event);
    void handleClickUp(SDL_Event event);
    void handleMouseMove(SDL_Event event);
    void handleInput(SDL_Event event);

    /*
      Getters for edge pointers
    */

    int* getRight(){return right;};
    int* getLeft(){return left;};
    int* getTop(){return top;};
    int* getBottom(){return bottom;};

    /*
      Getter for distance to border to resize item
    */
    int getBDist(){return borderDist;};

    /*
      Setters for edge pointers 
    */

    void setRight(int *i){right = i;};
    void setLeft(int *i){left = i;};
    void setTop(int *i){top = i;};
    void setBottom(int *i){bottom = i;};

    /*
      Function used to update right and top edges based upon changed width and height

      Returns : 
        true if either the top or right value changed 
        false otherwise
    */
    bool updateSize() {
      int oldW = *right;
      int oldH = *top;

      *right = *left + width;
      *top = *bottom + height;

      return (*right != oldW) || (*top != oldH);
    }

    /*
      Function to be called upon assignment of all edge pointers (sets width and height)
    */
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
    int borderDist = 3;
    int *left, *top, *right, *bottom;
    int width = 0, height = 0;
    int minHeight = 100;
    int minWidth = 100;
    int ID;
    bool openGL = true;

  public:
    //No default constructor
    WindowItem() = delete;

    /*
      Actual default constructor used by the application as it passes the ID
    */
    WindowItem(int nID) : initialised(true), ID(nID){};

    /*
      Setter for minimum window item size
    */
    void setMinSize(int nwMW, int nwMH) {minWidth = nwMW; minHeight = nwMH;};

    /*
      Setter for distance from edge to resize the item size
    */
    void setBorderDist(int nwDist) {borderDist = nwDist;};

    /*
      Default destructor
    */
    virtual ~WindowItem(){};

    /*
      Getter functions
    */

    int getID(){return ID;}; 
    int getX(){return *left;};
    int getY(){return *bottom;};
    int getWidth(){return width;};
    int getHeight(){return height;};
};


/*
  EXAMPLE DERIVED CLASS
*/

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
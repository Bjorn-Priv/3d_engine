#ifndef HAPP
#define HAPP

#include "windowitem.h"

/*
  Struct to hold window properties:
    Name (const char*)
    Width (int)
    Height (int)
    MinWidth (int)
    MinHeight (int)
    Flags (SDL_WindowFlags) ; see SDL documentation
*/
struct S_WindProp {
  const char* name = "default";
  int width = 600;
  int height = 600;
  int MinWidth = 500;
  int MinHeight = 500;
  SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;
};

class App {
  private:
    bool initialised;

    SDL_Window* window;
    S_WindProp properties;
    SDL_GLContext context;
    std::vector<WindowItem*> items;

  public:
    App() = delete;

    /*
      Default constructor 
      Parameters: 
        flags: SDL initialisation flags (see SDL Documentation)
        GLversion: Version of OpenGL to use ; example: 3.3
        properties: window properties struct for all window values
    */
    App(SDL_InitFlags flags, double GLversion, S_WindProp properties);

    /*
      Creates a new window item of class T 
        Create your own windowItem derived class to pass through it
      
      if creating an item for the first time, parameters do not matter and can be arbitrary values

      Parameters: 
        ID : ID of windowItem that has to be split
        horizontal : boolean to indicate wether to split the window with {ID} 
          horizontally or vertically, true = horizontally ; false = vertically
      returns: Pointer to the window item
    */
    template <typename T>
    T *createWindowItem(int ID, bool horizontal);

    void update();
    void handleEvent(SDL_Event event);
    void render();

    ~App();
};

template <typename T>
T *App::createWindowItem(int ID, bool hori) {
  if (!initialised || !(std::is_base_of<WindowItem, T>::value)) return nullptr;

  T *i = new T((int)items.size());
  WindowItem *temp = reinterpret_cast<WindowItem *>(i);

  if (items.size() == 0) {
    temp->setSize(0, 0, properties.width, properties.height);
    items.push_back(temp);
    return i;
  }

  WindowItem *o = items[ID];
  int X = o->getX();
  int Y = o->getY();
  int WIDTH = o->getWidth();
  int HEIGHT = o->getHeight();

  if (hori) {
    int height1 = (HEIGHT / 2) + (HEIGHT % 2);
    int height2 = (HEIGHT / 2);
    o->setSize(X, Y+height2, WIDTH, height1);
    temp->setSize(X, Y, WIDTH, height2);

    //copy affected from certain axis and direction

  } else {
    int width1 = (WIDTH / 2) + (WIDTH % 2);
    int width2 = (WIDTH / 2);
    o->setSize(X, Y, width1, HEIGHT);
    temp->setSize(X+width1, Y, width2, HEIGHT);

    //copy affected from certain axis and direction
  }

  items.push_back(temp);

  for (size_t i = 0; i < items.size(); i++) {
    SDL_Log("i: %d, X : %d, Y : %d, width : %d, height %d", (int)i, items[i]->getX(), items[i]->getY(), items[i]->getWidth(), items[i]->getHeight());
  }
  SDL_Log("__________________________");

  return i;
} //createWindowItem

#endif

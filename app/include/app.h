#ifndef HAPP
#define HAPP

#include "windowitem.h"
#include <list>

/*
  Function that searches for an item in a vector of arbitrary types

  Parameters: 
    item : item that is looked for
    vector : vector that is searched in

  Returns : 
    true if item in vector
    false otherwise
  
  Disclaimer: assumes "==" is defined for type T
*/
template <typename T> 
bool ItemInVector(T item, std::vector<T> &vector) {
  for (T i : vector) if (i == item) return true;
  return false;
}

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
  class WindowItemOrder;
  private:
    bool initialised;

    SDL_Window* window;
    S_WindProp properties;
    SDL_GLContext context;
    std::vector<WindowItem*> items;
    WindowItemOrder *windowOrder;

    std::vector<int> coordArena;

    void resizeWindow(int width, int height);
    /* 
      Helper functions for resizeWindow
      Seperates logic for Width, Height; Growth and Shrinkage 
      Parameter : How much the window needs to change 
      Returns : How much the window was changed (negative for shrinking)
    */
    int shrinkWindowWidth(int deltaX), shrinkWindowHeight(int deltaY), 
        growWindowWidth(int deltaX), growWindowHeight(int deltaY);

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
        Item : ID of windowItem that has to be split
        Horizontal : boolean to indicate wether to split the window with {ID} 
          horizontally or vertically, true = horizontally ; false = vertically
      returns: Pointer to the window item
    */
    template <typename T>
    T *createWindowItem(WindowItem *Item, bool Horizontal);

    void run();

    void update();
    void handleEvent(SDL_Event event);
    void render();

    ~App();
};

/*
  Helper Class for App to store the order of windowItems when resizing
   Contains functions that make it easier to manage the resizing of the window
*/
class App::WindowItemOrder {
  struct WindowItemGraph;
  private: 
    std::vector<WindowItemGraph> ItemsInOrderH{};
    std::vector<WindowItemGraph> ItemsInOrderV{};

    bool horizontal = true;
  public: 
    /*
      Pushes new windowItem into the order vectors and graphs
      Depending on current Horizontal value pushes to either horizontal or vertical graph
      Parameters: 
        prevItem : WindowItem that was just split in half
        item : new Item that needs to be pushed into vectors
    */
    void push(WindowItem *prevItem, WindowItem *item);

    /*
      Setter for Horizontal boolean, if set to true every other function will perform on
      horizontal graphs, if false all other functions will perform on vertical graphs
      Parameters: 
        boolean : value to set horizontal to 
    */
    void setHorizontal(bool boolean) {horizontal = boolean;}

    /* -----------------------------------------------------
      getters
    */
    int getNGraphs() {return horizontal ? ItemsInOrderH.size() : ItemsInOrderV.size();};

    int shrinkWidth(int delta, bool rest);
    int growWidth(int delta, bool rest);
    int shrinkHeight(int delta, bool rest);
    int growHeight(int delta, bool rest);

    // /*
    //   Indexing operator, returns either horizontal or vertical depending on Horizontal boolean
    // */
    // WindowItemGraph &operator[](int idx) {return horizontal ? ItemsInOrderH[idx] : ItemsInOrderV[idx];}
};

/*
  Helper Struct for WindowItemOrder, stores a singular graph and its operations
*/
struct App::WindowItemOrder::WindowItemGraph {
  std::vector<WindowItem*> graph = {};

  //default constructor
  WindowItemGraph(){};

  /*
    Custom constructor for a new graph
    copies the vector {copy} and pushes all its items into a new graph 
    except for the item with id {ignore}, then afterwards pushes item with id {add}
    into the graph
    Parameters: 
      ignore : item to be ignored (so not copied)
      copy : vector to copy 
      add : item to be pushed onto graph after copying
  */
  WindowItemGraph(WindowItem *ignore, std::vector<WindowItem*> *copy, WindowItem *add);

  /*
    Member function for graph
    Parameters: 
      item : WindowItem id that is looked for in the graph
    Returns:
      true if item is in the graph
      false otherwise
  */
  bool contains(WindowItem *item) {
    if (graph.empty()) return false;
    for (WindowItem *i : graph) if (item == i) return true;
    return false;
  }

  int maxShrinkageHeight() {
    int min = INT_MAX;
    for (WindowItem *item : graph) 
      if (min > item->leftoverHeight()) min = item->leftoverHeight();
    return min;
  }

  int maxShrinkageWidth() {
    int min = INT_MAX;
    for (WindowItem *item : graph) 
      if (min > item->leftoverWidth()) min = item->leftoverWidth();
    return min;
  }

  void shrinkWidth(int delta) {for (WindowItem *item : graph) item->changeWidth(-delta);}
  void growWidth(int delta) {for (WindowItem *item : graph) item->changeWidth(delta);}
  void shrinkHeight(int delta) {for (WindowItem *item : graph) item->changeHeight(-delta);}
  void growHeight(int delta) {for (WindowItem *item : graph) item->changeHeight(delta);}

  /*
    Pushes item onto graph
    Parameters: 
      item : item to be pushed
  */
  void add(WindowItem *item) {graph.push_back(item);}

  /*
    Gives graph data in the form of a vector<int> *
  */
  std::vector<WindowItem*> *data() {return &graph;}
};

template <typename T>
T *App::createWindowItem(WindowItem *item, bool hori) {
  if (!initialised || !(std::is_base_of<WindowItem, T>::value)) return nullptr;
  
  T *i = new T((int)items.size());
  WindowItem *temp = reinterpret_cast<WindowItem *>(i);

  if (items.size() == 0) {
    SDL_Log("Red with: %d, %d", properties.width, properties.height);
    temp->setSize(0, 0, properties.width, properties.height);
    items.push_back(temp);
    windowOrder->push(nullptr, temp);
    return i;
  }

  if (item == nullptr || item->getID() >= (int)items.size() || item != items[item->getID()]) return nullptr;

  int WIDTH = item->getWidth();
  int HEIGHT = item->getHeight();

  SDL_Log("Width : %d, Hieght : %d", WIDTH, HEIGHT);

  if (hori) {
    int height1 = (HEIGHT / 2) + (HEIGHT % 2);
    int height2 = (HEIGHT / 2);
    temp->setSize(item->getX(), item->getY(), WIDTH, height2);
    item->setSize(item->getX(), item->getY()+height2, WIDTH, height1);

    //copy affected from certain axis and direction

  } else {
    int width1 = (WIDTH / 2) + (WIDTH % 2);
    int width2 = (WIDTH / 2);
    temp->setSize(item->getX()+width1, item->getY(), width2, HEIGHT);
    item->setSize(item->getX(), item->getY(), width1, HEIGHT);

    //copy affected from certain axis and direction
  }

  windowOrder->setHorizontal(hori);
  windowOrder->push(item, temp);

  items.push_back(temp);

  for (size_t i = 0; i < items.size(); i++) {
    SDL_Log("i: %d, X : %d, Y : %d, width : %d, height %d", (int)i, items[i]->getX(), items[i]->getY(), items[i]->getWidth(), items[i]->getHeight());
  }
  SDL_Log("__________________________");

  return i;
} //createWindowItem

#endif

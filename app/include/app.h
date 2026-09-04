#ifndef HAPP
#define HAPP

#include "windowitem.h"
#include <queue>

/*
  Function that searches for an item in a vector of arbitrary types

  Parameters: 
    item : item that is looked for
    vector : vector that is searched in

  Returns : 
    true if item in vector
    false otherwise
  
  Conditions: assumes "==" is defined for type T
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
    bool openGL;

    SDL_Window* window;
    S_WindProp properties;
    SDL_GLContext context;

    std::vector<WindowItem*> items;
    WindowItemOrder *windowOrder;

    WindowItem *active;

    SDL_Cursor *cursor;
    int prevType = 0;
    int *MouseX = nullptr, *MouseY = nullptr;

    std::deque<int> coordArena;

    /*
      Helper function that pushes an integer into the arena 
      and returns a pointer to it

      Parameters:
        integer : the integer to push into the arena

      Returns : pointer to integer within arena
    */
    int *pushArena(int integer) {
      coordArena.push_back(integer);
      return &coordArena.back();
    }

    /*
      Helper function that handles mouse click

      Parameter: 
        event : SDL event data 
    */
    void handleClickDown(SDL_Event event);

    /*
      Helper function that handles mouse click release

      Parameter: 
        event : SDL event data 
    */
    void handleClickUp(SDL_Event event);

    /*
      Helper function that handles mouse movement

      Parameter: 
        event : SDL event data 
    */
    void handleMouseMove(SDL_Event event);

    /*
      Helper function that handles mouse movement 
      when borders are being held by the mouse

      should only be called if borders are being dragged

      Parameter: 
        event : SDL event data 
    */
    void moveBorders(SDL_Event event);

    /*
      Helper function that checks if there is a vertical border at x, y
      in the window, and returns the border

      Parameters: 
        x : x coordinate within window
        y : y coordinate within window

      Returns:
        pointer to the vertical border
    */
    int *verticalBorderAt(int x, int y);

    /*
      Helper function that checks if there is a horizontal border at x, y
      in the window, and returns the border

      Parameters: 
        x : x coordinate within window
        y : y coordinate within window

      Returns:
        pointer to the horizontal border
    */
    int *horizontalBorderAt(int x, int y);

    void setActive(SDL_Event event);

    /*
      Helper function that handles the cursor that is shown 
      based on the sort of subwindow border it is on

      Parameters: 
        hori : horizontal border pointer (nullptr if no border)
        vert : vertical border pointer (nullptr if no border)
    */
    void handleBorderCursor(int* hori, int* vert);

    /*
      Function that creates a new cursor object and deletes the old one

      Parameter: 
        type : SDL_SystemCursor enum type of new cursor
    */
    void setCursor(SDL_SystemCursor type);

    /*
      Helper function for resizing window action

      Parameters: 
        event : SDL_Event data
    */
    void resizeWindow(SDL_Event event);

    /* 
      Helper functions for resizeWindow
      Seperates logic for Width, Height; Growth and Shrinkage 

      Parameters:
        delta : How much the window needs to change size
        width : boolean to indicate in which dimension the window changes
          --true -> width is changed
          --false -> height is changed

      Returns : How much the window was changed
    */
    int shrinkWindow(int delta, bool width), growWindow(int delta, bool width);


    /*
      Function that updates all starting positions for every item
      should be called after having shrunk or grown any windows
    */
    void updateItems();

    /*
      Helper function that calls the update function for every window item
    */
    void update();

    /*
      Helper function that calls the render function for every window item
    */
    void render();

    /*
      Helper function that is called when an action occurs

      Parameters:
        event : SDL_Event that contains information about the action
    */
    void handleEvent(SDL_Event event);

  public:
    //no default constructor
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
        item : ID of windowItem that has to be split
        horizontal : boolean to indicate how to split subwindow {item}
          --true -> horizontally
          --false -> vertically

      returns: Pointer to the window item
    */
    template <typename T>
    T *createWindowItem(WindowItem *item, bool horizontal);

    /* 
      Main function to run the app
    */
    void run();

    ~App();
};

/*
  Helper Class for App to store the order of windowItems when resizing
  Contains functions that make it easier to manage the resizing of the window
*/
class App::WindowItemOrder {
  struct WindowItemGraph;
  private: 
    std::vector<WindowItemGraph> itemsInOrderH{};
    std::vector<WindowItemGraph> itemsInOrderV{};

    std::vector<WindowItemGraph>* order = &itemsInOrderH;
    std::vector<WindowItemGraph>* orderOpp = &itemsInOrderV;

    //Function to switch mode of class to horizontal 
    void setToHorizontal() {order = &itemsInOrderH; orderOpp = &itemsInOrderV;};
    //Function to switch mode of class to vertical 
    void setToVertical() {order = &itemsInOrderV; orderOpp = &itemsInOrderH;};

    /*
      Finds Largest or Smallest graph 
      (A graph that can either shrink the most or grow the most)

      Parameters: 
        bWidth : boolean to indicate which dimension to search in
          --true -> width
          --false -> height
        *out : pointer to an integer that will contain amount of growth or shrinkage
      
      Returns:
        Pointer to the graph that is either largest or smallest
    */
    WindowItemGraph *findLargestGraph(bool bWidth, int *out),
                    *findSmallestGraph(bool bWidth, int *out);

  public: 
    //default constructor
    WindowItemOrder(){}

    /*
      Pushes new windowItem into the order vectors and graphs

      Parameters: 
        prevItem : WindowItem that was just split in half
        item : new Item that needs to be pushed into vectors
        width : boolean value to indicate if there was a horizontal split or vertical split
          -- true -> horizontal split
          -- false -> vertical split
    */
    void push(WindowItem *prevItem, WindowItem *item, bool width);


    /* 
      Getter for amount of graphs in a direction 
      
      Parameters : 
        width : boolean to indicate Horizontal graphs or Vertical graphs

      Returns :
        amount of graphs in specified direction
    */
    int getNGraphs(bool width) {return width ? itemsInOrderH.size() : itemsInOrderV.size();};

    /*
      Function that shrinks/grows every WindowItem graph correctly and according to limits

      Parameters :
        delta : amount that every graph needs to shrink/grow
        rest : extra amount that needs to be shrunk/grown but can not be divided equally
        width : boolean to indicicate which dimension to shrink/grow
          -- true -> width
          -- false -> height
      
      Returns : 
        total amount that was shrunk or grown
    */
    int shrink(int delta, int rest, bool width), 
        grow(int delta, int rest, bool width);
};

/*
  Helper Struct for WindowItemOrder, stores a singular graph and its operations
*/
struct App::WindowItemOrder::WindowItemGraph {
  std::vector<WindowItem*> graph = {};

  //default constructor
  WindowItemGraph(){};

  /*
    Default constructor when wanting to initialise the graph with an item
    
    Parameters: 
      item : item that is pushed into the graph 
  */
  WindowItemGraph(WindowItem* item){add(item);};

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
  WindowItemGraph(WindowItem *ignore, std::vector<WindowItem*> &copy, WindowItem *add);

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
  } //contains

  /*
    Function that returns how much the graph can still shrink in either height or width

    Parameters : 
      width : boolean to indicate wether to return width or height value
        -- true -> calculates for width
        -- false -> calculates for height
    
    Returns : 
      The max amount the wanted dimension can still shrink
  */
  int maxShrink(bool width) {
    int min = INT_MAX;
    for (WindowItem *item : graph) {
      if (width && min > item->leftoverWidth()) min = item->leftoverWidth();
      else if (!width && min > item->leftoverHeight()) min = item->leftoverHeight();
    }
    return min;
  } //maxShrink

  /* 
    Grows a window item by delta (can be negative to shrink)

    Parameters :
      delta : amount to change the window size with
      width : boolean to indicate wether or not it is a width or height change
        -- true -> width is changed
        -- false -> height is changed
  */
  void grow(int delta, bool width) {
    for (WindowItem *item : graph) {
      if (width)
        item->changeWidth(delta);
      else 
        item->changeHeight(delta);
    }
  } //grow

  /*
    Pushes item onto graph

    Parameters: 
      item : item to be pushed
  */
  void add(WindowItem *item) {graph.push_back(item);}

  /*
    Gives graph data in the form of a vector<int> *
  */
  std::vector<WindowItem*> &data() {return graph;}
};

template <typename T>
T *App::createWindowItem(WindowItem *item, bool hori) {
  static_assert(std::is_base_of<WindowItem, T>::value, "Type T needs to be derived from WindowItem");
  if (!initialised) return nullptr;

  T *i = new T((int)items.size());
  WindowItem *temp = reinterpret_cast<WindowItem *>(i);

  if (items.size() == 0) {
    items.push_back(temp);
    windowOrder->push(nullptr, temp, hori);
    temp->setLeft(pushArena(0));
    temp->setBottom(pushArena(0));
    temp->setRight(pushArena(properties.width));
    temp->setTop(pushArena(properties.height));
    temp->initSize();
    active = temp;
    return i;
  }

  if (item == nullptr || item->getID() >= (int)items.size() || item != items[item->getID()]) return nullptr;

  int WIDTH = item->getWidth();
  int HEIGHT = item->getHeight();

  if (hori) {
    int height = (HEIGHT / 2);
    temp->setLeft(item->getLeft());
    temp->setRight(item->getRight());
    temp->setBottom(item->getBottom());

    temp->setTop(pushArena(height));
    item->setBottom(temp->getTop());

  } else {
    int width = (WIDTH / 2) + (WIDTH % 2);
    temp->setTop(item->getTop());
    temp->setBottom(item->getBottom());
    temp->setRight(item->getRight());

    temp->setLeft(pushArena(width));
    item->setRight(temp->getLeft());
  }

  temp->initSize();
  item->initSize();
  windowOrder->push(item, temp, hori);

  items.push_back(temp);
  return i;
} //createWindowItem

#endif

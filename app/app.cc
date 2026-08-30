#include "include/app.h"

#include <chrono>

bool intLimDel(int o, int lim, int delta) {
  return o >= lim-delta && o <= lim+delta;
}

App::App(SDL_InitFlags f1, double GL, S_WindProp p) : initialised(true), openGL(false), window(nullptr), properties(p), windowOrder(new WindowItemOrder()) {
  initialised = SDL_Init(f1);

  if (p.width < p.MinWidth) properties.width = p.MinWidth;
  if (p.height < p.MinHeight) properties.height = p.MinHeight;

  SDL_SetWindowMinimumSize(window, p.MinWidth, p.MinHeight);
  window = SDL_CreateWindow(p.name, properties.width, properties.height, p.flags);
  
  if (GL == 0.0) return;

  openGL = true;
  int maj = (int)GL;
  int min = (int)((GL*10.0) - maj*10);

  initialised = initialised && SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, maj) &&
                               SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, min) &&
                               SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  context = SDL_GL_CreateContext(window);
  glewExperimental = GL_TRUE;
  initialised = glewInit() == GLEW_OK;
  glGetError();
  glEnable(GL_DEPTH_TEST);

  if (!initialised) SDL_Log("%s", SDL_GetError());
} //constructor

void App::run() {
  bool running = true;
  SDL_Event event;

  while (running) {
    while (running && SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) running = false;
      handleEvent(event);
    } 

    update();
    render();
  }
} //run

void App::update() {
  if (!initialised) return;

  for (WindowItem* item : items) item->AppUpdate();
} //update

void App::render() {
  if (!initialised) return;

  if (openGL) {
    glViewport(0, 0, properties.width, properties.height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  for (WindowItem* item : items) item->AppRender();

  if (openGL)
    SDL_GL_SwapWindow(window);
} //render

void App::handleEvent(SDL_Event e) {
  if (!initialised) return;
  if (e.type == SDL_EVENT_WINDOW_RESIZED)  //window is resized
    resizeWindow(e.window.data1, e.window.data2);
  if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) 
    handleClickDown(e);
  if (e.type == SDL_EVENT_MOUSE_BUTTON_UP)
    handleClickUp(e);
  if (e.type == SDL_EVENT_MOUSE_MOTION) 
    handleMouseMove(e);

} //handleEvent

void App::handleClickDown(SDL_Event e) {
  mouseDown = true;
  MouseX = verticalBorderAt(e.button.x, properties.height-(int)e.button.y);
  MouseY = horizontalBorderAt(e.button.x, properties.height-(int)e.button.y);
  if (MouseX != nullptr || MouseY != nullptr) {
    SDL_Log("ON BORDER!!!!!");}
}

void App::handleClickUp(SDL_Event e) {
  mouseDown = false;
  MouseX = nullptr;
  MouseY = nullptr;
}

void App::handleMouseMove(SDL_Event e) {
  int *vert = verticalBorderAt(e.button.x, properties.height-(int)e.button.y);
  int *hori = horizontalBorderAt(e.button.x, properties.height-(int)e.button.y);

  int type = 0;
  if (vert) type += 1;
  if (hori) type += 2;

  if (type != prevType) {
    SDL_SystemCursor cursorID;
    if (type == 0) cursorID = SDL_SYSTEM_CURSOR_DEFAULT;
    if (type == 1) cursorID = SDL_SYSTEM_CURSOR_EW_RESIZE;
    if (type == 2) cursorID = SDL_SYSTEM_CURSOR_NS_RESIZE;
    if (type == 3) cursorID = SDL_SYSTEM_CURSOR_MOVE;

    SDL_DestroyCursor(cursor);
    cursor = SDL_CreateSystemCursor(cursorID);
    SDL_SetCursor(cursor);
    prevType = type;
  }

  if (MouseX == nullptr && MouseY == nullptr) return;
  if (MouseX != nullptr) *MouseX = e.motion.x;
  if (MouseY != nullptr) *MouseY = (properties.height-(int)e.motion.y);
  for (WindowItem *item : items) 
    item->initSize();
}

int *App::verticalBorderAt(int x, int y) {
  if (intLimDel(x, properties.width, deltaBorder)) return nullptr;
  if (intLimDel(x, 0, deltaBorder)) return nullptr;
  for (WindowItem *item : items) {
    if (y < *item->getBottom()+deltaBorder || y > *item->getTop()+deltaBorder) continue;
    if (intLimDel(x, *item->getLeft(), deltaBorder)) return item->getLeft();
    if (intLimDel(x, *item->getRight(), deltaBorder)) return item->getRight();
  }
  return nullptr; 
}

int *App::horizontalBorderAt(int x, int y) {
  if (intLimDel(y, properties.height, deltaBorder)) return nullptr;
  if (intLimDel(y, 0, deltaBorder)) return nullptr;
  for (WindowItem *item : items) {
    if (x < *item->getLeft()+deltaBorder || x > *item->getRight()+deltaBorder) continue;
    if (intLimDel(y, *item->getTop(), deltaBorder)) return item->getTop();
    if (intLimDel(y, *item->getBottom(), deltaBorder)) return item->getBottom();
  }
  return nullptr; 
}

void App::resizeWindow(int w, int h) {
  int deltaW = w - properties.width;
  int deltaH = h - properties.height;

  int diffW = (deltaW >= 0) ? growWindow(deltaW, true) : shrinkWindow(-deltaW, true);
  int diffH = (deltaH >= 0) ? growWindow(deltaH, false) : shrinkWindow(-deltaH, false);

  updateItems();

  properties.width += (deltaW >= 0) ? diffW : -diffW; 
  properties.height += (deltaH >= 0) ? diffH : -diffH;
} //resizeWindow

void App::updateItems() {
  bool changed = true;
  while (changed) {
    changed = false;
    for (WindowItem *item : items) if (item->updateSize()) changed = true;
  }
} //updateItems

int App::shrinkWindow(int delta, bool w) {
  int nGraphs = windowOrder->getNGraphs(w);

  if (nGraphs == 0) return 0;

  int diff = delta / nGraphs;
  int rest = delta % nGraphs;

  return windowOrder->shrink(diff, rest, w);
} //shrinkWindowWidth

int App::growWindow(int delta, bool w) {
  int nGraphs = windowOrder->getNGraphs(w);

  if (nGraphs == 0) return 0;

  int diff = delta / nGraphs;
  int rest = delta % nGraphs;

  return windowOrder->grow(diff, rest, w);
} //growWindowWidth

App::~App() { 
  delete windowOrder;
  for (WindowItem* item : items) delete item;
  
  SDL_GL_DestroyContext(context);

  if (window != nullptr) SDL_DestroyWindow(window);
  SDL_Quit();
} //default destructor

void App::WindowItemOrder::push(WindowItem *oldI, WindowItem *newI, bool bWidth) {
  if (bWidth) setToHorizontal();
  else setToVertical();

  if (oldI == nullptr) { //both orders are empty
    order->emplace_back(newI); 
    orderOpp->emplace_back(newI);
    return;
  }

  int max = order->size();
  for (int i = 0; i < max; i++) 
    if (order->at(i).contains(oldI)) order->at(i).add(newI);
  
  max = orderOpp->size();
  for (int i = 0; i < max; i++) 
    if (orderOpp->at(i).contains(oldI)) orderOpp->emplace_back(oldI, orderOpp->at(i).data(), newI);
} //push


int App::WindowItemOrder::shrink(int delta, int rest, bool bWidth) {
  if (bWidth) setToHorizontal();
  else setToVertical();

  if (order->empty()) return 0;

  int size = order->size();
  int zeroes = 0;
  int totalChange = 0;

  if (delta == 0) {
    for (int idx = 0; idx < rest; idx++) {
      int maxWidth = 0; 
      WindowItemGraph *largest = findLargestGraph(bWidth, &maxWidth);
      if (maxWidth != -1) {largest->grow(-1, bWidth);} 
      else {return idx;}
    }
    return rest;
  }

  int max;
  for (WindowItemGraph &i : *order) {
    max = i.maxShrink(bWidth);
    if (max <= 0) {zeroes++; continue;} 

    if (max < delta) rest += delta-max;
    else max = delta;

    i.grow(-max, bWidth);
    totalChange += max;
  }

  if (zeroes != size && rest != 0) 
    totalChange += shrink(rest / (size-zeroes), (rest % (size-zeroes)), bWidth);

  return totalChange;
} //shrink

int App::WindowItemOrder::grow(int delta, int rest, bool bWidth) {
  if (bWidth) setToHorizontal();
  else setToVertical();
  
  if (order->empty()) return 0;

  int size = order->size();
  int min = INT_MAX;

  for (int i = 0; i < size; i++) 
    order->at(i).grow(delta, bWidth);

  for (int idx = 0; idx < rest; idx++) 
    findSmallestGraph(bWidth, &min)->grow(1, bWidth);

  return delta * size + rest;
} //grow

App::WindowItemOrder::WindowItemGraph *App::WindowItemOrder::findLargestGraph(bool bWidth, int *out){
  WindowItemGraph *largest = nullptr;
  int temp = 0;
  *out = -1;
  for (WindowItemGraph &i : *order) {
    temp = i.maxShrink(bWidth);

    if (temp > *out) {
      largest = &i; 
      *out = temp;
    }
  }
  return largest;
} //largestGraph

App::WindowItemOrder::WindowItemGraph *App::WindowItemOrder::findSmallestGraph(bool bWidth, int *out){
  WindowItemGraph *smallest = nullptr;
  int temp = 0;
  *out = INT_MAX;
  for (WindowItemGraph &i : *order) {
    temp = i.maxShrink(bWidth);

    if (*out > temp) {
      smallest = &i; 
      *out = temp;
    }
  }
  return smallest;
} //largestGraph

App::WindowItemOrder::WindowItemGraph::WindowItemGraph(WindowItem *ignore, std::vector<WindowItem*> &copy, WindowItem *add) {
  int size = copy.size();
  for (int i = 0; i < size; i++) {
    if (copy.at(i) == ignore) continue;
    graph.push_back(copy.at(i));
  }
  graph.push_back(add);
} //constructor
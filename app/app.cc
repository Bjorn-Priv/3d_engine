#include "include/app.h"

App::App(SDL_InitFlags f1, double GL, S_WindProp p) : initialised(true), window(nullptr), properties(p), windowOrder(new WindowItemOrder(&items)) {
  int maj = (int)GL;
  int min = (int)((GL*10.0) - maj*10);

  initialised = SDL_Init(f1) && 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, maj) &&
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, min) &&
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  
  if (!initialised) {
    SDL_Log("%s", SDL_GetError());
    return;
  }

  if (p.width < p.MinWidth) properties.width = p.MinWidth;
  if (p.height < p.MinHeight) properties.height = p.MinHeight;

  window = SDL_CreateWindow(p.name, properties.width, properties.height, p.flags);
  SDL_SetWindowMinimumSize(window, p.MinWidth, p.MinHeight);
  context = SDL_GL_CreateContext(window);
  glewExperimental = GL_TRUE;
  initialised = glewInit() == GLEW_OK;
  glGetError();
  glEnable(GL_DEPTH_TEST);
} //constructor

void App::update() {
  if (!initialised) return;

  for (WindowItem* item : items) item->AppUpdate();
} //update

void App::handleEvent(SDL_Event e) {
  if (!initialised) return;
  if (e.type == SDL_EVENT_WINDOW_RESIZED)  //window is resized
    resizeWindow(e.window.data1, e.window.data2);
  if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) 
    SDL_Log("CLICK!!!!!!!!!!");

  
} //handleEvent

void App::resizeWindow(int w, int h) {
  int deltaW = w - properties.width;
  int deltaH = h - properties.height;

  int diffW = (deltaW >= 0) ? growWindowWidth(deltaW) : shrinkWindowWidth(-deltaW);
  int diffH = (deltaH >= 0) ? growWindowHeight(deltaH) : shrinkWindowHeight(-deltaH);

  windowOrder->updateItems();

  properties.width += (deltaW >= 0) ? diffW : -diffW; 
  properties.height += (deltaH >= 0) ? diffH : -diffH;

  // for (size_t i = 0; i < items.size(); i++) {
  //   SDL_Log("i: %d, X : %d, Y : %d, width : %d, height %d", (int)i, items[i]->getX(), items[i]->getY(), items[i]->getWidth(), items[i]->getHeight());
  // }
  // SDL_Log("__________________________");
} //resizeWindow

int App::shrinkWindowWidth(int deltaX) {
  windowOrder->setHorizontal(true);
  int nGraphs = windowOrder->getNGraphs();

  if (nGraphs == 0) return 0;

  int diff = deltaX / nGraphs;
  int rest = deltaX % nGraphs;

  return windowOrder->shrinkWidth(diff, rest);
} //shrinkWindowWidth

int App::shrinkWindowHeight(int deltaY) {
  windowOrder->setHorizontal(false);
  int nGraphs = windowOrder->getNGraphs();

  if (nGraphs == 0) return 0;

  int diff = deltaY / nGraphs;
  int rest = deltaY % nGraphs;

  return windowOrder->shrinkHeight(diff, rest);
} //shrinkWindowHeight

int App::growWindowWidth(int deltaX) {
  windowOrder->setHorizontal(true);
  int nGraphs = windowOrder->getNGraphs();

  if (nGraphs == 0) return 0;

  int diff = deltaX / nGraphs;
  int rest = deltaX % nGraphs;

  return windowOrder->growWidth(diff, rest);
} //growWindowWidth

int App::growWindowHeight(int deltaY) {
  windowOrder->setHorizontal(false);
  int nGraphs = windowOrder->getNGraphs();

  if (nGraphs == 0) return 0;

  int diff = deltaY / nGraphs;
  int rest = deltaY % nGraphs;

  return windowOrder->growHeight(diff, rest);
} //growWindowHeight

void App::render() {
  if (!initialised) return;

  glViewport(0, 0, properties.width, properties.height);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (WindowItem* item : items) item->AppRender  ();

  SDL_GL_SwapWindow(window);
} //render

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

App::~App() { 
  delete windowOrder;
  for (WindowItem* item : items) delete item;
  
  SDL_GL_DestroyContext(context);

  if (window != nullptr) SDL_DestroyWindow(window);
  SDL_Quit();
} //default destructor

void App::WindowItemOrder::push(WindowItem *oldI, WindowItem *newI) {
  std::vector<WindowItemGraph> *temp = horizontal ? &ItemsInOrderH : &ItemsInOrderV;
  std::vector<WindowItemGraph> *tempO = !horizontal ? &ItemsInOrderH : &ItemsInOrderV;

  if (oldI == nullptr) {
    temp->push_back(WindowItemGraph());
    temp->at(0).add(newI);
    tempO->push_back(WindowItemGraph());
    tempO->at(0).add(newI);
    return;
  }

  size_t max = temp->size();

  for (size_t i = 0; i < max; i++) 
    if (temp->at(i).contains(oldI)) temp->at(i).add(newI);
  
  max = tempO->size();

  for (size_t i = 0; i < max; i++) 
    if (tempO->at(i).contains(oldI)) tempO->push_back(WindowItemGraph(oldI, tempO->at(i).data(), newI));
} //push

int App::WindowItemOrder::shrinkWidth(int delta, int rest) {
  if (ItemsInOrderH.empty()) return 0;
  int zeroes = 0;
  int totalChange = 0;

  if (delta == 0 && rest != 0) {
    for (int idx = 0; idx < rest; idx++) {
      int maxI = 0;
      int maxWidth = -1;
      int temp;
      for (size_t i = 0; i < ItemsInOrderH.size(); i++) {
        temp = ItemsInOrderH[i].maxShrinkageWidth();
        if (temp > maxWidth) {
          maxI = i;
          maxWidth = temp;
        }
      }
      if (maxWidth != -1) {
        ItemsInOrderH[maxI].shrinkWidth(1); 
      } else {return idx;}
    }
    return rest;
  }

  for (size_t i = 0; i < ItemsInOrderH.size(); i++) {
    int max = ItemsInOrderH[i].maxShrinkageWidth();

    if (max <= 0) {zeroes++; continue;} 

    if (max < delta) rest += delta-max;
    else max = delta;

    ItemsInOrderH[i].shrinkWidth(max);
    totalChange += max;
  }

  if (zeroes != (int)ItemsInOrderH.size() && rest != 0) 
    totalChange += shrinkWidth(rest / (ItemsInOrderH.size()-zeroes), (rest % (ItemsInOrderH.size()-zeroes)));

  return totalChange;
}

int App::WindowItemOrder::growWidth(int delta, int rest) {
  if (ItemsInOrderH.empty()) return 0;
  int min = INT_MAX;
  int minI = 0;

  for (size_t i = 0; i < ItemsInOrderH.size(); i++) {
    ItemsInOrderH[i].growWidth(delta);
  }

  for (int idx = 0; idx < rest; idx++) {
    for (size_t i = 0; i < ItemsInOrderH.size(); i++) {
      int temp = ItemsInOrderH[i].maxShrinkageWidth();
      if (min > temp) {
        min = temp;
        minI = i;
      }
    }
    ItemsInOrderH[minI].growWidth(1); 
  }

  return delta*ItemsInOrderH.size() + rest;
}

int App::WindowItemOrder::shrinkHeight(int delta, int rest) {
  if (ItemsInOrderV.empty()) return 0;
  int zeroes = 0;
  int totalChange = 0;

  if (delta == 0 && rest != 0) {
    for (int idx = 0; idx < rest; idx++) {
      int maxI = 0;
      int max = 0;
      for (size_t i = 0; i < ItemsInOrderV.size(); i++) {
        int temp = ItemsInOrderV[i].maxShrinkageHeight();
        if (temp > max) {
          maxI = i;
          max = temp;
        }
      }
      if (max > 0) {
        ItemsInOrderV[maxI].shrinkHeight(1); 
      } else {return idx;}
    }
    return rest;
  }

  for (size_t i = 0; i < ItemsInOrderV.size(); i++) {
    int max = ItemsInOrderV[i].maxShrinkageHeight();

    if (max <= 0) {zeroes++; continue;} 

    if (max < delta) rest += delta-max;
    else max = delta;

    ItemsInOrderV[i].shrinkHeight(max);
    totalChange += max;
  }

  if (zeroes != (int)ItemsInOrderV.size() && rest != 0) 
    totalChange += shrinkHeight(rest / (ItemsInOrderV.size()-zeroes), (rest % (ItemsInOrderV.size()-zeroes)));

  return totalChange;
}

int App::WindowItemOrder::growHeight(int delta, int rest) {
  if (ItemsInOrderV.empty()) return 0;

  for (size_t i = 0; i < ItemsInOrderV.size(); i++) {
    ItemsInOrderV[i].growHeight(delta);
  }

  for (int idx = 0; idx < rest; idx++) {
    int min = INT_MAX;
    int minI = 0;
    for (size_t i = 0; i < ItemsInOrderV.size(); i++) {
      int temp = ItemsInOrderV[i].maxShrinkageHeight();
      if (min > temp) {
        min = temp;
        minI = i;
      }
    }

    ItemsInOrderV[minI].growHeight(1); 
  }

  return delta*ItemsInOrderV.size() + rest;
}

void App::WindowItemOrder::updateItems() {
  bool changed = true;
  while (changed) {
    changed = false;
    for (WindowItem *item : *items) if (item->updateSize()) changed = true;
  }
}

App::WindowItemOrder::WindowItemGraph::WindowItemGraph(WindowItem *ignore, std::vector<WindowItem*>* copy, WindowItem *add) {
  for (size_t i = 0; i < copy->size(); i++) {
    if (copy->at(i) == ignore) continue;
    graph.push_back(copy->at(i));
  }
  graph.push_back(add);
} //constructor


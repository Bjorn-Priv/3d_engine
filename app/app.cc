#include "include/app.h"

App::App(SDL_InitFlags f1, double GL, S_WindProp p) : initialised(true), window(nullptr), properties(p) {
  int maj = (int)GL;
  int min = (int)((GL*10.0) - maj*10);

  initialised = SDL_Init(f1) && 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, maj) &&
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, min) &&
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  
  if (!initialised) {
    SDL_Log("%s", SDL_GetError());
  } else { 
    if (properties.width < p.MinWidth) properties.width = p.MinWidth;
    if (properties.height < p.MinHeight) properties.height = p.MinHeight;
    window = SDL_CreateWindow(p.name, properties.width, properties.height, p.flags);
    SDL_SetWindowMinimumSize(window, p.MinWidth, p.MinHeight);
    context = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    initialised = glewInit() == GLEW_OK;
    glGetError();
    glEnable(GL_DEPTH_TEST);
  }
} //default constructor

void App::update() {
  if (!initialised) return;

  glViewport(0, 0, properties.width, properties.height);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (WindowItem* item : items) item->update();
}

void App::handleEvent(SDL_Event e) {
  if (!initialised) return;
  if (e.type == SDL_EVENT_WINDOW_RESIZED) { //window is resized

    for (size_t i = 0; i < items.size(); i++) {
      int nWidth = e.window.data1 - items[i]->getX();
      int nHeight = e.window.data2 - items[i]->getY();
      // if (items[i]->getX() + items[i]->getWidth() == properties.width) items[i]->changeWidth(nWidth);
      // if (items[i]->getY() + items[i]->getHeight() == properties.height) items[i]->changeHeight(nHeight);
    }

    properties.width = e.window.data1; //grab width
    properties.height = e.window.data2; //grab height
  }
  
}

void App::render() {
  if (!initialised) return;

  for (WindowItem* item : items) item->render();

  SDL_GL_SwapWindow(window);
}

App::~App() { 
  for (WindowItem* item : items) delete item;
  
  SDL_GL_DestroyContext(context);

  if (window != nullptr) SDL_DestroyWindow(window);
  SDL_Quit();
} //default destructor
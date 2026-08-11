#include "app/include/app.h"

//runs the main loop of the program
int main() {
  bool running = true; //main program boolean

  S_WindProp p = {"Blender", 600, 600, 500, 500, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE};
  App application{SDL_INIT_VIDEO, 3.3, p};

  SDL_Event event; //event stack

  Item3D<Actions3D> *sub1 = application.createWindowItem<Item3D<Actions3D>>(0, true);
  Item3D<Actions3D> *sub2 = application.createWindowItem<Item3D<Actions3D>>(sub1->getID(), true);
  Item3D<Actions3D> *sub3 = application.createWindowItem<Item3D<Actions3D>>(sub2->getID(), false);

  GLclampf red[4] = {0.5f, 0.1f, 0.1f, 1.0f};
  GLclampf blue[4] = {0.1f, 0.1f, 0.5f, 1.0f};
  GLclampf green[4] = {0.1f, 0.5f, 0.1f, 1.0f};

  sub1->setBG(red);
  sub2->setBG(green);
  sub3->setBG(blue);
  
  
  while (running) { //main loop
    while (running && SDL_PollEvent(&event)) { //handle events
      if (event.type == SDL_EVENT_QUIT) { //why quit? :(
        running = false;
      }
      application.handleEvent(event);
    } //while

    application.update();
    application.render();

  } //while
} //main_loop

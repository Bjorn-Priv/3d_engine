#include "app/include/app.h"

//runs the main loop of the program
int main() {
  S_WindProp p = {"Blender", 600, 600, 500, 500, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE};
  App application{SDL_INIT_VIDEO, 3.3, p};

  //TODO: add minimum size to the contuctor so that easier to find per window minimum size
  Item3D<Actions3D> *sub1 = application.createWindowItem<Item3D<Actions3D>>(nullptr, true);
  Item3D<Actions3D> *sub2 = application.createWindowItem<Item3D<Actions3D>>(sub1, true);
  Item3D<Actions3D> *sub3 = application.createWindowItem<Item3D<Actions3D>>(sub2, true);
  Item3D<Actions3D> *sub4 = application.createWindowItem<Item3D<Actions3D>>(sub3, false);
  Item3D<Actions3D> *sub5 = application.createWindowItem<Item3D<Actions3D>>(sub1, false);


  GLclampf red[4] = {0.5f, 0.1f, 0.1f, 1.0f};
  GLclampf blue[4] = {0.1f, 0.1f, 0.5f, 1.0f};
  GLclampf green[4] = {0.1f, 0.5f, 0.1f, 1.0f};
  GLclampf black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  GLclampf white[4] = {1.0f, 1.0f, 1.0f, 1.0f};

  sub1->setBG(red);
  sub2->setBG(green);
  sub3->setBG(blue);
  sub4->setBG(black);
  sub5->setBG(white);

  SDL_Log("after sbu1->update");
  application.run();

  
  
} //main_loop

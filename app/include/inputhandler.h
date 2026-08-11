#ifndef HINPUTHANDLER
#define HINPUTHANDLER

#include <SDL3/SDL.h>

#include <GL/glew.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>


enum Actions3D {
  HI
};

template <class T>
class InputHandler {
  private:
    //map with template type for custom enum actions 
    //return correct action based on input 

  public:
    InputHandler();

    ~InputHandler();
};

template <class T>
InputHandler<T>::InputHandler(){

}

template <class T>
InputHandler<T>::~InputHandler(){

}


#endif
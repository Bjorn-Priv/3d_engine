#include "include/windowitem.h"

void WindowItem::AppUpdate() {
  if (!initialised) return;
  update();
}

void WindowItem::AppRender() {
  if (!initialised) return;

  if (openGL) {
    glViewport(getX(), getY(), getWidth(), getHeight());

    glEnable(GL_SCISSOR_TEST);
    glScissor(getX(), getY(), getWidth(), getHeight());
  }
  render();
  if (openGL) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_SCISSOR_TEST);
  }
}
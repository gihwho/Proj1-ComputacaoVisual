#ifndef APP_H
#define APP_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdint.h>
#include <stdbool.h>
#include "ui.h"

typedef struct {
  SDL_Window*   winMain;
  SDL_Renderer* renMain;

  SDL_Window*   winSide;
  SDL_Renderer* renSide;

  Uint32 mainID, sideID;
  bool   mainOpen, sideOpen;

  // Imagens/superfícies
  SDL_Surface*  img_rgba32;
  SDL_Surface*  gray_rgba32;
  SDL_Surface*  eq_rgba32;

  // Texturas para exibir na janela principal
  SDL_Texture*  texImage;
  SDL_Texture*  texImageGray;
  SDL_Texture*  texImageEq;
  bool          usingEq;

  // Texto/fontes (janela lateral)
  TTF_Font*     font;
  SDL_Texture * line1, *line2;
  int           l1w, l1h, l2w, l2h;

  // UI
  UIButton      btnEqualize;

  // Histograma
  uint32_t      hist[256];
  double        mean, stddev;

  // Layout
  int           imgW, imgH;
} App;

bool app_init(App* app, const char* imagePath, const char* fontPath);
void app_run(App* app);
void app_destroy(App* app);

#endif
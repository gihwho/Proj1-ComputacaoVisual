#ifndef PROJ_HIST_MODULE_H
#define PROJ_HIST_MODULE_H

#include <SDL3/SDL.h>
#include <stdint.h>

//Calcula histograma (0..255), média e desvio-padrão e exige surface RGBA32 em escala de cinza (R==G==B)
void hist_compute(SDL_Surface* grayRGBA32, uint32_t hist[256],
                  double* mean, double* stddev);

const char* hist_brightness_label(double mean);
const char* hist_contrast_label(double stddev);

// Desenha o histograma no retângulo (x,y,w,h)
void hist_draw(SDL_Renderer* r, const uint32_t hist[256],
               int x, int y, int w, int h);

#endif
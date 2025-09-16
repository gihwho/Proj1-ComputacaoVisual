#ifndef IMAGING_H
#define IMAGING_H

#include <SDL3/SDL.h>
#include <stdbool.h>

// Carrega imagem e converte para RGBA32
SDL_Surface* img_load_rgba32(const char* path);

// Converte para escala de cinza (Y = 0.2125R + 0.7154G + 0.0721B)
SDL_Surface* img_to_grayscale(SDL_Surface* srcRGBA32);

// Verifica se RGBA32 já é grayscale (R==G==B)
bool img_is_grayscale(SDL_Surface* s);

// Retorna uma NOVA surface RGBA32 (grayscale) equalizada
SDL_Surface* img_equalize(SDL_Surface* grayRGBA32);

// Salva como PNG (retorna true em sucesso)
bool img_save_png(SDL_Surface* surface, const char* path);

#endif
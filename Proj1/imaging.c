#include "imaging.h"
#include <SDL3_image/SDL_image.h>
#include <string.h>
#include <stdint.h>

SDL_Surface* img_load_rgba32(const char* path) {
  SDL_Surface* loaded = IMG_Load(path);
  if (!loaded) return NULL;
  SDL_Surface* out = NULL;
  if (loaded->format == SDL_PIXELFORMAT_RGBA32) out = SDL_ConvertSurface(loaded, loaded->format);
  else out = SDL_ConvertSurface(loaded, SDL_PIXELFORMAT_RGBA32);
  SDL_DestroySurface(loaded);
  return out;
}

bool img_is_grayscale(SDL_Surface* s) {
  if (!s || s->format != SDL_PIXELFORMAT_RGBA32) return false;
  if (!SDL_LockSurface(s)) return false;
  const uint8_t* pixels = (const uint8_t*)s->pixels;
  const int pitch = s->pitch;
  for (int y = 0; y < s->h; ++y) {
    const uint8_t* row = pixels + y * pitch;
    for (int x = 0; x < s->w; ++x) {
      const uint8_t* p = row + x * 4;
      if (!(p[0] == p[1] && p[1] == p[2])) { SDL_UnlockSurface(s); return false; }
    }
  }
  SDL_UnlockSurface(s);
  return true;
}

SDL_Surface* img_to_grayscale(SDL_Surface* srcRGBA32) {
  if (!srcRGBA32 || srcRGBA32->format != SDL_PIXELFORMAT_RGBA32) return NULL;
  SDL_Surface* dst = SDL_ConvertSurface(srcRGBA32, srcRGBA32->format);
  if (!dst) return NULL;
  if (!SDL_LockSurface(dst)) { SDL_DestroySurface(dst); return NULL; }

  uint8_t* pixels = (uint8_t*)dst->pixels;
  const int pitch = dst->pitch;
  for (int y = 0; y < dst->h; ++y) {
    uint8_t* row = pixels + y * pitch;
    for (int x = 0; x < dst->w; ++x) {
      uint8_t* p = row + x * 4;
      float r = (float)p[0], g = (float)p[1], b = (float)p[2];
      float Y = 0.2125f*r + 0.7154f*g + 0.0721f*b;
      if (Y < 0.f)   Y = 0.f;
      if (Y > 255.f) Y = 255.f;
      uint8_t y8 = (uint8_t)Y;
      p[0] = y8; p[1] = y8; p[2] = y8;
    }
  }
  SDL_UnlockSurface(dst);
  return dst;
}

SDL_Surface* img_equalize(SDL_Surface* grayRGBA32) {
  if (!grayRGBA32 || grayRGBA32->format != SDL_PIXELFORMAT_RGBA32) return NULL;

  uint32_t hist[256]; memset(hist, 0, sizeof(hist));
  uint64_t npx = (uint64_t)grayRGBA32->w * (uint64_t)grayRGBA32->h;
  if (!SDL_LockSurface(grayRGBA32)) return NULL;
  const uint8_t* spx = (const uint8_t*)grayRGBA32->pixels;
  const int pitch = grayRGBA32->pitch;
  for (int y = 0; y < grayRGBA32->h; ++y) {
    const uint8_t* row = spx + y * pitch;
    for (int x = 0; x < grayRGBA32->w; ++x) {
      const uint8_t* p = row + x * 4;
      hist[p[0]]++;
    }
  }
  SDL_UnlockSurface(grayRGBA32);
  if (npx == 0) return SDL_ConvertSurface(grayRGBA32, grayRGBA32->format);

  uint64_t cdf = 0;
  uint64_t cdf_min = 0;
  int found_min = 0;

  uint32_t lut[256];
  for (int i = 0; i < 256; ++i) {
    cdf += hist[i];
    if (!found_min && hist[i] != 0) { cdf_min = cdf; found_min = 1; }
    double denom = (double)(npx - cdf_min);
    double val = denom > 0.0 ? ((double)(cdf - cdf_min) / denom) * 255.0 : 0.0;
    if (val < 0.0) val = 0.0; if (val > 255.0) val = 255.0;
    lut[i] = (uint32_t)(val + 0.5);
  }

  SDL_Surface* out = SDL_ConvertSurface(grayRGBA32, grayRGBA32->format);
  if (!out) return NULL;
  if (!SDL_LockSurface(out)) { SDL_DestroySurface(out); return NULL; }
  uint8_t* dpx = (uint8_t*)out->pixels;
  const int dpitch = out->pitch;
  for (int y = 0; y < out->h; ++y) {
    uint8_t* row = dpx + y * dpitch;
    for (int x = 0; x < out->w; ++x) {
      uint8_t* p = row + x * 4;
      uint8_t g = p[0];
      uint8_t e = (uint8_t)lut[g];
      p[0] = e; p[1] = e; p[2] = e;
    }
  }
  SDL_UnlockSurface(out);
  return out;
}

bool img_save_png(SDL_Surface* surface, const char* path) {
  if (!surface || !path) return false;
  return IMG_SavePNG(surface, path);
}
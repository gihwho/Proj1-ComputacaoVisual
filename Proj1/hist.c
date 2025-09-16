#include "hist.h"
#include "config.h"
#include <math.h>

void hist_compute(SDL_Surface* grayRGBA32, uint32_t hist[256],
                  double* out_mean, double* out_stddev) {
  for (int i = 0; i < 256; ++i) hist[i] = 0;

  const uint64_t npx = (uint64_t)grayRGBA32->w * (uint64_t)grayRGBA32->h;
  if (npx == 0) { if (out_mean) *out_mean = 0; if (out_stddev) *out_stddev = 0; return; }

  if (!SDL_LockSurface(grayRGBA32)) { if (out_mean) *out_mean = 0; if (out_stddev) *out_stddev = 0; return; }
  const uint8_t* pixels = (const uint8_t*)grayRGBA32->pixels;
  const int pitch = grayRGBA32->pitch;

  uint64_t sum = 0;
  for (int y = 0; y < grayRGBA32->h; ++y) {
    const uint8_t* row = pixels + y * pitch;
    for (int x = 0; x < grayRGBA32->w; ++x) {
      const uint8_t* p = row + x * 4;
      uint8_t v = p[0];          // R==G==B
      hist[v]++; sum += v;
    }
  }
  SDL_UnlockSurface(grayRGBA32);

  double mean = (double)sum / (double)npx;
  long double acc = 0.0L;
  for (int i = 0; i < 256; ++i) {
    long double d = (long double)i - (long double)mean;
    acc += (long double)hist[i] * d * d;
  }
  double variance = (double)(acc / (long double)npx);
  if (out_mean)   *out_mean = mean;
  if (out_stddev) *out_stddev = sqrt(variance);
}

const char* hist_brightness_label(double mean) {
  if (mean < 85.0)  return "escura";
  if (mean < 170.0) return "média";
  return "clara";
}

const char* hist_contrast_label(double stddev) {
  if (stddev < 30.0)  return "baixo";
  if (stddev < 70.0)  return "médio";
  return "alto";
}

void hist_draw(SDL_Renderer* r, const uint32_t hist[256],
               int x, int y, int w, int h) {
  uint32_t peak = 1;
  for (int i = 0; i < 256; ++i) if (hist[i] > peak) peak = hist[i];

  // moldura
  SDL_SetRenderDrawColor(r, 200, 200, 200, 255);
  SDL_FRect rect = { (float)x, (float)y, (float)w, (float)h };
  SDL_RenderRect(r, &rect);

  // barras
  float barW = (float)w / 256.0f;
  for (int i = 0; i < 256; ++i) {
    float norm = (float)hist[i] / (float)peak;
    float barH = norm * (float)(h - 2);
    SDL_FRect bar = { (float)x + i * barW, (float)(y + h - 1) - barH, barW, barH };
    SDL_SetRenderDrawColor(r, 100, 180, 255, 255);
    SDL_RenderFillRect(r, &bar);
  }
}
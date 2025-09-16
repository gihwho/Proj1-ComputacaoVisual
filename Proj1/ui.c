#include "ui.h"
#include "config.h"
#include <string.h>

bool ui_button_handle_event(UIButton* b, const SDL_Event* ev, Uint32 windowID) {
  if (ev->window.windowID != windowID) return false;

  if (ev->type == SDL_EVENT_MOUSE_MOTION ||
      ev->type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
      ev->type == SDL_EVENT_MOUSE_BUTTON_UP) {

    float mx=0, my=0; SDL_GetMouseState(&mx, &my);
    bool inside = (mx >= b->rect.x && mx <= b->rect.x + b->rect.w &&
                   my >= b->rect.y && my <= b->rect.y + b->rect.h);

    if (ev->type == SDL_EVENT_MOUSE_MOTION) {
      b->state = inside ? BTN_HOVER : BTN_NEUTRAL;
    } else if (ev->type == SDL_EVENT_MOUSE_BUTTON_DOWN && inside && ev->button.button == SDL_BUTTON_LEFT) {
      b->state = BTN_PRESS;
    } else if (ev->type == SDL_EVENT_MOUSE_BUTTON_UP && ev->button.button == SDL_BUTTON_LEFT) {
      b->state = inside ? BTN_HOVER : BTN_NEUTRAL;
      if (inside) return true;
    }
  }
  return false;
}

void ui_button_draw(SDL_Renderer* r, UIButton* b, TTF_Font* font) {
  if      (b->state == BTN_PRESS) SDL_SetRenderDrawColor(r, 10, 90, 180, 255);
  else if (b->state == BTN_HOVER) SDL_SetRenderDrawColor(r, 60,140,255, 255);
  else                            SDL_SetRenderDrawColor(r, 25,110,220, 255);
  SDL_RenderFillRect(r, &b->rect);

  if (font && b->label) {
    SDL_Texture* t = ui_render_text(r, font, b->label, (SDL_Color){255,255,255,255}, NULL, NULL);
    if (t) {
      float tw=0.f, th=0.f;
      SDL_GetTextureSize(t, &tw, &th);
      SDL_FRect dst = { b->rect.x + (b->rect.w - tw)/2.f, b->rect.y + (b->rect.h - th)/2.f, tw, th };
      SDL_RenderTexture(r, t, NULL, &dst);
      SDL_DestroyTexture(t);
    }
  }
}

SDL_Texture* ui_render_text(SDL_Renderer* r, TTF_Font* font, const char* txt,
                            SDL_Color color, int* out_w, int* out_h) {
  if (!font || !txt || !*txt) return NULL;
  size_t len = strlen(txt);
  SDL_Surface* s = TTF_RenderText_Blended(font, txt, len, color);
  if (!s) return NULL;
  SDL_Texture* t = SDL_CreateTextureFromSurface(r, s);
  if (t && out_w) *out_w = s->w;
  if (t && out_h) *out_h = s->h;
  SDL_DestroySurface(s);
  return t;
}
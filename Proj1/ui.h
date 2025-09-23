#ifndef UI_H
#define UI_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

typedef enum { BTN_NEUTRAL=0, BTN_HOVER, BTN_PRESS } BtnState;

typedef struct {
  SDL_FRect rect;
  BtnState state;
  const char* label;
} UIButton;

// Inicializa um botão
static inline void ui_button_init(UIButton* b, float x, float y, float w, float h, const char* label) {
  b->rect = (SDL_FRect){x,y,w,h};
  b->state = BTN_NEUTRAL;
  b->label = label;
}

// Processa eventos de mouse para o botão (retorna true se houve clique solto dentro)
bool ui_button_handle_event(UIButton* b, const SDL_Event* ev, Uint32 windowID);

// Desenha o botão (rótulo centralizado)
void ui_button_draw(SDL_Renderer* r, UIButton* b, TTF_Font* font);

// Renderiza texto com SDL_ttf 3.0x -> retorna texture com tamanho. out_w/out_h são opcionais
SDL_Texture* ui_render_text(SDL_Renderer* r, TTF_Font* font, const char* txt,
                            SDL_Color color, int* out_w, int* out_h);

#endif
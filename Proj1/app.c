#include "app.h"
#include "config.h"
#include "imaging.h"
#include "hist.h"
#include "ui.h"
#include <SDL3_image/SDL_image.h>

static void make_side_text(App* app) {
  if (!app->renSide || !app->font) return;
  if (app->line1) { SDL_DestroyTexture(app->line1); app->line1 = NULL; }
  if (app->line2) { SDL_DestroyTexture(app->line2); app->line2 = NULL; }

  const char* b = hist_brightness_label(app->mean);
  const char* c = hist_contrast_label(app->stddev);

  char l1[128]; SDL_snprintf(l1, sizeof(l1), "média: %.1f (%s)", app->mean, b);
  char l2[128]; SDL_snprintf(l2, sizeof(l2), "contraste: %.1f (%s)", app->stddev, c);

  app->line1 = ui_render_text(app->renSide, app->font, l1, (SDL_Color){255,255,255,255}, &app->l1w, &app->l1h);
  app->line2 = ui_render_text(app->renSide, app->font, l2, (SDL_Color){255,255,255,255}, &app->l2w, &app->l2h);
}

bool app_init(App* app, const char* imagePath, const char* fontPath) {
  *app = (App){0};
  app->mainOpen = app->sideOpen = true;

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL_Init falhou: %s", SDL_GetError());
    return false;
  }
  if (!TTF_Init()) {
    SDL_Log("TTF_Init falhou: %s", SDL_GetError());
  }

  // Carregar imagem e converter p/ RGBA32
  app->img_rgba32 = img_load_rgba32(imagePath);
  if (!app->img_rgba32) {
    SDL_Log("Falha ao carregar imagem: %s", SDL_GetError());
    return false;
  }
  app->imgW = app->img_rgba32->w;
  app->imgH = app->img_rgba32->h;

  // Garantir grayscale
  app->gray_rgba32 = img_is_grayscale(app->img_rgba32)
      ? SDL_ConvertSurface(app->img_rgba32, app->img_rgba32->format)
      : img_to_grayscale(app->img_rgba32);
  if (!app->gray_rgba32) return false;

  // Janela principal
  app->winMain = SDL_CreateWindow("Proj1 - Imagem (Grayscale)", app->imgW, app->imgH, 0);
  if (!app->winMain) return false;
  SDL_SetWindowPosition(app->winMain, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  app->renMain = SDL_CreateRenderer(app->winMain, NULL);
  if (!app->renMain) return false;

  // Texturas principais
  app->texImageGray = SDL_CreateTextureFromSurface(app->renMain, app->gray_rgba32);
  if (!app->texImageGray) return false;

  app->texImage   = app->texImageGray; // exibe grayscale por padrão
  app->texImageEq = NULL;
  app->eq_rgba32  = NULL;
  app->usingEq    = false;

  // Janela lateral
  int mainX=0, mainY=0; SDL_GetWindowPosition(app->winMain, &mainX, &mainY);
  app->winSide = SDL_CreateWindow("Histograma", SIDE_W, SIDE_H, 0);
  if (!app->winSide) return false;
  SDL_SetWindowPosition(app->winSide, mainX + app->imgW + 16, mainY);
  app->renSide = SDL_CreateRenderer(app->winSide, NULL);
  if (!app->renSide) return false;

  app->mainID = SDL_GetWindowID(app->winMain);
  app->sideID = SDL_GetWindowID(app->winSide);

  // Histograma e estatisticas da imagem atual
  hist_compute(app->gray_rgba32, app->hist, &app->mean, &app->stddev);

  // Fonte
  if (TTF_WasInit() && fontPath) {
    app->font = TTF_OpenFont(fontPath, 12);
    if (!app->font) SDL_Log("Fonte falhou (%s)", SDL_GetError());
  }

  make_side_text(app);

  // Botao
  ui_button_init(&app->btnEqualize, (float)UI_PAD, (float)(SIDE_H - UI_PAD - BTN_H), BTN_W, BTN_H, "Equalizar");

  return true;
}

void app_run(App* app) {
  SDL_FRect histRect = { (float)(UI_PAD), (float)(UI_PAD + 40), (float)HIST_W, (float)HIST_H };
  bool running = true;
  while (running) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      switch (ev.type) {
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
          Uint32 id = ev.window.windowID;
          if (app->sideOpen && id == app->sideID) {
            if (app->line1) { SDL_DestroyTexture(app->line1); app->line1 = NULL; }
            if (app->line2) { SDL_DestroyTexture(app->line2); app->line2 = NULL; }
            if (app->renSide){ SDL_DestroyRenderer(app->renSide); app->renSide=NULL; }
            if (app->winSide){ SDL_DestroyWindow(app->winSide);   app->winSide=NULL; }
            app->sideOpen = false;
          } else if (app->mainOpen && id == app->mainID) {
            if (app->texImageEq)   { SDL_DestroyTexture(app->texImageEq);   app->texImageEq = NULL; }
            if (app->texImageGray) { SDL_DestroyTexture(app->texImageGray); app->texImageGray = NULL; }
            app->texImage = NULL; // alias
            if (app->renMain){ SDL_DestroyRenderer(app->renMain); app->renMain=NULL; }
            if (app->winMain){ SDL_DestroyWindow(app->winMain);   app->winMain=NULL; }
            app->mainOpen = false;
          }
          if (!app->mainOpen && !app->sideOpen) running = false;
        } break;

        case SDL_EVENT_KEY_DOWN: {
          Uint32 id = ev.key.windowID;

          if (ev.key.key == SDLK_ESCAPE || ev.key.key == SDLK_Q) {
            if (app->sideOpen && id == app->sideID) {
              if (app->line1) { SDL_DestroyTexture(app->line1); app->line1 = NULL; }
              if (app->line2) { SDL_DestroyTexture(app->line2); app->line2 = NULL; }
              if (app->renSide){ SDL_DestroyRenderer(app->renSide); app->renSide=NULL; }
              if (app->winSide){ SDL_DestroyWindow(app->winSide);   app->winSide=NULL; }
              app->sideOpen = false;
            } else if (app->mainOpen && id == app->mainID) {
              if (app->texImageEq)   { SDL_DestroyTexture(app->texImageEq);   app->texImageEq = NULL; }
              if (app->texImageGray) { SDL_DestroyTexture(app->texImageGray); app->texImageGray = NULL; }
              app->texImage = NULL;
              if (app->renMain){ SDL_DestroyRenderer(app->renMain); app->renMain=NULL; }
              if (app->winMain){ SDL_DestroyWindow(app->winMain);   app->winMain=NULL; }
              app->mainOpen = false;
            } else {
              if (app->sideOpen) {
                if (app->line1) { SDL_DestroyTexture(app->line1); app->line1 = NULL; }
                if (app->line2) { SDL_DestroyTexture(app->line2); app->line2 = NULL; }
                if (app->renSide){ SDL_DestroyRenderer(app->renSide); app->renSide=NULL; }
                if (app->winSide){ SDL_DestroyWindow(app->winSide);   app->winSide=NULL; }
                app->sideOpen = false;
              } else if (app->mainOpen) {
                if (app->texImageEq)   { SDL_DestroyTexture(app->texImageEq);   app->texImageEq = NULL; }
                if (app->texImageGray) { SDL_DestroyTexture(app->texImageGray); app->texImageGray = NULL; }
                app->texImage = NULL;
                if (app->renMain){ SDL_DestroyRenderer(app->renMain); app->renMain=NULL; }
                if (app->winMain){ SDL_DestroyWindow(app->winMain);   app->winMain=NULL; }
                app->mainOpen = false;
              }
            }
            if (!app->mainOpen && !app->sideOpen) running = false;
          }

          // Salvar imagem
          if (ev.key.key == SDLK_S) {
            SDL_Surface* cur = (app->usingEq && app->eq_rgba32) ? app->eq_rgba32 : app->gray_rgba32;
            if (!cur) { SDL_Log("Sem surface atual para salvar."); break; }

            const char* outpng = "output_image.png";
            if (img_save_png(cur, outpng)) {
              SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Salvar",
                                       "Imagem salva em output_image.png", NULL);
            } else {
              const char* outbmp = "output_image.bmp";
              if (SDL_SaveBMP(cur, outbmp)) {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Salvar",
                                         "PNG falhou; imagem salva como output_image.bmp", NULL);
              } else {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Salvar",
                                         "Falha ao salvar PNG e BMP.", NULL);
              }
            }
          }
        } break;

        case SDL_EVENT_MOUSE_MOTION:
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
          if (app->sideOpen && app->renSide && ev.window.windowID == app->sideID) {
            bool clicked = ui_button_handle_event(&app->btnEqualize, &ev, app->sideID);
            if (clicked) {
              if (!app->usingEq) {
                // cria equalizada
                if (!app->eq_rgba32) {
                  app->eq_rgba32 = img_equalize(app->gray_rgba32);
                  if (!app->eq_rgba32) { SDL_Log("Equalização falhou."); break; }
                }
                if (!app->texImageEq) {
                  app->texImageEq = SDL_CreateTextureFromSurface(app->renMain, app->eq_rgba32);
                  if (!app->texImageEq) { SDL_Log("Textura equalizada falhou."); break; }
                }
                app->texImage = app->texImageEq;
                app->usingEq  = true;

                hist_compute(app->eq_rgba32, app->hist, &app->mean, &app->stddev);
                make_side_text(app);

                app->btnEqualize.label = "Original";
              } else {
                app->texImage = app->texImageGray;
                app->usingEq  = false;

                hist_compute(app->gray_rgba32, app->hist, &app->mean, &app->stddev);
                make_side_text(app);

                app->btnEqualize.label = "Equalizar";
              }
            }
          }
          break;

        default: break;
      }
    }

    // Render principal
    if (app->mainOpen && app->renMain) {
      SDL_SetRenderDrawColor(app->renMain, 30, 30, 30, 255);
      SDL_RenderClear(app->renMain);
      if (app->texImage) {
        SDL_FRect dst = {0.f, 0.f, (float)app->imgW, (float)app->imgH};
        SDL_RenderTexture(app->renMain, app->texImage, NULL, &dst);
      }
      SDL_RenderPresent(app->renMain);
    }

    // Render lateral
    if (app->sideOpen && app->renSide) {
      SDL_SetRenderDrawColor(app->renSide, 20, 20, 24, 255);
      SDL_RenderClear(app->renSide);

      float tx = (float)UI_PAD, ty = (float)UI_PAD;
      if (app->line1) { SDL_FRect d = {tx, ty, (float)app->l1w, (float)app->l1h}; SDL_RenderTexture(app->renSide, app->line1, NULL, &d); ty += app->l1h + 6; }
      if (app->line2) { SDL_FRect d = {tx, ty, (float)app->l2w, (float)app->l2h}; SDL_RenderTexture(app->renSide, app->line2, NULL, &d); }

      hist_draw(app->renSide, app->hist, (int)histRect.x, (int)histRect.y, (int)histRect.w, (int)histRect.h);

      ui_button_draw(app->renSide, &app->btnEqualize, app->font);

      SDL_RenderPresent(app->renSide);
    }

    if (!app->mainOpen && !app->sideOpen) running = false;
  }
}

void app_destroy(App* app) {
  if (app->line1) SDL_DestroyTexture(app->line1);
  if (app->line2) SDL_DestroyTexture(app->line2);
  if (app->font)  TTF_CloseFont(app->font);

  if (app->texImageEq)   SDL_DestroyTexture(app->texImageEq);
  if (app->texImageGray) SDL_DestroyTexture(app->texImageGray);

  if (app->renSide) SDL_DestroyRenderer(app->renSide);
  if (app->winSide) SDL_DestroyWindow(app->winSide);

  if (app->renMain) SDL_DestroyRenderer(app->renMain);
  if (app->winMain) SDL_DestroyWindow(app->winMain);

  if (app->eq_rgba32)   SDL_DestroySurface(app->eq_rgba32);
  if (app->gray_rgba32) SDL_DestroySurface(app->gray_rgba32);
  if (app->img_rgba32)  SDL_DestroySurface(app->img_rgba32);

  TTF_Quit();
  SDL_Quit();
}
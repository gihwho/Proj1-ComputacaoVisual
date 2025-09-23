// Copyright (c) 2025 Andre Kishimoto - https://kishimoto.com.br/
// Alunos: Giovanna Borges Coelho e Isabella Rodrigues de Oliveira
// SPDX-License-Identifier: Apache-2.0

#include "app.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    SDL_Log("Uso: %s caminho_da_imagem.ext [caminho_da_fonte.ttf]\n", argv[0]);
    return SDL_APP_FAILURE;
  }
  const char* imagePath = argv[1];
  const char* fontPath  = (argc >= 3) ? argv[2] : "C:\\Users\\Giovanna\\Desktop\\Proj1-Computacao_Visual\\Proj1\\fonts\\NotoSans-Regular.ttf";

  App app;
  if (!app_init(&app, imagePath, fontPath)) return SDL_APP_FAILURE;
  app_run(&app);
  app_destroy(&app);
  return 0;
}
# Proj1-ComputacaoVisual

Software em **C** para processamento de imagens usando **SDL3**,
**SDL_image** e **SDL_ttf**.
Desenvolvido para a disciplina Computação Visual - Universidade
Presbiteriana Mackenzie.

## Objetivo

O programa carrega uma imagem (PNG, JPG ou BMP), verifica se já está em escala de cinza e, se não estiver, converte automaticamente.
Ele exibe:

-   **Janela principal**: mostra a imagem em escala de cinza ou equalizada.
-   **Janela secundária**: mostra o **histograma** da imagem,
    estatísticas (média e contraste) e um **botão** para equalizar ou
    reverter a imagem.

Também permite **salvar a imagem** exibida com um simples comando de
teclado.

## Funcionalidades

1.  **Carregamento de imagem**
    -   Suporta PNG, JPG e BMP
    -   Trata erros de arquivo inexistente ou formato inválido
2.  **Análise e conversão para escala de cinza**
    -   Detecta se a imagem já é grayscale

    -   Converte automaticamente, aplicando a fórmula
            Y = 0.2125 * R + 0.7154 * G + 0.0721 * B
3.  **Interface gráfica**
    -   Duas janelas:
        -   **Principal**: exibe a imagem
        -   **Secundária**: mostra histograma, média de intensidade
            (clara, média ou escura), contraste (alto, médio ou baixo) e
            um botão
4.  **Equalização do histograma**
    -   Botão "Equalizar" aplica equalização na imagem.
    -   Ao clicar novamente, volta para a imagem original
    -   O texto do botão muda dinamicamente
5.  **Salvar imagem**
    -   Pressione **S** para salvar a imagem exibida em
        `output_image.png`
    -   Se o PNG falhar, salva como `output_image.bmp`

## Compilação

- Bibliotecas **SDL3**, **SDL_image** e **SDL_ttf**

No terminal (dentro da pasta do projeto "Proj1"),
execute:

``` bash
mingw32-make
```

Isso compila todo o projeto e gera o executável `main` ou `main.exe`.

## Execução

Para rodar o programa, use, no diretório "...\Proj1-Computacao_Visual\Proj1":

PS.: Lembrando que as imagens devem estar na pasta "Proj1"


``` bash
./main "nomeImagem.extensao"
```
Exemplo:

``` bash
./main "test.png"
```

Ou
 ``` powershell
 .\main.exe "test.png"
 ```

## Estrutura do projeto

  `main.c`       Ponto de entrada, inicializa e executa o app.

  `app.c`        Lógica principal, criação das janelas e loop de eventos.

  `imaging.c`    Carregamento, conversão para grayscale e equalização.

  `hist.c`       Cálculo e desenho do histograma, média e contraste.

  `ui.c`         Interface do botão e renderização de textos.

  `config.h`     Constantes de tamanho e configurações gerais.

## Autores

-   Giovanna Borges Coelho
-   Isabella Rodrigues de Oliveira
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const int WID = 400;
const int HEI = 300;
const int GRX = 40;
const int GRY = 30;

const int GAMEPLAY = 1;
const int WIN_SCREEN = 2;
const int LOSE_SCREEN = 3;

/*
void itoa(int val, char* buf, int base){
	
	static char tmp[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		tmp[i] = "0123456789abcdef"[val % base];
	
	strcpy(buf, tmp);
	
}
*/

const int SPF = 1;

struct tail_segment {
  int id, x, y;
};

int occupied[GRX][GRY] = {0};

int main(int argc, char **args) {
  srand(time(NULL));

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *win = SDL_CreateWindow("", 100, 100, WID, HEI, SDL_WINDOW_SHOWN);
  SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  SDL_Event ev;

  if (TTF_Init() < 0) {
    printf("nogood ttfinit\n");
    return 11;
  }

  TTF_Font *font = TTF_OpenFont("arial.ttf", HEI / GRY * 2 - 2);
  if (font == NULL) {
    printf("nogood font\n");
    return 11;
  }
  SDL_Surface *text = NULL;
  SDL_Texture *text_tex = NULL;

  int posX = 3;
  int posY = 2;
  int dirX = 1;
  int dirY = 0;

  int len = 3;

  int rewX = rand() % GRX;
  int rewY = rand() % GRY + 2;

  int score = 0;
  char buf[32];

  struct tail_segment ts[9999] = {{0, 2, 2}, {1, 1, 2}, {2, 0, 2}};

  itoa(score, buf, 10);
  printf("%s\n", buf);

  int running = 1;

  int current_state = GAMEPLAY;

  while (running) {
    while (SDL_PollEvent(&ev) != 0) {
      if (ev.type == SDL_QUIT)
        running = 0;

      if (ev.type == SDL_KEYDOWN)
        switch (ev.key.keysym.sym) {
        case SDLK_UP:
          dirX = 0;
          dirY = -1;
          break;
        case SDLK_DOWN:
          dirX = 0;
          dirY = 1;
          break;
        case SDLK_LEFT:
          dirX = -1;
          dirY = 0;
          break;
        case SDLK_RIGHT:
          dirX = 1;
          dirY = 0;
          break;
        }
    }
    switch (current_state) {
    case GAMEPLAY:

      if ((dirX == ts[0].x - posX) && (dirY == ts[0].y - posY)) {
        dirX = -dirX;
        dirY = -dirY;
      }

      for (int i = len - 1; i > 0; i--) {
        ts[i].x = ts[i - 1].x;
        ts[i].y = ts[i - 1].y;
      }

      ts[0].x = posX;
      ts[0].y = posY;

      posX = posX + dirX;
      posY = posY + dirY;

      if (posX == GRX)
        posX = 0;
      if (posY == GRY)
        posY = 2;

      if (posX < 0)
        posX = GRX - 1;
      if (posY == 1)
        posY = GRY - 1;

      memset(occupied, 0, sizeof(occupied));
      occupied[posX][posY] = 2;

      for (int i = 0; i < len; i++) {
        occupied[ts[i].x][ts[i].y] = 1;
      }

      if (occupied[posX][posY] == 1) {
        if (score > 100) {
          printf("you win brother! good job!\n");
          printf("your final score is %d\n", score);

          current_state = WIN_SCREEN;
        } else {
          printf("you lost brother. go home.\n");
          printf("final score is %d.\n", score);

          current_state = LOSE_SCREEN;
        }
      }

      if (posX == rewX && posY == rewY) {
        int maxloop = 20;
        while (occupied[rewX][rewY]) {
          rewX = rand() % GRX;
          rewY = 2 + rand() % (GRY - 2);
          if (!maxloop--)
            break;
        }

        printf("%d %d\n", rewX, rewY);

        score += SPF;
        itoa(score, buf, 10);
        printf("%s\n", buf);

        for (int i = 0; i < SPF; i++) {
          ts[len].id = len;
          ts[len].x = ts[len - 1].x + (ts[len - 1].x - ts[len - 2].x);
          ts[len].y = ts[len - 1].y + (ts[len - 1].y - ts[len - 2].y);
          len++;
        }
      }

      SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
      SDL_RenderClear(ren);

      float gw = (float)WID / (float)GRX;
      float gh = (float)HEI / (float)GRY;

      // draw tail
      for (int i = 0; i < len; i++) {
        SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
        SDL_Rect tt = {ts[i].x * gw, ts[i].y * gh, gw, gh};
        SDL_RenderFillRect(ren, &tt);
      }

      // draw player
      SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
      SDL_Rect pos = {posX * gw, posY * gh, gw, gh};
      SDL_RenderFillRect(ren, &pos);

      // draw reward
      SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
      SDL_Rect rew = {rewX * gw, rewY * gh, gw, gh};
      SDL_RenderFillRect(ren, &rew);

      // draw status bar
      SDL_SetRenderDrawColor(ren, 255, 122, 48, 255);
      SDL_Rect st = {0, 0, WID, gh * 2};
      SDL_RenderFillRect(ren, &st);

      // draw score
      SDL_Color col = {255, 255, 255};

      text = TTF_RenderUTF8_Solid(font, buf, col);
      if (text == NULL) {
        printf("nogood text\n");
        return 11;
      }

      text_tex = SDL_CreateTextureFromSurface(ren, text);
      if (text_tex == NULL) {
        printf("nogood text tex\n");
        return 11;
      }

      SDL_Rect sco = {gw, 0, text->w, text->h};
      SDL_RenderCopy(ren, text_tex, NULL, &sco);

      SDL_DestroyTexture(text_tex);
      SDL_FreeSurface(text);
      break;

    case WIN_SCREEN:
      char wtxt[256] = "you win brother! good job! your final score is ";
      strcat(wtxt, buf);

      text = TTF_RenderUTF8_Solid(font, wtxt, col);
      if (text == NULL) {
        printf("nogood text\n");
        return 11;
      }

      text_tex = SDL_CreateTextureFromSurface(ren, text);
      if (text_tex == NULL) {
        printf("nogood text tex\n");
        return 11;
      }

      SDL_Rect winr = {gw, HEI / 2 - gh, text->w, text->h};
      SDL_RenderCopy(ren, text_tex, NULL, &winr);

      SDL_DestroyTexture(text_tex);
      SDL_FreeSurface(text);

      break;
    case LOSE_SCREEN:
      char ltxt[256] = "you lost brother. go home. final score is ";
      strcat(ltxt, buf);

      text = TTF_RenderUTF8_Solid(font, ltxt, col);
      if (text == NULL) {
        printf("nogood text\n");
        return 11;
      }

      text_tex = SDL_CreateTextureFromSurface(ren, text);
      if (text_tex == NULL) {
        printf("nogood text tex\n");
        return 11;
      }

      SDL_Rect losr = {gw, HEI / 2 - gh, text->w, text->h};
      SDL_RenderCopy(ren, text_tex, NULL, &losr);

      SDL_DestroyTexture(text_tex);
      SDL_FreeSurface(text);

      break;
    }

    SDL_RenderPresent(ren);

    SDL_Delay(1000 / 10);
  }

  TTF_CloseFont(font);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}

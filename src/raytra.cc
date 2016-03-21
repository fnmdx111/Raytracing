#include "objs.hh"
#include "scene.hh"
#include <algorithm>

#ifdef VPROGRESS
#include <thread>
#include <SDL2/SDL.h>
#include <tbb/concurrent_queue.h>
tbb::concurrent_bounded_queue<Renderlet*> rlet_queue;
#endif

using namespace std;

int NSAMPLE = 1;
int SHDNSAMPLE = 1;
int GLSNSAMPLE = 1;
int DOFNSAMPLE = 1;
int TILE_SIZE = 32;

int
main(int argc, char** argv)
{
  if (argc < 3) {
    cout << "usage: scene-file output-path" << endl;
    return -1;
  }
  if (argc > 3) {
    NSAMPLE = atoi(argv[3]);
  }
  if (argc > 4) {
    SHDNSAMPLE = atoi(argv[4]);
  }  if (argc > 5) {
    GLSNSAMPLE = atoi(argv[5]);
  }
  if (argc > 6) {
    DOFNSAMPLE = atoi(argv[6]);
  }
  if (argc > 7) {
    TILE_SIZE = atoi(argv[7]);
  }

  Scene scene((string(argv[1])));

#ifdef VPROGRESS

  std::thread t([&]() {
    scene.cam.render();
    scene.cam.save(string(argv[2]));
  });

  t.detach();

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    cerr << "Couldn't initialize SDL: " << SDL_GetError() << endl;
  } else {
    SDL_Window* window = 0;
    SDL_Renderer* renderer = 0;
    int err = SDL_CreateWindowAndRenderer(scene.cam.pw, scene.cam.ph, 
                                          SDL_WINDOW_OPENGL,
                                          &window, &renderer);
    if (err) {
      cerr << "Couldn't create window and renderer: " << SDL_GetError() << endl;
    } else {
      SDL_SetWindowTitle(window, (string("Raytra: ")
                                  + string(argv[1])).c_str());
      SDL_RenderClear(renderer);

      SDL_Event event;

      SDL_Surface* surf = SDL_CreateRGBSurface(0,
                                               scene.cam.pw,
                                               scene.cam.ph,
                                               32,
                                               0xff000000,
                                               0x00ff0000,
                                               0x0000ff00,
                                               0x000000ff);
      if (!surf) {
        cerr << "Couldn't create surface: " << SDL_GetError() << endl;
        goto SDL_CLEAN_UP;
      }

      Renderlet* let;
      bool should_poll = true;
      int pixels = 0;
      while (1) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
          break;
        } else if (event.type == SDL_KEYUP) {
          break;
        }

        if (should_poll) {
          rlet_queue.pop(let);

          if (let->x == -1) {
            should_poll = false;
          }

          ++pixels;
          let->r = max(0.0, min(1.0, let->r));
          let->g = max(0.0, min(1.0, let->g));
          let->b = max(0.0, min(1.0, let->b));

#define RB(t) ((t) == 1.0 ? 255 : (t) * 256.0)
          Uint8* target_pixel = (Uint8*) surf->pixels
          + let->y * surf->pitch + let->x * sizeof(Uint32);
          *((Uint32*)target_pixel) =
          int(RB(let->r)) << 24 |
          int(RB(let->g)) << 16 |
          int(RB(let->b)) << 8 |
          0xff;

          delete let;

          if (pixels % PROGRESS_SAMPLE_RATE == 0) {
            SDL_LockSurface(surf);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,
                                                                surf);
            SDL_UnlockSurface(surf);
            if (!texture) {
              cerr << "Couldn't create texture from surface: " << SDL_GetError()
                   << endl;
              goto SDL_CLEAN_UP;
            }

            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            SDL_DestroyTexture(texture);
          }
        } else {
          SDL_LockSurface(surf);
          SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,
                                                              surf);
          SDL_UnlockSurface(surf);
          if (!texture) {
            cerr << "Couldn't create texture from surface: " << SDL_GetError()
            << endl;
            goto SDL_CLEAN_UP;
          }

          SDL_RenderClear(renderer);
          SDL_RenderCopy(renderer, texture, NULL, NULL);
          SDL_RenderPresent(renderer);
          SDL_DestroyTexture(texture);
        }
      }


      SDL_FreeSurface(surf);
    }

  SDL_CLEAN_UP:
    if (renderer) {
      SDL_DestroyRenderer(renderer);
      renderer = 0;
    }
    if (window) {
      SDL_DestroyWindow(window);
      window = 0;
    }

    SDL_Quit();
  }

#else
  scene.cam.render();
  scene.cam.save(string(argv[2]));
#endif


  return 0;
}

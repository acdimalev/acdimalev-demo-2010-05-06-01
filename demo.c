#include <cairo.h>
#include <math.h>
#include <locale.h>
#include <monetary.h>

#include "SDL.h"
#include "config.h"

#define SCALE (WIDTH < HEIGHT ? WIDTH : HEIGHT)

static double value_inc_rate = 1/16.0;
static int value_inc_amount = 100;

int main(int argc, char **argv) {
  SDL_Surface *sdl_surface;
  Uint32 next_frame;
  cairo_t *cr;

  int running;
  int value;
  Uint32 value_inc_time;

  setlocale(LC_ALL, "");

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
#if FULLSCREEN
  SDL_SetVideoMode(HRES, VRES, 32, SDL_FULLSCREEN);
  SDL_ShowCursor(0);
#else
  SDL_SetVideoMode(HRES, VRES, 32, 0);
#endif

  { /* Initialize Canvas */
    cairo_surface_t *cr_surface;

    sdl_surface = SDL_GetVideoSurface();
    SDL_LockSurface(sdl_surface);
    cr_surface = cairo_image_surface_create_for_data(
        sdl_surface->pixels,
        CAIRO_FORMAT_RGB24,
        sdl_surface->w,
        sdl_surface->h,
        sdl_surface->pitch
      );
    cr = cairo_create(cr_surface);
    cairo_surface_destroy(cr_surface);

    // cartesian
    cairo_translate(cr, HRES/2.0, VRES/2.0);
    cairo_scale(cr, 1, -1);

    // fixed scale
    cairo_scale(cr, SCALE * 1.0 * HRES / WIDTH, SCALE * 1.0 * VRES / HEIGHT);
  }

  { /* Demo Logic */
    Uint32 now = SDL_GetTicks();

    running = 1;
    value = 0;
    value_inc_time = now + value_inc_rate * 1024;
  }

  { /* Initialize Delay */
    Uint32 now = SDL_GetTicks();

    next_frame = now + 1024.0 / FPS;
  }

  while(running) {

    { /* Render Frame */
      cairo_text_extents_t te;
      char string[8];
      double gap;

      // Clear
      cairo_save(cr);
      cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
      cairo_paint(cr);
      cairo_restore(cr);

      cairo_set_source_rgb(cr, 1, 1, 1);

      // Render Value
      strfmon(string, sizeof(string), "%!7.0n", (double)value);
      cairo_select_font_face(cr, "monospace",
        CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
      cairo_set_font_size(cr, 1/16.0);
      cairo_text_extents(cr, string, &te);

      gap = 1/16.0 - -te.y_bearing;

      cairo_move_to(cr,
        1/2.0 * -WIDTH / SCALE + gap,
        1/2.0 * HEIGHT / SCALE - 1/16.0);
      cairo_save(cr);
      cairo_scale(cr, 1, -1);
      cairo_show_text(cr, string);
      cairo_restore(cr);

      cairo_move_to(cr,
        1/2.0 * WIDTH / SCALE - te.x_advance - gap,
        1/2.0 * HEIGHT / SCALE - 1/16.0);
      cairo_save(cr);
      cairo_scale(cr, 1, -1);
      cairo_show_text(cr, string);
      cairo_restore(cr);

      cairo_move_to(cr,
        1/2.0 * -WIDTH / SCALE + gap,
        1/2.0 * -HEIGHT / SCALE + gap);
      cairo_save(cr);
      cairo_scale(cr, 1, -1);
      cairo_show_text(cr, string);
      cairo_restore(cr);

      cairo_move_to(cr,
        1/2.0 * WIDTH / SCALE - te.x_advance - gap,
        1/2.0 * -HEIGHT / SCALE + gap);
      cairo_save(cr);
      cairo_scale(cr, 1, -1);
      cairo_show_text(cr, string);
      cairo_restore(cr);
    }

    { /* Update Display */
      SDL_UnlockSurface(sdl_surface);
      SDL_Flip(sdl_surface);
      SDL_LockSurface(sdl_surface);
    }

    { /* Delay */
      Uint32 now = SDL_GetTicks();

      if (now < next_frame) {
        SDL_Delay(next_frame - now);
        next_frame = next_frame + 1024.0 / FPS;
      } else {
        error(0, 0, "Time Lapse");
        next_frame = now + 1024.0 / FPS;
      }
    }

    { /* Game Logic */
      SDL_Event event;
      Uint32 now = SDL_GetTicks();

      if (now > value_inc_time) {
        value = value + value_inc_amount;
        value_inc_time = value_inc_time + value_inc_rate * 1024;
      }

      while ( SDL_PollEvent(&event) ) {
        if (event.type == SDL_QUIT) {
          running = 0;
        }
        if (event.type == SDL_KEYDOWN) {
          if (event.key.keysym.sym == SDLK_q) {
            running = 0;
          }
        }
      }
    }

  }

  cairo_destroy(cr);
  SDL_Quit();

  return 0;
}

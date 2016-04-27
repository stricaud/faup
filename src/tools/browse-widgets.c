#include <caca.h>

#include "browse-colors.h"

void draw_window(caca_canvas_t *cv, char *window_title, int x, int y, int width, int height)
{
  int i;

  caca_set_color_ansi(cv, WINDOW_CFG, WINDOW_CBG);
  caca_fill_box(cv, x, y, width, height, ' ');
  caca_draw_cp437_box(cv, x, y, width, height);

  if (window_title) {
    caca_printf(cv, x + 2, y, "| %s |", window_title);
  }

}

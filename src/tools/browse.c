/* #include <string.h> */
#include <caca.h>

#include <faup/snapshot.h>
#include <faup/snapshot-file.h>

#include "browse-colors.h"
#include "browse-widgets.h"

int faup_snapshot_browser(char *snapshot_name)
{
  caca_canvas_t *cv; caca_display_t *dp;
  faup_snapshot_t *snapshot = NULL;

  int i;
  int quit = 0;
  
  snapshot = faup_snapshot_read(snapshot_name);
  if (!snapshot) {
    fprintf(stderr, "Cannot read snapshot:%s\n", snapshot_name);
    return -1;
  }
  
  dp = caca_create_display(NULL);
  if(!dp) return 1;
  cv = caca_get_canvas(dp);
  caca_set_display_title(dp, snapshot->name);
  /* caca_set_color_ansi(cv, CACA_BLUE, CACA_YELLOW); */
  /* caca_printf(cv, 0, 0, "w:%d h:%d\n", caca_get_canvas_width(cv), caca_get_canvas_height(cv)); */

  draw_window(cv, snapshot->name, 0, 1, 100, 50);

  for (i = 0; i < snapshot->length; i++) {
    faup_snapshot_item_t *item = snapshot->items[i];

    if (!i) {
      caca_set_color_ansi(cv, TEXT_CFG, TEXT_CBG);
    } else {
      caca_set_color_ansi(cv, TEXT_CBG, TEXT_CFG);
    }
    caca_printf(cv, 2, 3+i, "%s", item->key);
  }

  caca_set_color_ansi(cv, TEXT_CFG, TEXT_CBG);

  caca_refresh_display(dp);

  while (!quit) {
    caca_event_t ev;
    
    while(caca_get_event(dp, CACA_EVENT_ANY, &ev, 0)) {
      if (caca_get_event_type(&ev) & CACA_EVENT_KEY_PRESS) {
	switch(caca_get_event_key_ch(&ev)) {
	case 'q':
	case 'Q':
	case CACA_KEY_ESCAPE:
	  quit = 1;
	  break;
	case CACA_KEY_RETURN:
	  break;
	case CACA_KEY_UP:
	  break;
	case CACA_KEY_DOWN:
	  break;
	}
      }
      
      caca_refresh_display(dp);
    }
  }
  
  caca_free_display(dp);

  faup_snapshot_free(snapshot);
  
  return 0;
}

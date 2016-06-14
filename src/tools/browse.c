#include <faup/snapshot.h>
#include <faup/snapshot-file.h>

#include <gtcaca/main.h>
#include <gtcaca/window.h>
#include <gtcaca/textlist.h>
#include <gtcaca/button.h>
#include <gtcaca/label.h>

faup_snapshot_t *snapshot = NULL;
gtcaca_window_widget_t *win_values = NULL;


int item_textlist_key_press(gtcaca_textlist_widget_t *widget, int key, void *userdata)
{
  gtcaca_application_widget_t *app = (gtcaca_application_widget_t *)userdata;
  struct htable_iter iter;
  char *value;
  int i;
  gtcaca_textlist_widget_t *textlist;
  faup_snapshot_value_count_t *vc;

  
  switch(key) {
  case CACA_KEY_RETURN:
    if (widget->parent->width > (app->width / 2)) {
      widget->parent->width /= 2;
      win_values = gtcaca_window_new((gtcaca_widget_t *)app, "Values", app->x + (app->width /2), app->y, app->width/2, app->height);
    }

    value = gtcaca_textlist_get_text_selected(widget);

  /*   for (i = 0; i < snapshot->length; i++) { */
  /*     faup_snapshot_item_t *item = snapshot->items[i]; */
  /*     if (!strcmp(item->key, value)) { */
  /* 	textlist = gtcaca_textlist_new((gtcaca_widget_t *)win_values, 2, 2); */

  /* 	  vc = htable_first(&item->values, &iter); */
  /* 	  while (vc) { */
  /* 	    gtcaca_textlist_append(textlist, vc->value);	     */
  /* 	    vc = htable_next(&item->values, &iter); */
  /* 	  } */
  /* /\* } *\/ */

  /*     } */
    }
    
    break;
  }
}

int faup_snapshot_browser(char *snapshot_name)
{
  gtcaca_application_widget_t *app;
  gtcaca_window_widget_t *win;
  gtcaca_textlist_widget_t *textlist;
  gtcaca_button_widget_t *button;
  gtcaca_label_widget_t *label;

  int i;
  
  snapshot = faup_snapshot_read(snapshot_name);
  if (!snapshot) {
    fprintf(stderr, "Cannot read snapshot:%s\n", snapshot_name);
    return -1;
  }

  gtcaca_init(NULL, NULL);

  app = gtcaca_application_new(snapshot->name);

  win = gtcaca_window_new((gtcaca_widget_t *)app, "Items", app->x, app->y, app->width, app->height);

  //  gtcaca_widget_printall();
  
  textlist = gtcaca_textlist_new((gtcaca_widget_t *)win, 2, 2);

  for (i = 0; i < snapshot->length; i++) {
    faup_snapshot_item_t *item = snapshot->items[i];
    gtcaca_textlist_append(textlist, item->key);
  }

  gtcaca_textlist_key_cb_register(textlist, item_textlist_key_press, (void *)app);

  gtcaca_main();
  
  faup_snapshot_free(snapshot);
  
  return 0;
}

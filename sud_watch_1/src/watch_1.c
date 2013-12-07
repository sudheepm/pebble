#include "pebble.h"

Window *window;
TextLayer *text_date_layer;
TextLayer *text_week_layer;
TextLayer *text_time_layer;
TextLayer *text_mldate_layer;
Layer *line_layer;
Layer *line_layer1;

static BitmapLayer *icon_layer;
static GBitmap *icon_bitmap = NULL;

static BitmapLayer *icon_layer1;
static GBitmap *icon_bitmap1 = NULL;

static uint32_t WEATHER_ICONS[] = {
  RESOURCE_ID_IMAGE_CLOUD
};


static uint32_t WEEK_ICONS[] = {
  RESOURCE_ID_SUN,
  RESOURCE_ID_MON,
  RESOURCE_ID_TUE,
  RESOURCE_ID_WED,
  RESOURCE_ID_THU,
  RESOURCE_ID_FRI,
  RESOURCE_ID_SAT
};

static uint32_t MONTH_ICONS[] = {
  RESOURCE_ID_01,
  RESOURCE_ID_02,
  RESOURCE_ID_03,
  RESOURCE_ID_04,
  RESOURCE_ID_05,
  RESOURCE_ID_06,
  RESOURCE_ID_07,
  RESOURCE_ID_08,
  RESOURCE_ID_09,
  RESOURCE_ID_10,
  RESOURCE_ID_11,
  RESOURCE_ID_12
};

void line_layer_update_callback(Layer *layer, GContext* ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // Need to be static because they're used by the system later.
  static char time_text[] = "00:00";
  static char week_text[] = "Xxxxxxxxxx";
  static char date_text[] = "  Xxx 00, 0000";
  static char mldate_text[] = "00,0000";

  char *time_format;
  float dayrep=0;
  time_t epoch_time;
  struct tm *tm_p;
  //int ml_diff = 2013-1189
  int b_ml_yr = 1189;
  int mnth=0;
  int mnday;
  epoch_time = time( NULL );
  tm_p = localtime( &epoch_time );
//  dayrep=0;

  mnday=1;
  dayrep = (1900+tm_p->tm_year)*1000 + (1+tm_p->tm_mon)*100 + (tm_p->tm_mday);
  if (dayrep<=20131215)
  {  
    mnth=3;
    if (tm_p->tm_mon==10)
      {
        mnday = tm_p->tm_mday-15;
       }
    else
      {
        mnday = tm_p->tm_mday+15;   
      } 
  }
 else if (dayrep<=20140113)
  {  
    mnth=4;
    if (tm_p->tm_mon==11)
      {
        mnday = tm_p->tm_mday-15;
       }
    else
      {
        mnday = tm_p->tm_mday+16;    
      } 
  }


  snprintf(mldate_text,sizeof(mldate_text),"%d,%d",mnday,b_ml_yr);
 /* if (tm_p->tm_min==0)
  {
    week_int=2;
  }
  else if (tm_p->tm_min % 2 == 0)
  {
   week_int=1;
  }
  else
  {
   week_int=0;
  }
*/



  // TODO: Only update the date when it's changed.
  strftime(date_text, sizeof(date_text), "  %b %e, %Y", tick_time);
  text_layer_set_text(text_date_layer, date_text);

  strftime(week_text, sizeof(week_text), "  %A", tick_time);
  text_layer_set_text(text_week_layer, week_text);

  text_layer_set_text(text_mldate_layer, mldate_text);

  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  strftime(time_text, sizeof(time_text), time_format, tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(text_time_layer, time_text);

  if (icon_bitmap) {
        gbitmap_destroy(icon_bitmap);
      }

   icon_bitmap = gbitmap_create_with_resource(MONTH_ICONS[mnth]);
   bitmap_layer_set_bitmap(icon_layer, icon_bitmap);

  if (icon_bitmap1) {
        gbitmap_destroy(icon_bitmap1);
      }

   icon_bitmap1 = gbitmap_create_with_resource(WEEK_ICONS[tm_p->tm_wday]);
   bitmap_layer_set_bitmap(icon_layer1, icon_bitmap1);

}

void handle_deinit(void) {
  tick_timer_service_unsubscribe();
}

void handle_init(void) {
  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer(window);

  icon_layer = bitmap_layer_create(GRect(8, 32, 57, 20));
  layer_add_child(window_layer, bitmap_layer_get_layer(icon_layer));

  icon_layer1 = bitmap_layer_create(GRect(10, 12, 66, 20));
  layer_add_child(window_layer, bitmap_layer_get_layer(icon_layer1));


  text_week_layer = text_layer_create(GRect(2, 56, 144-8, 168-58));
  text_layer_set_text_color(text_week_layer, GColorWhite);
  text_layer_set_background_color(text_week_layer, GColorClear);
  text_layer_set_font(text_week_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(window_layer, text_layer_get_layer(text_week_layer));


  text_date_layer = text_layer_create(GRect(2, 78, 144-8, 168-68));
  text_layer_set_text_color(text_date_layer, GColorWhite);
  text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_font(text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(window_layer, text_layer_get_layer(text_date_layer));

  text_mldate_layer = text_layer_create(GRect(66, 29, 144-71, 168-32));
  text_layer_set_text_color(text_mldate_layer, GColorWhite);
  text_layer_set_background_color(text_mldate_layer, GColorClear);
  text_layer_set_font(text_mldate_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(window_layer, text_layer_get_layer(text_mldate_layer));



  text_time_layer = text_layer_create(GRect(7, 102, 144-7, 168-102));
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

  GRect line_frame = GRect(8, 107, 139, 2);
  line_layer = layer_create(line_frame);
  layer_set_update_proc(line_layer, line_layer_update_callback);
  layer_add_child(window_layer, line_layer);

  GRect line_frame1 = GRect(8, 56, 139, 2);
  line_layer1 = layer_create(line_frame1);
  layer_set_update_proc(line_layer1, line_layer_update_callback);
  layer_add_child(window_layer, line_layer1);


  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  // TODO: Update display here to avoid blank display on launch?
}


int main(void) {
  handle_init();

  app_event_loop();
  
  handle_deinit();
}

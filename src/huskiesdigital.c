#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID { 0xB3, 0xBF, 0x49, 0xE3, 0x8B, 0xAA, 0x42, 0xB5, 0x83, 0xE4, 0x5D, 0x79, 0xC4, 0xA5, 0x3A, 0x6C }
PBL_APP_INFO(MY_UUID,
             "Huskies Digital", "ZissCo",
             1, 3, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

BmpContainer background_image;
			 
Window window;

const int BIG_DIGIT_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_NUM_0,
  RESOURCE_ID_IMAGE_NUM_1,
  RESOURCE_ID_IMAGE_NUM_2,
  RESOURCE_ID_IMAGE_NUM_3,
  RESOURCE_ID_IMAGE_NUM_4,
  RESOURCE_ID_IMAGE_NUM_5,
  RESOURCE_ID_IMAGE_NUM_6,
  RESOURCE_ID_IMAGE_NUM_7,
  RESOURCE_ID_IMAGE_NUM_8,
  RESOURCE_ID_IMAGE_NUM_9
};


#define TOTAL_TIME_DIGITS 4
BmpContainer time_digits_images[TOTAL_TIME_DIGITS];

unsigned short get_display_hour(unsigned short hour) {

  if (clock_is_24h_style()) {
    return hour;
  }

  unsigned short display_hour = hour % 12;

  // Converts "0" to "12"
  return display_hour ? display_hour : 12;

}

void set_container_image(BmpContainer *bmp_container, const int resource_id, GPoint origin) {

  layer_remove_from_parent(&bmp_container->layer.layer);
  bmp_deinit_container(bmp_container);

  bmp_init_container(resource_id, bmp_container);

  GRect frame = layer_get_frame(&bmp_container->layer.layer);
  frame.origin.x = origin.x;
  frame.origin.y = origin.y;
  layer_set_frame(&bmp_container->layer.layer, frame);

  layer_add_child(&window.layer, &bmp_container->layer.layer);
}

void update_display(PblTm *current_time) {

  unsigned short display_hour = get_display_hour(current_time->tm_hour);

  set_container_image(&time_digits_images[0], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour/10], GPoint(49, 151));
  set_container_image(&time_digits_images[1], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour%10], GPoint(61, 151));

  set_container_image(&time_digits_images[2], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min/10], GPoint(76, 151));
  set_container_image(&time_digits_images[3], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min%10], GPoint(87, 151));

/*
  if (!clock_is_24h_style()) {
    if (current_time->tm_hour >= 12) {
      set_container_image(&time_format_image, RESOURCE_ID_IMAGE_PM_MODE, GPoint(17, 68));
    } else {
      set_container_image(&time_format_image, RESOURCE_ID_IMAGE_AM_MODE, GPoint(17, 68));
    }

    if (display_hour/10 == 0) {
      layer_remove_from_parent(&time_digits_images[0].layer.layer);
      bmp_deinit_container(&time_digits_images[0]);
    }
  }
*/
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)ctx;

  update_display(t->tick_time);
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Huskies Digital");
  window_stack_push(&window, true);
  window_set_background_color(&window, GColorBlack);
  
  resource_init_current_app(&APP_RESOURCES);

  bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND, &background_image);
  layer_add_child(&window.layer, &background_image.layer.layer);

  // Avoids a blank screen on watch start.
  PblTm tick_time;

  get_time(&tick_time);
  update_display(&tick_time);

}

void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  bmp_deinit_container(&background_image);

  for (int i = 0; i < TOTAL_TIME_DIGITS; i++) {
    bmp_deinit_container(&time_digits_images[i]);
  }

}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
	.deinit_handler = &handle_deinit,

    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };
  app_event_loop(params, &handlers);
}

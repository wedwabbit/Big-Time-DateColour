#include <pebble.h>
#include "gbitmap_color_palette_manipulator.h"

//#define SHOW_APP_LOGS

static Window *s_main_window;
static AppTimer *app_timer;

// Configuration parameters.
#define KEY_TIMEBACKGROUND 0
#define KEY_TIMEFOREGROUND 1
#define KEY_DATEBACKGROUND 2
#define KEY_DATEFOREGROUND 3
#define KEY_DATETIMEOUT 4
#define KEY_DATEENABLED 5
#define KEY_FONTTYPE 6

// Slot on-screen layout:
//     0 1
//     2 3
#define TOTAL_IMAGE_SLOTS 4
#define NUMBER_OF_FONTS 7 // Number of fonts available to the user.
#define IMAGES_IN_SET 10 // Number of images in each font set.

// These images are 72 x 84 pixels (i.e. a quarter of the display),
// black and white with the digit character centered in the image.
const int IMAGE_RESOURCE_IDS[NUMBER_OF_FONTS * IMAGES_IN_SET] = {
    RESOURCE_ID_IMAGE_FONT1_0, RESOURCE_ID_IMAGE_FONT1_1, RESOURCE_ID_IMAGE_FONT1_2,
    RESOURCE_ID_IMAGE_FONT1_3, RESOURCE_ID_IMAGE_FONT1_4, RESOURCE_ID_IMAGE_FONT1_5,
    RESOURCE_ID_IMAGE_FONT1_6, RESOURCE_ID_IMAGE_FONT1_7, RESOURCE_ID_IMAGE_FONT1_8,
    RESOURCE_ID_IMAGE_FONT1_9,
    RESOURCE_ID_IMAGE_FONT2_0, RESOURCE_ID_IMAGE_FONT2_1, RESOURCE_ID_IMAGE_FONT2_2,
    RESOURCE_ID_IMAGE_FONT2_3, RESOURCE_ID_IMAGE_FONT2_4, RESOURCE_ID_IMAGE_FONT2_5,
    RESOURCE_ID_IMAGE_FONT2_6, RESOURCE_ID_IMAGE_FONT2_7, RESOURCE_ID_IMAGE_FONT2_8,
    RESOURCE_ID_IMAGE_FONT2_9,
    RESOURCE_ID_IMAGE_FONT3_0, RESOURCE_ID_IMAGE_FONT3_1, RESOURCE_ID_IMAGE_FONT3_2,
    RESOURCE_ID_IMAGE_FONT3_3, RESOURCE_ID_IMAGE_FONT3_4, RESOURCE_ID_IMAGE_FONT3_5,
    RESOURCE_ID_IMAGE_FONT3_6, RESOURCE_ID_IMAGE_FONT3_7, RESOURCE_ID_IMAGE_FONT3_8,
    RESOURCE_ID_IMAGE_FONT3_9,
    RESOURCE_ID_IMAGE_FONT4_0, RESOURCE_ID_IMAGE_FONT4_1, RESOURCE_ID_IMAGE_FONT4_2,
    RESOURCE_ID_IMAGE_FONT4_3, RESOURCE_ID_IMAGE_FONT4_4, RESOURCE_ID_IMAGE_FONT4_5,
    RESOURCE_ID_IMAGE_FONT4_6, RESOURCE_ID_IMAGE_FONT4_7, RESOURCE_ID_IMAGE_FONT4_8,
    RESOURCE_ID_IMAGE_FONT4_9,
    RESOURCE_ID_IMAGE_FONT5_0, RESOURCE_ID_IMAGE_FONT5_1, RESOURCE_ID_IMAGE_FONT5_2,
    RESOURCE_ID_IMAGE_FONT5_3, RESOURCE_ID_IMAGE_FONT5_4, RESOURCE_ID_IMAGE_FONT5_5,
    RESOURCE_ID_IMAGE_FONT5_6, RESOURCE_ID_IMAGE_FONT5_7, RESOURCE_ID_IMAGE_FONT5_8,
    RESOURCE_ID_IMAGE_FONT5_9,
    RESOURCE_ID_IMAGE_FONT6_0, RESOURCE_ID_IMAGE_FONT6_1, RESOURCE_ID_IMAGE_FONT6_2,
    RESOURCE_ID_IMAGE_FONT6_3, RESOURCE_ID_IMAGE_FONT6_4, RESOURCE_ID_IMAGE_FONT6_5,
    RESOURCE_ID_IMAGE_FONT6_6, RESOURCE_ID_IMAGE_FONT6_7, RESOURCE_ID_IMAGE_FONT6_8,
    RESOURCE_ID_IMAGE_FONT6_9,
    RESOURCE_ID_IMAGE_FONT7_0, RESOURCE_ID_IMAGE_FONT7_1, RESOURCE_ID_IMAGE_FONT7_2,
    RESOURCE_ID_IMAGE_FONT7_3, RESOURCE_ID_IMAGE_FONT7_4, RESOURCE_ID_IMAGE_FONT7_5,
    RESOURCE_ID_IMAGE_FONT7_6, RESOURCE_ID_IMAGE_FONT7_7, RESOURCE_ID_IMAGE_FONT7_8,
    RESOURCE_ID_IMAGE_FONT7_9
};

static GBitmap *s_images[TOTAL_IMAGE_SLOTS + 1];
static BitmapLayer *s_image_layers[TOTAL_IMAGE_SLOTS];

#define EMPTY_SLOT -1

// The state is either "empty" or the digit of the image currently in the slot.
static int s_image_slot_state[TOTAL_IMAGE_SLOTS] = {
    EMPTY_SLOT, EMPTY_SLOT, EMPTY_SLOT, EMPTY_SLOT
};

// Loads the digit image from the application's resources and
// displays it on-screen in the correct location.
// Each slot is a quarter of the screen.
static void load_digit_image_into_slot(int slot_number, int digit_value, int foreground_colour, int background_colour) {

    int fonttype; // Font used to display digits.

    GRect bounds = layer_get_bounds(window_get_root_layer(s_main_window));

    // Get the chosen font type.
    if(!(fonttype = persist_read_int(KEY_FONTTYPE))) {
        // There was no saved preference so set a default. 
        fonttype = 0;
    }

    s_image_slot_state[slot_number] = digit_value;
    s_images[slot_number] = gbitmap_create_with_resource(IMAGE_RESOURCE_IDS[digit_value + fonttype * IMAGES_IN_SET]);
#ifdef SHOW_APP_LOGS
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%d, %d", foreground_colour, background_colour);
#endif

    // If requested background is white then replace the existing white digit with a red placeholder, replace
    // the black background with white, and then replace the red placeholder with the requested colour.
    // Note: use the gbitmap_fill_all_except first to get rid of any stray colours.
    if(gcolor_equal(GColorFromHEX(background_colour), GColorWhite)) {
#ifdef SHOW_APP_LOGS
        APP_LOG(APP_LOG_LEVEL_DEBUG, "White background.");
#endif
        gbitmap_fill_all_except(GColorBlack, GColorRed, false, s_images[slot_number], NULL);
        replace_gbitmap_color(GColorBlack, GColorWhite, s_images[slot_number], NULL);
        replace_gbitmap_color(GColorRed, GColorFromHEX(foreground_colour), s_images[slot_number], NULL);
    }
    // Requested foreground is black then replace the existing black background with a red placeholder, replace
    // the white digit with black, and then replace the red placeholder with the requested colour.
    else if(gcolor_equal(GColorFromHEX(foreground_colour), GColorBlack)) {
#ifdef SHOW_APP_LOGS
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Black foreground.");
#endif
        gbitmap_fill_all_except(GColorWhite, GColorRed, false, s_images[slot_number], NULL);
        replace_gbitmap_color(GColorWhite, GColorBlack, s_images[slot_number], NULL);
        replace_gbitmap_color(GColorRed, GColorFromHEX(background_colour), s_images[slot_number], NULL);
    }
    // Replace the black background with the requested background and the white digit with the requested foreground.
    else {
#ifdef SHOW_APP_LOGS
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Other background/foreground.");
#endif
        gbitmap_fill_all_except(GColorWhite, GColorFromHEX(background_colour), false, s_images[slot_number], NULL);
        replace_gbitmap_color(GColorWhite, GColorFromHEX(foreground_colour), s_images[slot_number], NULL);
    }

    GRect tile_bounds = gbitmap_get_bounds(s_images[slot_number]);

    const int x_offset = (bounds.size.w - (2 * tile_bounds.size.w)) / 2;
    const int y_offset = (bounds.size.h - (2 * tile_bounds.size.h)) / 2;
    BitmapLayer *bitmap_layer = bitmap_layer_create(
            GRect(x_offset + ((slot_number % 2) * tile_bounds.size.w),
                y_offset + ((slot_number / 2) * tile_bounds.size.h),
                tile_bounds.size.w, tile_bounds.size.h));
    s_image_layers[slot_number] = bitmap_layer;
    bitmap_layer_set_bitmap(bitmap_layer, s_images[slot_number]);

    Layer *window_layer = window_get_root_layer(s_main_window);
    layer_add_child(window_layer, bitmap_layer_get_layer(bitmap_layer));
}

static void unload_digit_image_from_slot(int slot_number) {
    if (s_image_slot_state[slot_number] != EMPTY_SLOT) {
        layer_remove_from_parent(bitmap_layer_get_layer(s_image_layers[slot_number]));
        bitmap_layer_destroy(s_image_layers[slot_number]);
        gbitmap_destroy(s_images[slot_number]);

        // This is now an empty slot
        s_image_slot_state[slot_number] = EMPTY_SLOT;
    }
}

static void display_value(unsigned short value, unsigned short row_number, bool show_first_leading_zero, bool isdate) {

    int foreground_colour;
    int background_colour;

    // Get the colours of the displayed values.
    if(isdate) {
        if(persist_read_int(KEY_DATEBACKGROUND))
            background_colour = persist_read_int(KEY_DATEBACKGROUND);
        else
            background_colour = 0x000000; // There was no saved preference so set a default of black.
        if(persist_read_int(KEY_DATEFOREGROUND))
            foreground_colour = persist_read_int(KEY_DATEFOREGROUND);
        else
            foreground_colour = 0x000000; // There was no saved preference so set a default black.
    }
    else {
        if(persist_read_int(KEY_TIMEBACKGROUND))
            background_colour = persist_read_int(KEY_TIMEBACKGROUND);
        else
            background_colour = 0x000000; // There was no saved preference so set a default of black.
        if(persist_read_int(KEY_TIMEFOREGROUND))
            foreground_colour = persist_read_int(KEY_TIMEFOREGROUND);
        else
            foreground_colour = 0x000000; // There was no saved preference so set a default of black.
    }

    // If the foreground and background colours are the same then invert the background colour.
    if(foreground_colour == background_colour)
        background_colour = ~background_colour; 

#ifdef SHOW_APP_LOGS
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%d, %d", foreground_colour, background_colour);
#endif

    value = value % 100; // Maximum of two digits per row.

    // Column order is: | Column 0 | Column 1 |
    // (We process the columns in reverse order because that makes
    // extracting the digits from the value easier.)
    for (int column_number = 1; column_number >= 0; column_number--) {
        int slot_number = (row_number * 2) + column_number;
        unload_digit_image_from_slot(slot_number);
        if (!((value == 0) && (column_number == 0) && !show_first_leading_zero)) {
            load_digit_image_into_slot(slot_number, value % 10, foreground_colour, background_colour);
        }
        else {
            // Load a blank tile.
            load_digit_image_into_slot(slot_number, 0, background_colour, background_colour);
        }
        value = value / 10;
    }
}

static unsigned short get_display_hour(unsigned short hour) {
    if (clock_is_24h_style()) {
        return hour;
    }
    // Converts "0" to "12"
    unsigned short display_hour = hour % 12;
    return display_hour ? display_hour : 12;
}

static void display_time(struct tm *disp_time) {
    display_value(get_display_hour(disp_time->tm_hour), 0, false, false);
    display_value(disp_time->tm_min, 1, true, false);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
#ifdef SHOW_APP_LOGS
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%d:%d", tick_time->tm_hour, tick_time->tm_min);
#endif
    display_time(tick_time);
}

static void auto_redisplay_time(void *data) {
    time_t now = time(NULL);
    struct tm tick_time = *localtime(&now); 
    app_timer = NULL;
    display_time(&tick_time);
}

static void display_date(struct tm *disp_date) {
    int datetimeout = 1;

    display_value(disp_date->tm_mday, 0, true, true);
    display_value(disp_date->tm_mon+1, 1, true, true);
    if (app_timer != NULL) {
        app_timer_cancel(app_timer);
    }

    // Get the requested timeout value if it exists.
    if(!(datetimeout = persist_read_int(KEY_DATETIMEOUT))) {
        // There was no saved preference so set a default of 1 second.
        datetimeout = 1;
    }

#ifdef SHOW_APP_LOGS
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%d, %d", datetimeout, datetimeout * 1000);
#endif

    app_timer = app_timer_register(datetimeout * 1000, auto_redisplay_time, NULL);
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
    // Date requested so get the current date.
    time_t now = time(NULL);
    struct tm tick_date = *localtime(&now); 
#ifdef SHOW_APP_LOGS
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%d/%d", tick_date.tm_mday, tick_date.tm_mon+1);
#endif
    app_timer = NULL;
    display_date(&tick_date);
}

static void main_window_load(Window *window) {
    time_t now = time(NULL);
    struct tm tick_time = *localtime(&now);  // First run through so set display_time.
    display_time(&tick_time);
}

static void main_window_unload(Window *window) {
    for (int i = 0; i < TOTAL_IMAGE_SLOTS; i++) {
        unload_digit_image_from_slot(i);
    }
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
    Tuple *timebackground_t = dict_find(iter, KEY_TIMEBACKGROUND);
    Tuple *timeforeground_t = dict_find(iter, KEY_TIMEFOREGROUND);
    Tuple *datebackground_t = dict_find(iter, KEY_DATEBACKGROUND);
    Tuple *dateforeground_t = dict_find(iter, KEY_DATEFOREGROUND);
    Tuple *datetimeout_t = dict_find(iter, KEY_DATETIMEOUT);
    Tuple *dateenabled_t = dict_find(iter, KEY_DATEENABLED);
    Tuple *fonttype_t = dict_find(iter, KEY_FONTTYPE);

    int colour;
    int dateenabled;
    int fonttype;

    // Retrieve the values passed in and store them in persistent storage.
    if (timebackground_t) {
        colour = timebackground_t->value->int32;
        persist_write_int(KEY_TIMEBACKGROUND, colour);
#ifdef SHOW_APP_LOGS
        APP_LOG(APP_LOG_LEVEL_DEBUG, "TBG inbox: %ld, %ld", timebackground_t->value->int32, persist_read_int(KEY_TIMEBACKGROUND));
#endif
    }

    if (timeforeground_t) {
        colour = timeforeground_t->value->int32;
        persist_write_int(KEY_TIMEFOREGROUND, colour);
#ifdef SHOW_APP_LOGS
        APP_LOG(APP_LOG_LEVEL_DEBUG, "TFG inbox: %ld, %ld", timeforeground_t->value->int32, persist_read_int(KEY_TIMEFOREGROUND));
#endif
    }

    if (datebackground_t) {
        colour = datebackground_t->value->int32;
        persist_write_int(KEY_DATEBACKGROUND, colour);
#ifdef SHOW_APP_LOGS
        APP_LOG(APP_LOG_LEVEL_DEBUG, "DBG inbox: %ld, %ld", datebackground_t->value->int32, persist_read_int(KEY_DATEBACKGROUND));
#endif
    }

    if (dateforeground_t) {
        colour = dateforeground_t->value->int32;
        persist_write_int(KEY_DATEFOREGROUND, colour);
#ifdef SHOW_APP_LOGS
        APP_LOG(APP_LOG_LEVEL_DEBUG, "DFG inbox: %ld, %ld", dateforeground_t->value->int32, persist_read_int(KEY_DATEFOREGROUND));
#endif
    }

    if (datetimeout_t) {
        colour = datetimeout_t->value->int32;
        persist_write_int(KEY_DATETIMEOUT, colour);
#ifdef SHOW_APP_LOGS
        APP_LOG(APP_LOG_LEVEL_DEBUG, "DTO inbox: %ld, %ld", datetimeout_t->value->int32, persist_read_int(KEY_DATETIMEOUT));
#endif
    }

    if (dateenabled_t) {
        dateenabled = dateenabled_t->value->int8;
        persist_write_int(KEY_DATEENABLED, dateenabled);
#ifdef SHOW_APP_LOGS
        APP_LOG(APP_LOG_LEVEL_DEBUG, "DE inbox: %zu, %ld", dateenabled_t->value->int8, persist_read_int(KEY_DATEENABLED));
#endif
        // If the user has enabled date then subscribe to the tap service (will have no affect if already subscribed).
        // If disabled then unsubscribe from the service (again no affect if already unsubscribed).
        if(!persist_read_int(KEY_DATEENABLED))
        {
#ifdef SHOW_APP_LOGS
            APP_LOG(APP_LOG_LEVEL_DEBUG, "accel_tap_service_subscribe");
#endif
            accel_tap_service_subscribe(tap_handler);
        }
        else {
#ifdef SHOW_APP_LOGS
            APP_LOG(APP_LOG_LEVEL_DEBUG, "accel_tap_service_unsubscribe");
#endif
            accel_tap_service_unsubscribe();
        }

    }

    if (fonttype_t) {
        fonttype = fonttype_t->value->int32;
        persist_write_int(KEY_FONTTYPE, fonttype);
#ifdef SHOW_APP_LOGS
        APP_LOG(APP_LOG_LEVEL_DEBUG, "FT inbox: %ld, %ld", fonttype_t->value->int32, persist_read_int(KEY_FONTTYPE));
#endif
    }

    time_t now = time(NULL);
    struct tm tick_time = *localtime(&now);
    display_time(&tick_time);
}

static void init() {
    s_main_window = window_create();
    window_set_background_color(s_main_window, GColorClear);
    window_set_window_handlers(s_main_window, (WindowHandlers) {
            .load = main_window_load,
            .unload = main_window_unload,
            });
    window_stack_push(s_main_window, true);
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

    // If the option to show date when tapped is enabled then subscribe to the tap service. Default if no option
    // chosen is to enable date.
    // Note: persist_read_bool will return false if the key is not set. Since we want to display the date by default (false)
    // and when the users chooses to (true) we need to use the following scheme: enabled - false, not set - false, disabled - true.
    if(!persist_read_bool(KEY_DATEENABLED)) {
        accel_tap_service_subscribe(tap_handler);
    }

    app_message_register_inbox_received(inbox_received_handler);
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
    accel_tap_service_unsubscribe();
    tick_timer_service_unsubscribe();
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}

/* ===========================================================================
 
 Copyright (c) 2013 Edward Patel
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 
 =========================================================================== */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "resource_ids.auto.h"

#include "util.h"
#include "http.h"
#include "httpcapture.h"
#include "time_layer.h"
#include "icons.h"

#define MAKE_SCREENSHOT 0
	
PBL_APP_INFO(HTTP_UUID,
             "Antonio All", "Antonio Asaro",
             1, 1,
             RESOURCE_ID_WATCH_MENU_ICON,
             APP_INFO_WATCH_FACE);   // Based off of "pbl-index" by "Edward Patel"

// Weather info --> needs location and units!!
// Stock List is in the form ?stock1=name1&stock2= --> must have 2 names!!
#define WEATHER_UNITS "°C"
#define WEATHER_LOC_UNITS "http://antonioasaro.site50.net/weather.php/?location=Toronto,Canada&units=metric"
#define STOCK_QUOTE_LIST  "http://antonioasaro.site50.net/stocks.php/?stock1=AMD&stock2=INTC"     

#define NUM_LINES 5
#define PBLINDEX_STOCK_COOKIE   9997
#define PBLINDEX_WEATHER_COOKIE 9777

Window window;
TimeLayer time_layer;
TextLayer textLayer[3][NUM_LINES];
static int initial_minute;
static bool first_quotes = true;
static bool first_weather = true;
static bool bmp_present = false;
static BmpContainer weather_icon; 
GFont font_hour;  
GFont font_date;

void set_display_fail(char *text) {
#ifdef _DEBUG
	text_layer_set_text(&textLayer[0][0], "Failed");
    text_layer_set_text(&textLayer[0][1], text);
	for (int i=0; i<NUM_LINES; i++) {
		if (i>1) text_layer_set_text(&textLayer[0][i], "");
        text_layer_set_text(&textLayer[1][i], "");
        text_layer_set_text(&textLayer[2][i], "");
	}
#endif
}

char *ftoa(int i, bool j) {
  	static char buf[8];
	strcpy(buf, "");
	if (!j) strcat(buf, "$");
	if (i<0) { i = -i; strcat(buf, "-"); }
	if (!j) strcat(buf, itoa(i/100)); else strcat(buf, itoa(i/10));
	strcat(buf, ".");
	if (!j) strcat(buf, itoa(i%100)); else strcat(buf, itoa(i%10));
	if (j) strcat(buf, "%");
    return(buf);
}

void set_weather_icon(BmpContainer *container, char *icon) {
	if(bmp_present) {
		layer_remove_from_parent(&container->layer.layer);
		bmp_deinit_container(container);
		bmp_present = false;
	}

    int resource_id = RESOURCE_ID_ICON_WIND;  // RAIN;
	if (strcmp(icon, "01d") == 0) resource_id = RESOURCE_ID_ICON_CLEAR_DAY;
	if (strcmp(icon, "01n") == 0) resource_id = RESOURCE_ID_ICON_CLEAR_NIGHT;
	if (strcmp(icon, "02d") == 0) resource_id = RESOURCE_ID_ICON_CLOUDY;
	if (strcmp(icon, "02n") == 0) resource_id = RESOURCE_ID_ICON_CLOUDY;
	if (strcmp(icon, "09d") == 0) resource_id = RESOURCE_ID_ICON_RAIN;
	if (strcmp(icon, "09n") == 0) resource_id = RESOURCE_ID_ICON_RAIN;

	bmp_init_container(resource_id, container);
	layer_set_frame(&container->layer.layer, GRect(100, 50, 60, 60));
	layer_add_child(&window.layer, &container->layer.layer);
	bmp_present = true;
}

void request_weather() {
    DictionaryIterator *body;
    if (http_out_get(WEATHER_LOC_UNITS, false, PBLINDEX_WEATHER_COOKIE, &body) != HTTP_OK ||
        http_out_send() != HTTP_OK) {
        set_display_fail("WT fail()");
    }
}

void request_quotes() {
    DictionaryIterator *body;
    if (http_out_get(STOCK_QUOTE_LIST, false, PBLINDEX_STOCK_COOKIE, &body) != HTTP_OK ||
        http_out_send() != HTTP_OK) {
        set_display_fail("QT fail()");
    }
}

void failed(int32_t cookie, int http_status, void *ctx) {
    if (cookie == 0 ||
		cookie == PBLINDEX_STOCK_COOKIE ||
		cookie == PBLINDEX_WEATHER_COOKIE) {
        set_display_fail("BT fail()");
    }
}

void success(int32_t cookie, int http_status, DictionaryIterator *dict, void *ctx) {
	if (cookie == PBLINDEX_WEATHER_COOKIE) {
		static char conditions[3][16];
		strcpy(conditions[2], "NULL");
    	for (int i=0; i<3; i++) {
			Tuple *weather = dict_find(dict,  i+1);
			if (weather) {
				if (i==0) strcpy(conditions[i], weather->value->cstring); 
				if (i==1) strcpy(conditions[i], itoa(weather->value->int32));	
				if (i==1) strcat(conditions[i], WEATHER_UNITS); 
				if (i==2) strcpy(conditions[i], weather->value->cstring); 
				if (i<=1) text_layer_set_text(&textLayer[i*2][2], conditions[i]);
		 		if (i==2) set_weather_icon(&weather_icon, conditions[2]);
		   }
	    }
	}
	
	if (cookie == PBLINDEX_STOCK_COOKIE) {
		static char stock1[3][16];  
		static char stock2[3][16];  
    	for (int i=0; i<3+3; i++) {
			Tuple *quotes = dict_find(dict,  i+1);
			if (quotes) {
				if (i==0) strcpy(stock1[i-0], quotes->value->cstring); 
				if (i==1) strcpy(stock1[i-0], ftoa(quotes->value->int32, 0));	
				if (i==2) strcpy(stock1[i-0], ftoa(quotes->value->int32, 1));	
				if (i==3) strcpy(stock2[i-3], quotes->value->cstring); 
				if (i==4) strcpy(stock2[i-3], ftoa(quotes->value->int32, 0));	
				if (i==5) strcpy(stock2[i-3], ftoa(quotes->value->int32, 1));	
				if (i<3) text_layer_set_text(&textLayer[i-0][3], stock1[i-0]);
				if (i>2) text_layer_set_text(&textLayer[i-3][4], stock2[i-3]);
		   }
	    }
	}
    // light_enable_interaction();
}

void reconnect(void *ctx) {
	first_weather = true;
	first_quotes  = true;
}
 
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t)
{
    static char date_text[] = "XXX 00/00";
    static char hour_text[] = "00";
    static char minute_text[] = ":00";

	(void)ctx;  /* prevent "unused parameter" warning */

    if (t->units_changed & DAY_UNIT) {		
	    string_format_time(date_text,sizeof(date_text),"%a %m/%d", t->tick_time);
		if (date_text[4] == '0') memmove(&date_text[4], &date_text[5], sizeof(date_text) - 1);
		text_layer_set_text(&textLayer[0][1], date_text);
    }

    if (clock_is_24h_style()) {
        string_format_time(hour_text, sizeof(hour_text), "%H", t->tick_time);
		if (hour_text[0] == '0') memmove(&hour_text[0], &hour_text[1], sizeof(hour_text) - 1);
    } else {
        string_format_time(hour_text, sizeof(hour_text), "%I", t->tick_time);
        if (hour_text[0] == '0') memmove(&hour_text[0], &hour_text[1], sizeof(hour_text) - 1);
    }
	string_format_time(minute_text, sizeof(minute_text), ":%M", t->tick_time);
    time_layer_set_text(&time_layer, hour_text, minute_text);
	
	if ((!first_weather && first_quotes) ||  (t->tick_time->tm_min % 30) == ((initial_minute+1) % 30)) {
		request_quotes(); first_quotes = false;
	}
	if (first_weather || (t->tick_time->tm_min % 30) == ((initial_minute+0) % 30)) {
		request_weather(); first_weather = false;
	}
}

void init_handler(AppContextRef ctx) {
    PblTm tm;
	PebbleTickEvent t;
    ResHandle res_h;
    ResHandle res_d;

    resource_init_current_app(&APP_RESOURCES);
    res_h = resource_get_handle(RESOURCE_ID_FUTURA_CONDENSED_53);
    res_d = resource_get_handle(RESOURCE_ID_FUTURA_18);

    font_hour = fonts_load_custom_font(res_h);
    font_date = fonts_load_custom_font(res_d);

	window_init(&window, "Antonio");
    window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, GColorBlack);

    // line 0 for time, line 1 for date, line 2 for weather and lines 3 + 4 for stocks
	for (int i=0; i<NUM_LINES; i++) {
        if (i<=1) text_layer_init(&textLayer[0][i], GRect(0+00,  5+i*54, 144, 45));
        if (i==2) text_layer_init(&textLayer[0][i], GRect(5+00, 35+i*28, 135, 28));
        if (i>=3) text_layer_init(&textLayer[0][i], GRect(5+00, 60+i*20, 135, 20));
        if (i<=2) text_layer_init(&textLayer[1][i], GRect(5+40, 35+i*28, 90,  28));
        if (i>=3) text_layer_init(&textLayer[1][i], GRect(5+40, 60+i*20, 90,  20));
        if (i<=2) text_layer_init(&textLayer[2][i], GRect(5+90, 35+i*28, 45,  28));
        if (i>=3) text_layer_init(&textLayer[2][i], GRect(5+90, 60+i*20, 45,  20));
        text_layer_set_font(&textLayer[0][i], fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
        text_layer_set_font(&textLayer[1][i], fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
        text_layer_set_font(&textLayer[2][i], fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
        if (i<=1) text_layer_set_background_color(&textLayer[0][i], GColorBlack);
        if (i>=2) text_layer_set_background_color(&textLayer[0][i], GColorWhite);
        text_layer_set_background_color(&textLayer[1][i], GColorWhite);
        text_layer_set_background_color(&textLayer[2][i], GColorWhite);
        if (i<=1) text_layer_set_text_color(&textLayer[0][i], GColorWhite);
        if (i>=2) text_layer_set_text_color(&textLayer[0][i], GColorBlack);
        text_layer_set_text_color(&textLayer[1][i], GColorBlack);
        text_layer_set_text_color(&textLayer[2][i], GColorBlack);
        text_layer_set_text_alignment(&textLayer[0][i], GTextAlignmentLeft);
        text_layer_set_text_alignment(&textLayer[1][i], GTextAlignmentLeft);
        text_layer_set_text_alignment(&textLayer[2][i], GTextAlignmentRight);
        layer_add_child(&window.layer, &textLayer[0][i].layer);
        if (i>=3) layer_add_child(&window.layer, &textLayer[1][i].layer);
        if (i>=2) layer_add_child(&window.layer, &textLayer[2][i].layer);
    }
	text_layer_set_text_alignment(&textLayer[0][1], GTextAlignmentCenter);
	text_layer_set_font(&textLayer[0][1], font_date);

    time_layer_init(&time_layer, window.layer.frame);
    time_layer_set_text_color(&time_layer, GColorWhite);
    time_layer_set_background_color(&time_layer, GColorClear);
    time_layer_set_fonts(&time_layer, font_hour, font_hour);
    layer_set_frame(&time_layer.layer, GRect(0, 0, 144, 168-6));
    layer_add_child(&window.layer, &time_layer.layer);
        
    http_set_app_id(PBLINDEX_STOCK_COOKIE); 
    http_register_callbacks((HTTPCallbacks){
        .failure = failed,
        .success = success,
        .reconnect = reconnect,
    }, NULL);

#if MAKE_SCREENSHOT
	http_capture_init(ctx);
#endif

	// Refresh time
	get_time(&tm);
    t.tick_time = &tm;
    t.units_changed = SECOND_UNIT | MINUTE_UNIT | HOUR_UNIT | DAY_UNIT;
	initial_minute = (tm.tm_min % 30);

	handle_minute_tick(ctx, &t);
}	

void handle_deinit(AppContextRef ctx)
{
    fonts_unload_custom_font(font_hour);
    fonts_unload_custom_font(font_date);
}

void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
        .init_handler = &init_handler,
        .tick_info =
        {
            .tick_handler = &handle_minute_tick,
            .tick_units = MINUTE_UNIT
        },
        .messaging_info = {
            .buffer_sizes = {
                .inbound = 124, // 124 safe for Android
                .outbound = 256,
            }
        },
    };
    
#if MAKE_SCREENSHOT
	http_capture_main(&handlers);
#endif
	
    app_event_loop(params, &handlers);
}
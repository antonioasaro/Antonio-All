/* ===========================================================================
 
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
             1, 0,
             RESOURCE_ID_WATCH_MENU_ICON,
             APP_INFO_WATCH_FACE);   // Based off of "pbl-index" by "Edward Patel"

////////////////////////////////////////////////////////////////////////////////////////////////
#define WEATHER_LOC_UNITS "http://antonioasaro.site50.net/weather_db.php"
#define STOCK_QUOTE_LIST  "http://antonioasaro.site50.net/stocks_db.php"     
////////////////////////////////////////////////////////////////////////////////////////////////

#define NUM_LINES 5
#define PBLINDEX_STOCK_COOKIE   9997
#define PBLINDEX_WEATHER_COOKIE 9777
#define WEATHER_DEGREE "°"
#define UPDATED_FRAME   (GRect(0, 71, 144, 168))

Window window;
TimeLayer time_layer;
TextLayer textLayer[3][NUM_LINES];
TextLayer updated_layer;	/* layer for the updated time */

static int initial_minute;
static bool first_quotes = true;
static bool first_weather = true;
static bool weather_bmp = false;
static bool wq_status_bmp = false;
static BmpContainer weather_icon; 
static BmpContainer wq_status_icon; 
GFont font_hour; 

GFont font_updated;     /* font for updated time */
static PblTm updated_tm;


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

// Show the updated info
void set_updated() {
    static char updated_hour_text[] = "00";
    static char updated_minute_text[] = ":00";
	static char updated_info[] = "        "; 		// @00:00pm

	if (clock_is_24h_style()) {
		string_format_time(updated_hour_text, sizeof(updated_hour_text), "%H", &updated_tm);
	}
	else {
	    string_format_time(updated_hour_text, sizeof(updated_hour_text), "%I", &updated_tm); 
	}
	if (updated_hour_text[0] == '0') memmove(&updated_hour_text[0], &updated_hour_text[1], sizeof(updated_hour_text) - 1);
   	string_format_time(updated_minute_text, sizeof(updated_minute_text), ":%M", &updated_tm);
		
	strcpy(updated_info, "@");
	strcat(updated_info, updated_hour_text);
	strcat(updated_info, updated_minute_text);
	if (updated_tm.tm_hour < 12) strcat(updated_info, "am"); else strcat(updated_info, "pm");
	text_layer_set_text(&updated_layer, updated_info);
}

void set_wq_status_icon(BmpContainer *container, bool status) {
	return;
	if(wq_status_bmp) {
		layer_remove_from_parent(&container->layer.layer);
		bmp_deinit_container(container);
		wq_status_bmp = false;
	}
	int status_id = status ? RESOURCE_ID_ICON_WQ_STATUS_OK : RESOURCE_ID_ICON_WQ_STATUS_FAIL;
	bmp_init_container(status_id, container);
	layer_set_frame(&container->layer.layer, GRect(0, 57, 30, 30));
	layer_add_child(&window.layer, &container->layer.layer);
	wq_status_bmp = true; 
}

void set_display_fail(char *text) {
	set_wq_status_icon(&wq_status_icon, false);
}

void set_weather_icon(BmpContainer *container, char *icon) {
	if(weather_bmp) {
		layer_remove_from_parent(&container->layer.layer);
		bmp_deinit_container(container);
		weather_bmp = false;
	}

    int resource_id = RESOURCE_ID_ICON_WIND;  // RAIN;
	if (strcmp(icon, "01d") == 0) resource_id = RESOURCE_ID_ICON_CLEAR_DAY;
	if (strcmp(icon, "01n") == 0) resource_id = RESOURCE_ID_ICON_CLEAR_NIGHT;
	if (strcmp(icon, "02d") == 0) resource_id = RESOURCE_ID_ICON_CLOUDY;
	if (strcmp(icon, "02n") == 0) resource_id = RESOURCE_ID_ICON_CLOUDY;
	if (strcmp(icon, "03d") == 0) resource_id = RESOURCE_ID_ICON_CLOUDY;
	if (strcmp(icon, "03n") == 0) resource_id = RESOURCE_ID_ICON_CLOUDY;
	if (strcmp(icon, "04d") == 0) resource_id = RESOURCE_ID_ICON_CLOUDY;
	if (strcmp(icon, "04n") == 0) resource_id = RESOURCE_ID_ICON_CLOUDY;
	if (strcmp(icon, "09d") == 0) resource_id = RESOURCE_ID_ICON_RAIN;
	if (strcmp(icon, "09n") == 0) resource_id = RESOURCE_ID_ICON_RAIN;
	if (strcmp(icon, "10d") == 0) resource_id = RESOURCE_ID_ICON_RAIN;
	if (strcmp(icon, "10n") == 0) resource_id = RESOURCE_ID_ICON_RAIN;
	if (strcmp(icon, "11d") == 0) resource_id = RESOURCE_ID_ICON_RAIN;
	if (strcmp(icon, "11n") == 0) resource_id = RESOURCE_ID_ICON_RAIN;
	if (strcmp(icon, "13d") == 0) resource_id = RESOURCE_ID_ICON_SNOW;
	if (strcmp(icon, "13n") == 0) resource_id = RESOURCE_ID_ICON_SNOW;
	if (strcmp(icon, "50d") == 0) resource_id = RESOURCE_ID_ICON_FOG;
	if (strcmp(icon, "50n") == 0) resource_id = RESOURCE_ID_ICON_FOG;

	bmp_init_container(resource_id, container);
	layer_set_frame(&container->layer.layer, GRect(100, 45, 60, 60));
	layer_add_child(&window.layer, &container->layer.layer);
	weather_bmp = true;
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
    if (cookie == PBLINDEX_STOCK_COOKIE ||
		cookie == PBLINDEX_WEATHER_COOKIE) {
        set_display_fail("BT fail()");
    }
}

void success(int32_t cookie, int http_status, DictionaryIterator *dict, void *ctx) {
	if (cookie == PBLINDEX_WEATHER_COOKIE) {
		static char conditions[4][32];
   		for (int i=0; i<4; i++) {
			Tuple *weather = dict_find(dict,  i+1);
			if (weather) {
				if (i==0) strcpy(conditions[0], weather->value->cstring); 
				if (i==1) strcpy(conditions[1], itoa(weather->value->int32));	
				if (i==1) strcat(conditions[1], WEATHER_DEGREE); 
				if (i==2) strcat(conditions[1], weather->value->cstring); 
				if (i==3) strcpy(conditions[2], weather->value->cstring); 
				if (i==0) text_layer_set_text(&textLayer[0][2], conditions[0]);
				if (i==2) text_layer_set_text(&textLayer[2][2], conditions[1]);
		 		if (i==3) set_weather_icon(&weather_icon, conditions[2]);
		 		if (i==3) set_wq_status_icon(&wq_status_icon, true);
		   }
	    }
		get_time(&updated_tm); set_updated(); 	// updated last successful wweather event
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
	ResHandle res_u;

    resource_init_current_app(&APP_RESOURCES);
    res_h = resource_get_handle(RESOURCE_ID_FUTURA_CONDENSED_48);
	res_u = resource_get_handle(RESOURCE_ID_FUTURA_12);

    font_hour = fonts_load_custom_font(res_h);
    font_updated = fonts_load_custom_font(res_u);

	window_init(&window, "Antonio");
    window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, GColorBlack);

    // line 0 for time, line 1 for date, line 2 for weather and lines 3 + 4 for stocks
	for (int i=0; i<NUM_LINES; i++) {
        if (i<=1) text_layer_init(&textLayer[0][i], GRect(0+00,  0+i*42, 144, 45));
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
	text_layer_set_font(&textLayer[0][1], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

    time_layer_init(&time_layer, window.layer.frame);
    time_layer_set_text_color(&time_layer, GColorWhite);
    time_layer_set_background_color(&time_layer, GColorClear);
    time_layer_set_fonts(&time_layer, font_hour, font_hour);
    layer_set_frame(&time_layer.layer, GRect(0, 0, 144, 168-6));
    layer_add_child(&window.layer, &time_layer.layer);
	
	// Add updated layer
    text_layer_init(&updated_layer, window.layer.frame);
    text_layer_set_text_color(&updated_layer, GColorWhite);
    text_layer_set_background_color(&updated_layer, GColorClear);
    text_layer_set_font(&updated_layer, font_updated);
    text_layer_set_text_alignment(&updated_layer, GTextAlignmentLeft);
    layer_set_frame(&updated_layer.layer, UPDATED_FRAME);
    layer_add_child(&window.layer, &updated_layer.layer);

        
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
	if (weather_bmp) bmp_deinit_container(&weather_icon); 
	if (wq_status_bmp) bmp_deinit_container(&wq_status_icon);
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
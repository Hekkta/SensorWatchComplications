#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mpg_face.h"

void mpg_face_setup(uint8_t watch_face_index, void ** context_ptr) {
    (void) watch_face_index;
    if (*context_ptr == NULL) {
        *context_ptr = malloc(sizeof(mpg_state_t));
        memset(*context_ptr, 0, sizeof(mpg_state_t));
        // Do any one-time tasks in here; the inside of this conditional happens only at boot.
    }
    // Do any pin or peripheral setup here; this will be called whenever the watch wakes from deep sleep.
}

static void stop_acceleration(mpg_state_t *state) {
    state->accelerating = false;
    movement_request_tick_frequency(1);  // Restore default
}

void mpg_face_activate(void *context) {
    mpg_state_t *state = (mpg_state_t *)context;

    // Handle any tasks related to your watch face coming on screen.
    state->showing_result = false;
    state->editing_miles = true;
    state->miles = 100;
    state->litres = 10;

    // Show your initial UI here.
    char buf[11];
    snprintf(buf, sizeof(buf), "FL%s %03d%02d",state->editing_miles ? " n" : " L", state->miles, state->litres);
    watch_display_text(0, buf);
}

bool mpg_face_loop(movement_event_t event, void *context) {
    mpg_state_t *state = (mpg_state_t *)context;

    switch (event.event_type) {
        case EVENT_ACTIVATE:
            // Show your initial UI here.
            mpg_face_activate(state);
            break;

        case EVENT_TICK:
            if (state->accelerating) {
                if (HAL_GPIO_BTN_ALARM_read()) {
                    // Button still held — keep incrementing
                    if (state->editing_miles) {
                        state->miles += 5;
                    } else {
                        state->litres += 1;
                    }

                    char buf[11];
                    snprintf(buf, sizeof(buf), "FL%s %03d%02d", state->editing_miles ? " n" : " L", state->miles, state->litres);
                    watch_display_text(0, buf);
                } else {
                    // Button released — stop
                    stop_acceleration(state);
                }
            }
            break;

        case EVENT_LIGHT_BUTTON_UP:
            // You can use the Light button for your own purposes.
            state->editing_miles = !state->editing_miles;
            state->showing_result = false;
            char buf[11];
            snprintf(buf, sizeof(buf), "FL%s %03d%02d",state->editing_miles ? " n" : " L", state->miles, state->litres);
            watch_display_text(0, buf);
            break;

        case EVENT_LIGHT_BUTTON_DOWN:
            // Suppress LED if you don’t want it to light up
            // (empty case)
            break;

        case EVENT_ALARM_BUTTON_DOWN:
            break;
        case EVENT_ALARM_LONG_PRESS:
            state->showing_result = false;

            state->accelerating = true;
            movement_request_tick_frequency(8);  // Start faster ticking

            break;
        case EVENT_LIGHT_LONG_PRESS:
            if (state->litres > 0) {
                float mpg = ((float)state->miles) / ((float)state->litres) * 4.54609f;
                char buf[11];
                snprintf(buf, sizeof(buf), "FL    %2d  ", (int)roundf(mpg));
                watch_display_text(0, buf);
                state->showing_result = true;
            } else {
                watch_display_string("ERRNOLIT", 0);
            }
            break;

        case EVENT_MODE_LONG_PRESS:
            /*
            // Long press to calculate and display MPG
            if (state->litres > 0) {
                float mpg = ((float)state->miles) / ((float)state->litres) * 4.54609f;
                char buf[11];
                snprintf(buf, sizeof(buf), "FL  %3d", (int)roundf(mpg));
                watch_display_text(0, buf);
                state->showing_result = true;
            } else {
                watch_display_string("ERRNOLIT", 0);
            }
            */
            break;

        case EVENT_LOW_ENERGY_UPDATE:
            /*
            // Update display once a minute in low energy mode.
            */
            break;
        case EVENT_ALARM_BUTTON_UP:
            if (state->accelerating) {
                // It was a long press – just stop accelerating
                state->accelerating = false;
                movement_request_tick_frequency(1);
            } else {
                // It was a short press – apply single increment
                if (!state->showing_result) {
                    if (state->editing_miles) {
                        state->miles += 5;
                    } else {
                        state->litres += 1;
                    }

                    char buf[11];
                    snprintf(buf, sizeof(buf), "FL%s %03d%02d", state->editing_miles ? " n" : " L", state->miles, state->litres);
                    watch_display_text(0, buf);
                }
            }
            break;
        case EVENT_MODE_BUTTON_UP:
            stop_acceleration(state);
            movement_move_to_next_face();
            return false;
        case EVENT_TIMEOUT:
            stop_acceleration(state);
            break;

        default:
            return movement_default_loop_handler(event);
    }

    return true;
}

void mpg_face_resign(void *context) {
    (void) context;

    // Handle any cleanup before your watch face goes off-screen.
}

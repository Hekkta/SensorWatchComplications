#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include "binomial_face.h"
#include <stdlib.h>  // for malloc
#include <string.h>  // for memset

void binomial_face_setup(uint8_t watch_face_index, void **context_ptr) {
    (void) watch_face_index;
    if (*context_ptr == NULL) {
        *context_ptr = malloc(sizeof(binomial_state_t));
        memset(*context_ptr, 0, sizeof(binomial_state_t));
        
        binomial_state_t *state = (binomial_state_t *)(*context_ptr);
        state->setting_index1 = 0;
        state->setting_index2 = 0;
        state->int_vars[0] = 1;
        state->int_vars[1] = 0;
        state->int_vars[2] = 0;
    }
}

static void stop_acceleration(binomial_state_t *state) {
    state->accelerating = false;
    movement_request_tick_frequency(1);  // Restore default
}

// Efficient binomial coefficient for n <= 100
double binomial_coefficient(int n, int k) {
    if (k > n) return 0.0;
    if (k > n - k) k = n - k;  // symmetry

    double result = 1.0;
    for (int i = 1; i <= k; i++) {
        result *= (n - (k - i));
        result /= i;
    }
    return result;
}

// Binomial PMF: P(X = k)
double binomial_pmf(int n, int k, double p) {
    if (k > n || k < 0) return 0.0;
    double bc = binomial_coefficient(n, k);
    return bc * pow(p, k) * pow(1 - p, n - k);
}

// CDF: P(X ≤ k)
double binomial_cdf(int n, int k, double p) {
    double sum = 0.0;
    for (int i = 0; i <= k; i++) {
        sum += binomial_pmf(n, i, p);
    }
    return sum;
}

// P(X < k)
double binomial_less_than(int n, int k, double p) {
    if (k <= 0) return 0.0; // No successes less than 0
    return binomial_cdf(n, k - 1, p);
}

// Main function to calculate and display probability
void calculate_and_display(binomial_state_t *state) {
    int n = state->int_vars[0]; // flips
    int k = state->int_vars[1]; // heads
    double p = state->int_vars[2] / 100.0;  // prob (scaled from 0-100 to 0.0-1.0)
    printf("Debug: flips=%d, heads=%d, prob=%.3f\n", n, k, p);

    double prob = 0.0;
    switch (state->setting_index1) {
        case 0: // E: exact
            prob = binomial_pmf(n, k, p);
            break;
        case 2: // L: less than or equal
            prob = binomial_cdf(n, k, p);
            break;
        case 1: // El less than
            prob = binomial_less_than(n, k, p);
            break;
        default:
            prob = 0.0;
            break;
    }

    if (prob > 0.999) {
        prob = 0.999;
    }

    printf("Debug: result=%.3f\n", prob);

    char buf[11];
    int whole = (int)(prob * 100);
    int tenth = (int)(prob * 1000) % 10;  // first decimal place

    // Format as two digits, dot, one decimal digit, e.g. "09.3"
    snprintf(buf, sizeof(buf), "Bi    %02d%1d ", whole, tenth);

    // Replace with your actual display function:
    watch_display_text(0, buf);
}

void binomial_face_activate(void *context) {
    binomial_state_t *state = (binomial_state_t *)context;

    // Get the first character from the current setting
    const char *current_setting1 = settings1[state->setting_index1];
    const char *current_setting2 = settings2[state->setting_index2];

    // Show initial UI
    char buf[11];
    if (state->setting_index2<3){
        snprintf(buf, sizeof(buf), "Bi%s %c%02d  ", current_setting1, current_setting2[0], state->int_vars[state->setting_index2]);
        watch_display_text(0, buf);
    }
    else{
        snprintf(buf, sizeof(buf), "Bi%s %c    ", current_setting1, current_setting2[0]);
        watch_display_text(0, buf);
    }
}

bool binomial_face_loop(movement_event_t event, void *context) {
    binomial_state_t *state = (binomial_state_t *)context;

    switch (event.event_type) {
        case EVENT_ACTIVATE:
            // Show your initial UI here.
            binomial_face_activate(state);
            break;

        case EVENT_TICK:
            if (state->accelerating) {
                if (HAL_GPIO_BTN_ALARM_read()) {
                    // Button still held — keep incrementing
                    state->int_vars[state->setting_index2]++;
                    if (state->int_vars[state->setting_index2] > 99){
                        state->int_vars[state->setting_index2] = 0;
                    }
                    binomial_face_activate(state);
                    
                } else {
                    // Button released — stop
                    stop_acceleration(state);
                }
            }
            break;

        case EVENT_LIGHT_BUTTON_UP:
            state->setting_index2++;
            if (state->setting_index2 > 3){
                state->setting_index2=0;
            }
            binomial_face_activate(state);
            break;

        case EVENT_LIGHT_BUTTON_DOWN:
            // Suppress LED if you don’t want it to light up
            // (empty case)
            break;

        case EVENT_ALARM_BUTTON_DOWN:
            break;
        case EVENT_ALARM_LONG_PRESS:
            state->accelerating = true;
            movement_request_tick_frequency(8);  // Start faster ticking
            break;
        case EVENT_LIGHT_LONG_PRESS:
            calculate_and_display(state);
            break;

        case EVENT_MODE_LONG_PRESS:
            /*
            // Long press to calculate and display binomial
            if (state->litres > 0) {
                float binomial = ((float)state->miles) / ((float)state->litres) * 4.54609f;
                char buf[11];
                snprintf(buf, sizeof(buf), "FL  %3d", (int)roundf(binomial));
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
            // add to the current setting 2
            if (state->setting_index2 < 3){
                state->int_vars[state->setting_index2]++;
                if (state->int_vars[state->setting_index2] > 99){
                        state->int_vars[state->setting_index2] = 0;
                    }
            }
            else{
                state->setting_index1++;
                if (state->setting_index1 > 2){
                    state->setting_index1=0;
                }
            }
            binomial_face_activate(state);
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

void binomial_face_resign(void *context) {
    (void) context;

    // Handle any cleanup before your watch face goes off-screen.
}

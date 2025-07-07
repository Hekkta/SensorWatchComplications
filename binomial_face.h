/*
 * MIT License
 *
 * Copyright (c) <#year#> <#author_name#>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "movement.h"


/*
 * binomial FACE
 *
 * Track miles and litres to calculate fuel economy (binomial).
 *
 * - Press the BOTTOM RIGHT button to increase miles (by 5) or litres (by 1).
 * - Hold it down to increase faster.
 * - Press the TOP LEFT button to switch between editing miles and litres.
 * - Long press TOP RIGHT to calculate and display binomial.
 */

static const char *settings1[] = {" E", " L", "EL"};
static const char *settings2[] = {"F", "H", "P", "S"};

typedef struct {
    uint8_t setting_index1;
    uint8_t setting_index2;
    bool accelerating;
    int int_vars[3];
} binomial_state_t;

void binomial_face_setup(uint8_t watch_face_index, void ** context_ptr);
void binomial_face_activate(void *context);
bool binomial_face_loop(movement_event_t event, void *context);
void binomial_face_resign(void *context);

#define binomial_face ((const watch_face_t){ \
    binomial_face_setup, \
    binomial_face_activate, \
    binomial_face_loop, \
    binomial_face_resign, \
    NULL, \
})

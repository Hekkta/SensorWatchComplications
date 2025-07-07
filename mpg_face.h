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
 * MPG FACE
 *
 * Track miles and litres to calculate fuel economy (MPG).
 *
 * - Press the BOTTOM RIGHT button to increase miles (by 5) or litres (by 1).
 * - Hold it down to increase faster.
 * - Press the TOP LEFT button to switch between editing miles and litres.
 * - Long press TOP RIGHT to calculate and display MPG.
 */

typedef struct {
    bool editing_miles;
    bool showing_result;
    bool accelerating;
    uint16_t miles;
    uint8_t litres;
} mpg_state_t;

void mpg_face_setup(uint8_t watch_face_index, void ** context_ptr);
void mpg_face_activate(void *context);
bool mpg_face_loop(movement_event_t event, void *context);
void mpg_face_resign(void *context);

#define mpg_face ((const watch_face_t){ \
    mpg_face_setup, \
    mpg_face_activate, \
    mpg_face_loop, \
    mpg_face_resign, \
    NULL, \
})

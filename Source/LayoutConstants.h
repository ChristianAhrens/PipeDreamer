/*
===============================================================================

Copyright (C) 2026 Christian Ahrens. All Rights Reserved.

  This file is part of the PipeDreamer fork maintained by Christian Ahrens, found at:
  https://github.com/christianahrens/PipeDreamer

  Original project by Bernardo Escalona:
  https://github.com/escalonely/PipeDreamer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

===============================================================================
*/


#pragma once


/**
 * @namespace Layout
 * @brief Named constants for all UI layout dimensions, ratios, and thresholds.
 *
 * Position constants are expressed as window-dimension divisors: the on-screen pixel
 * position is computed as @c windowWidth / DIVISOR or @c windowHeight / DIVISOR.
 * This keeps every magic number in one place and makes the scaling relationships
 * between UI elements immediately readable.
 */
namespace Layout
{
    // ---- Tile sizing --------------------------------------------------------

    /** Intended width-to-height aspect ratio used to derive a single constrained
     *  dimension for uniform tile and font scaling across window sizes. */
    static constexpr float ASPECT_RATIO                 = 1.4516f;

    /** Tile side length = constrainedDimension / TILE_SIZE_DIVISOR. */
    static constexpr float TILE_SIZE_DIVISOR            = 13.0f;

    /** Pipe stroke width = tileSize / PIPE_THICKNESS_DIVISOR. */
    static constexpr float PIPE_THICKNESS_DIVISOR       = 3.5f;

    /** Half-pipe stroke width = tileSize / PIPE_HALF_THICKNESS_DIVISOR.
     *  Used for the cross-pipe separator line positioning. */
    static constexpr float PIPE_HALF_THICKNESS_DIVISOR  = 6.0f;

    /** Cross-pipe separator line thickness = tileSize * CROSS_SEP_NUM / CROSS_SEP_DEN. */
    static constexpr float CROSS_SEP_NUM                = 5.0f;

    /** @see CROSS_SEP_NUM */
    static constexpr float CROSS_SEP_DEN                = 70.0f;


    // ---- Board position -----------------------------------------------------

    /** Board grid left edge = windowWidth / BOARD_H_DIVISOR. */
    static constexpr float BOARD_H_DIVISOR  = 5.114f;

    /** Board grid top edge = windowHeight / BOARD_V_DIVISOR. */
    static constexpr float BOARD_V_DIVISOR  = 7.75f;


    // ---- Pipe queue position ------------------------------------------------

    /** Queue column left edge = windowWidth / QUEUE_H_DIVISOR. */
    static constexpr float QUEUE_H_DIVISOR  = 18.0f;

    /** Bottom-most queue tile top edge = windowHeight / QUEUE_V_DIVISOR. */
    static constexpr float QUEUE_V_DIVISOR  = 1.3757f;


    // ---- Ooze meter ---------------------------------------------------------

    /** Ooze meter left edge = windowWidth / OOZE_METER_H_DIVISOR. */
    static constexpr float OOZE_METER_H_DIVISOR      = 12.05f;

    /** Ooze vial height = windowHeight / OOZE_METER_HEIGHT_DIVISOR. */
    static constexpr float OOZE_METER_HEIGHT_DIVISOR = 5.2542f;

    /** Total width of the ooze vial background rectangle, in pixels. */
    static constexpr int   OOZE_VIAL_WIDTH   = 22;

    /** Width of the ooze fill bar drawn inside the vial, in pixels. */
    static constexpr int   OOZE_FILL_WIDTH   = 16;

    /** Padding between the vial border and the fill bar on each side, in pixels. */
    static constexpr int   OOZE_VIAL_PADDING = 3;


    // ---- Bomb display -------------------------------------------------------

    /** Bomb indicator row left edge = windowWidth / BOMBS_H_DIVISOR. */
    static constexpr float BOMBS_H_DIVISOR  = 1.6749f;


    // ---- Fast-forward button ------------------------------------------------

    /** FF button left edge = windowWidth / FF_BUTTON_H_DIVISOR. */
    static constexpr float FF_BUTTON_H_DIVISOR      = 18.0f;

    /** FF button top edge = windowHeight / FF_BUTTON_V_DIVISOR. */
    static constexpr float FF_BUTTON_V_DIVISOR      = 1.1245f;

    /** Width of the fast-forward button rectangle, in pixels. */
    static constexpr float FF_BUTTON_W              = 70.0f;

    /** Height of the fast-forward button rectangle, in pixels. */
    static constexpr float FF_BUTTON_H              = 45.0f;

    /** X offset of the first FF icon triangle center from the button's left edge. */
    static constexpr float FF_ICON_CENTER_X         = 24.0f;

    /** Y offset of both FF icon triangle centers from the button's top edge. */
    static constexpr float FF_ICON_CENTER_Y         = 23.0f;

    /** Horizontal spacing between the two FF icon triangles, in pixels. */
    static constexpr float FF_ICON_TRIANGLE_SPACING = 19.0f;

    /** Circumradius of each triangle polygon in the FF icon, in pixels. */
    static constexpr float FF_ICON_RADIUS           = 13.0f;

    /** Rotation applied to each FF triangle polygon, in radians (approx. -30 degrees). */
    static constexpr float FF_ICON_ROTATION         = -0.52f;


    // ---- Score / level labels -----------------------------------------------

    /** "Level:" label left edge = windowWidth / LEVEL_LABEL_H_DIVISOR. */
    static constexpr float LEVEL_LABEL_H_DIVISOR       = 5.114f;

    /** Width of the "Level:" and "Score:" text rects = windowWidth / SCORE_LABEL_WIDTH_DIVISOR. */
    static constexpr float SCORE_LABEL_WIDTH_DIVISOR   = 9.7826f;

    /** "Score:" text left edge = windowWidth / SCORE_LABEL_H_DIVISOR. */
    static constexpr float SCORE_LABEL_H_DIVISOR       = 2.8421f;

    /** Score value left edge = windowWidth / SCORE_VALUE_H_DIVISOR. */
    static constexpr float SCORE_VALUE_H_DIVISOR       = 2.1880f;

    /** Score value rect width = windowWidth / SCORE_VALUE_WIDTH_DIVISOR. */
    static constexpr float SCORE_VALUE_WIDTH_DIVISOR   = 5.625f;

    /** Difficulty level number left edge = windowWidth / LEVEL_VALUE_H_DIVISOR. */
    static constexpr float LEVEL_VALUE_H_DIVISOR       = 3.375f;

    /** Difficulty level number rect width = windowWidth / LEVEL_VALUE_WIDTH_DIVISOR. */
    static constexpr float LEVEL_VALUE_WIDTH_DIVISOR   = 17.31f;

    /** Top Y offset shared by all score and level label rows, in pixels. */
    static constexpr int   SCORE_LABEL_V_OFFSET        = 20;


    // ---- Font scaling -------------------------------------------------------

    /** Reference window height at which all font point sizes were originally calibrated. */
    static constexpr float FONT_REF_HEIGHT  = 620.0f;

    /** Point size of the version hyperlink label at the reference window height. */
    static constexpr float FONT_VERSION_PT  = 18.0f;

    /** Point size of the score and level labels at the reference window height. */
    static constexpr float FONT_SCORE_PT    = 32.0f;


    // ---- Timer --------------------------------------------------------------

    /** GUI refresh interval in milliseconds. */
    static constexpr int GUI_REFRESH_RATE   = 60;


    // ---- Window size constraints --------------------------------------------

    /** Default initial window width on desktop, in pixels. */
    static constexpr int WINDOW_DEFAULT_W   = 900;

    /** Default initial window height on desktop, in pixels. */
    static constexpr int WINDOW_DEFAULT_H   = 620;

    /** Minimum allowed window width on desktop, in pixels. */
    static constexpr int WINDOW_MIN_W       = 594;

    /** Minimum allowed window height on desktop, in pixels. */
    static constexpr int WINDOW_MIN_H       = 414;

    /** Maximum allowed window width on desktop, in pixels. */
    static constexpr int WINDOW_MAX_W       = 2560;

    /** Maximum allowed window height on desktop, in pixels. */
    static constexpr int WINDOW_MAX_H       = 1440;


    // ---- High-score panel (HighScoreWindow) ---------------------------------

    /** Width of the score breakdown panel on the left of the HighScoreWindow, in pixels. */
    static constexpr int HS_SCORE_PANEL_W    = 320;

    /** Name column left offset relative to (windowLeft + tileSize), in pixels. */
    static constexpr int HS_COL_NAME_OFFSET  = 360;

    /** Score column left offset relative to (windowLeft + tileSize), in pixels. */
    static constexpr int HS_COL_SCORE_OFFSET = 460;

    /** Date column left offset relative to (windowLeft + tileSize), in pixels. */
    static constexpr int HS_COL_DATE_OFFSET  = 590;

    /** Width of the name column in the high-score table, in pixels. */
    static constexpr int HS_COL_NAME_W       = 100;

    /** Width of the score column in the high-score table, in pixels. */
    static constexpr int HS_COL_SCORE_W      = 100;

    /** Width of the date column in the high-score table, in pixels. */
    static constexpr int HS_COL_DATE_W       = 130;

    /** Height of each row in the high-score table, in pixels. */
    static constexpr int HS_ROW_HEIGHT       = 30;

    /** Vertical offset from the top of the high-score box to the first table row, in pixels. */
    static constexpr int HS_TABLE_V_OFFSET   = 80;

    /** Font point size used for high-score table entries. */
    static constexpr float HS_FONT_PT        = 25.0f;
}

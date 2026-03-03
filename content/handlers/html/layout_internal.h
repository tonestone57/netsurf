/*
 * Copyright 2003 James Bursa <bursa@users.sourceforge.net>
 *
 * This file is part of NetSurf, http://www.netsurf-browser.org/
 *
 * NetSurf is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * NetSurf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file
 * HTML layout private interface.
 */

#ifndef NETSURF_HTML_LAYOUT_INTERNAL_H
#define NETSURF_HTML_LAYOUT_INTERNAL_H

#define AUTO INT_MIN

/**
 * Layout a block formatting context.
 *
 * \param  block            BLOCK, INLINE_BLOCK, or TABLE_CELL to layout
 * \param  viewport_height  Height of viewport in pixels or -ve if unknown
 * \param  content          Memory pool for any new boxes
 * \return  true on success, false on memory exhaustion
 *
 * This function carries out layout of a block and its children, as described
 * in CSS 2.1 9.4.1.
 */
bool layout_block_context(
		struct box *block,
		int viewport_height,
		html_content *content);

/**
 * Layout a table.
 *
 * \param  table            table to layout
 * \param  available_width  width of containing block
 * \param  content          memory pool for any new boxes
 * \return  true on success, false on memory exhaustion
 */
bool layout_table(
		struct box *table,
		int available_width,
		html_content *content);

/**
 * Layout a flex container.
 *
 * \param[in] flex             table to layout
 * \param[in] available_width  width of containing block
 * \param[in] content          memory pool for any new boxes
 * \return  true on success, false on memory exhaustion
 */
bool layout_flex(
		struct box *flex,
		int available_width,
		html_content *content);

/**
 * Internal dimension calculation function for full cache population.
 *
 * \param  unit_len_ctx     Length conversion context
 * \param  available_width  width of containing block
 * \param  viewport_height  height of viewport in pixels or -ve if unknown
 * \param  box              current box
 * \param  style            style giving width, height, margins, paddings,
 *                          and borders
 */
void layout_find_dimensions_internal(
		const css_unit_ctx *unit_len_ctx,
		int available_width,
		int viewport_height,
		struct box *box,
		const css_computed_style *style);

typedef uint8_t (*css_len_func)(
		const css_computed_style *style,
		css_fixed *length, css_unit *unit);
typedef uint8_t (*css_border_style_func)(
		const css_computed_style *style);
typedef uint8_t (*css_border_color_func)(
		const css_computed_style *style,
		css_color *color);

/** Array of per-side access functions for computed style margins. */
extern const css_len_func margin_funcs[4];

/** Array of per-side access functions for computed style paddings. */
extern const css_len_func padding_funcs[4];

/** Array of per-side access functions for computed style border_widths. */
extern const css_len_func border_width_funcs[4];

/** Array of per-side access functions for computed style border styles. */
extern const css_border_style_func border_style_funcs[4];

/** Array of per-side access functions for computed style border colors. */
extern const css_border_color_func border_color_funcs[4];

/** Layout helper: Check whether box is a float. */
static inline bool layout__box_is_float_box(const struct box *b)
{
	return b->type == BOX_FLOAT_LEFT ||
	       b->type == BOX_FLOAT_RIGHT;
}

/** Layout helper: Check whether box takes part in inline flow. */
static inline bool layout__box_is_inline_flow(const struct box *b)
{
	return b->type == BOX_INLINE ||
	       b->type == BOX_INLINE_FLEX ||
	       b->type == BOX_INLINE_BLOCK ||
	       b->type == BOX_TEXT ||
	       b->type == BOX_INLINE_END;
}

/** Layout helper: Check whether box takes part in inline flow. */
static inline bool layout__box_is_flex_container(const struct box *b)
{
	return b->type == BOX_FLEX ||
	       b->type == BOX_INLINE_FLEX;
}

/** Layout helper: Check whether box takes part in inline flow. */
static inline bool layout__box_is_flex_item(const struct box *b)
{
	return (b->parent != NULL) && layout__box_is_flex_container(b->parent);
}

/** Layout helper: Check whether box is inline level. (Includes BR.) */
static inline bool layout__box_is_inline_level(const struct box *b)
{
	return layout__box_is_inline_flow(b) ||
	       b->type == BOX_BR;
}

/** Layout helper: Check whether box is inline level. (Includes BR, floats.) */
static inline bool layout__box_is_inline_content(const struct box *b)
{
	return layout__box_is_float_box(b) ||
	       layout__box_is_inline_level(b);
}

/** Layout helper: Check whether box is an object. */
static inline bool layout__box_is_object(const struct box *b)
{
	return b->object ||
	       (b->flags & (IFRAME | REPLACE_DIM));
}

/** Layout helper: Check whether box is replaced. */
static inline bool layout__box_is_replace(const struct box *b)
{
	return b->gadget ||
	       layout__box_is_object(b);
}

/** Layout helper: Check for CSS border on given side. */
static inline bool layout__have_border(
		enum box_side side,
		const css_computed_style *style)
{
	return border_style_funcs[side](style) != CSS_BORDER_STYLE_NONE;
}

static inline bool layout__box_is_absolute(const struct box *b)
{
	if (b->style == NULL)
		return false;

	return css_computed_position(b->style) == CSS_POSITION_ABSOLUTE ||
	       css_computed_position(b->style) == CSS_POSITION_FIXED;
}

static inline bool layout__flex_main_is_horizontal(const struct box *flex)
{
	const css_computed_style *style = flex->style;

	if (style == NULL)
		return true;

	switch (css_computed_flex_direction(style)) {
	default:                        /* Fallthrough. */
	case CSS_FLEX_DIRECTION_ROW:    /* Fallthrough. */
	case CSS_FLEX_DIRECTION_ROW_REVERSE:
		return true;
	case CSS_FLEX_DIRECTION_COLUMN: /* Fallthrough. */
	case CSS_FLEX_DIRECTION_COLUMN_REVERSE:
		return false;
	}
}

static inline bool layout__flex_direction_reversed(const struct box *flex)
{
	if (flex->style == NULL)
		return false;

	switch (css_computed_flex_direction(flex->style)) {
	default:                             /* Fallthrough. */
	case CSS_FLEX_DIRECTION_ROW_REVERSE: /* Fallthrough. */
	case CSS_FLEX_DIRECTION_COLUMN_REVERSE:
		return true;
	case CSS_FLEX_DIRECTION_ROW: /* Fallthrough. */
	case CSS_FLEX_DIRECTION_COLUMN:
		return false;
	}
}

static inline int layout__non_auto_margin(const struct box *b, enum box_side side)
{
	return (b->margin[side] == AUTO) ? 0 : b->margin[side];
}

static inline int layout__delta_outer_height(const struct box *b)
{
	return b->padding[TOP] +
	       b->padding[BOTTOM] +
	       b->border[TOP].width +
	       b->border[BOTTOM].width +
	       layout__non_auto_margin(b, TOP) +
	       layout__non_auto_margin(b, BOTTOM);
}

static inline int layout__delta_outer_width(const struct box *b)
{
	return b->padding[LEFT] +
	       b->padding[RIGHT] +
	       b->border[LEFT].width +
	       b->border[RIGHT].width +
	       layout__non_auto_margin(b, LEFT) +
	       layout__non_auto_margin(b, RIGHT);
}

static inline int layout__delta_outer_main(
		const struct box *flex,
		const struct box *b)
{
	if (layout__flex_main_is_horizontal(flex)) {
		return layout__delta_outer_width(b);
	} else {
		return layout__delta_outer_height(b);
	}
}

static inline int layout__delta_outer_cross(
		const struct box *flex,
		const struct box *b)
{
	if (layout__flex_main_is_horizontal(flex) == false) {
		return layout__delta_outer_width(b);
	} else {
		return layout__delta_outer_height(b);
	}
}

static inline int *layout__box_size_main_ptr(
		bool horizontal,
		struct box *b)
{
	return horizontal ? &b->width : &b->height;
}

static inline int *layout__box_size_cross_ptr(
		bool horizontal,
		struct box *b)
{
	return horizontal ? &b->height : &b->width;
}

static inline int layout__box_size_main(
		bool horizontal,
		const struct box *b)
{
	return horizontal ? b->width : b->height;
}

static inline int layout__box_size_cross(
		bool horizontal,
		const struct box *b)
{
	return horizontal ? b->height : b->width;
}

static inline bool layout__box_size_cross_is_auto(
		bool horizontal,
		struct box *b)
{
	css_fixed length;
	css_unit unit;

	if (b->style == NULL)
		return true;

	if (horizontal) {
		return css_computed_height(b->style,
				&length, &unit) == CSS_HEIGHT_AUTO;
	} else {
		return css_computed_width(b->style,
				&length, &unit) == CSS_WIDTH_AUTO;
	}
}

static inline enum css_align_self_e layout__box_align_self(
		const struct box *flex,
		const struct box *item)
{
	enum css_align_self_e align_self = CSS_ALIGN_SELF_AUTO;

	if (item->style != NULL) {
		align_self = css_computed_align_self(item->style);
	}

	if (align_self == CSS_ALIGN_SELF_AUTO && flex->style != NULL) {
		align_self = css_computed_align_items(flex->style);
	}

	return align_self;
}

/**
 * Determine width of margin, borders, and padding on one side of a box.
 *
 * \param unit_len_ctx  CSS length conversion context for document
 * \param style    style to measure
 * \param side     side of box to measure
 * \param margin   whether margin width is required
 * \param border   whether border width is required
 * \param padding  whether padding width is required
 * \param fixed    increased by sum of fixed margin, border, and padding
 * \param frac     increased by sum of fractional margin and padding
 */
static inline void calculate_mbp_width(
		const css_unit_ctx *unit_len_ctx,
		const css_computed_style *style,
		unsigned int side,
		bool margin,
		bool border,
		bool padding,
		int *fixed,
		float *frac)
{
	css_fixed value = 0;
	css_unit unit = CSS_UNIT_PX;

	if (style == NULL)
		return;

	/* margin */
	if (margin) {
		enum css_margin_e type;

		type = margin_funcs[side](style, &value, &unit);
		if (type == CSS_MARGIN_SET) {
			if (unit == CSS_UNIT_PCT) {
				*frac += FIXTOFLT(FDIV(value, F_100));
			} else {
				*fixed += FIXTOINT(css_unit_len2device_px(
						style, unit_len_ctx,
						value, unit));
			}
		}
	}

	/* border */
	if (border) {
		if (layout__have_border(side, style)) {
			border_width_funcs[side](style, &value, &unit);

			*fixed += FIXTOINT(css_unit_len2device_px(
					style, unit_len_ctx,
					value, unit));
		}
	}

	/* padding */
	if (padding) {
		padding_funcs[side](style, &value, &unit);
		if (unit == CSS_UNIT_PCT) {
			*frac += FIXTOFLT(FDIV(value, F_100));
		} else {
			*fixed += FIXTOINT(css_unit_len2device_px(
					style, unit_len_ctx,
					value, unit));
		}
	}
}

/**
 * Adjust a specified width or height for the box-sizing property.
 *
 * This turns the specified dimension into a content-box dimension.
 *
 * \param  unit_len_ctx          Length conversion context
 * \param  box		    gadget to adjust dimensions of
 * \param  available_width  width of containing block
 * \param  setwidth	    set true if the dimension to be tweaked is a width,
 *				else set false for a height
 * \param  dimension	    current value for given width/height dimension.
 *				updated to new value after consideration of
 *				gadget properties.
 */
static inline void layout_handle_box_sizing(
		const css_unit_ctx *unit_len_ctx,
		const struct box *box,
		const css_computed_style *style,
		int available_width,
		bool setwidth,
		int *dimension)
{
	enum css_box_sizing_e bs;

	if (style == NULL)
		return;

	bs = css_computed_box_sizing(style);

	if (bs == CSS_BOX_SIZING_BORDER_BOX) {
		int orig = *dimension;
		int fixed = 0;
		float frac = 0;

		calculate_mbp_width(unit_len_ctx, style,
				setwidth ? LEFT : TOP,
				false, true, true, &fixed, &frac);
		calculate_mbp_width(unit_len_ctx, style,
				setwidth ? RIGHT : BOTTOM,
				false, true, true, &fixed, &frac);
		orig -= frac * available_width + fixed;
		*dimension = orig > 0 ? orig : 0;
	}
}

/**
 * Calculate width, height, and thickness of margins, paddings, and borders.
 *
 * \param  unit_len_ctx     Length conversion context
 * \param  available_width  width of containing block
 * \param  viewport_height  height of viewport in pixels or -ve if unknown
 * \param  box              current box
 * \param  style            style giving width, height, margins, paddings,
 *                          and borders
 * \param  width            updated to width, may be NULL
 * \param  height           updated to height, may be NULL
 * \param  max_width        updated to max-width, may be NULL
 * \param  min_width        updated to min-width, may be NULL
 * \param  max_height       updated to max-height, may be NULL
 * \param  min_height       updated to min-height, may be NULL
 * \param  margin           filled with margins, may be NULL
 * \param  padding          filled with paddings, may be NULL
 * \param  border           filled with border widths, may be NULL
 */
static inline void layout_find_dimensions(
		const css_unit_ctx *unit_len_ctx,
		int available_width,
		int viewport_height,
		struct box *box,
		const css_computed_style *style,
		int *width,
		int *height,
		int *max_width,
		int *min_width,
		int *max_height,
		int *min_height,
		int margin[4],
		int padding[4],
		struct box_border border[4])
{
	css_fixed font_size = 0;
	css_unit font_unit = CSS_UNIT_PX;

	if (style != NULL) {
		css_computed_font_size(style, &font_size, &font_unit);
	}

	if (!(box->flags & DIM_CACHED) ||
	    box->cached_style != style ||
	    box->cached_font_size != font_size ||
	    box->cached_viewport_width != unit_len_ctx->viewport_width ||
	    box->cached_viewport_height_css != unit_len_ctx->viewport_height ||
	    box->cached_device_dpi != unit_len_ctx->device_dpi ||
	    box->cached_root_style != unit_len_ctx->root_style ||
	    box->cached_available_width != available_width ||
	    box->cached_viewport_height != viewport_height) {
		layout_find_dimensions_internal(unit_len_ctx, available_width,
				viewport_height, box, style);
	}

	if (width) *width = box->cached_width;
	if (height) *height = box->cached_height;
	if (max_width) *max_width = box->cached_max_width;
	if (min_width) *min_width = box->cached_min_width;
	if (max_height) *max_height = box->cached_max_height;
	if (min_height) *min_height = box->cached_min_height;
	if (margin) memcpy(margin, box->cached_margin, sizeof(int) * 4);
	if (padding) memcpy(padding, box->cached_padding, sizeof(int) * 4);
	if (border) memcpy(border, box->cached_border,
			sizeof(struct box_border) * 4);
}

#endif

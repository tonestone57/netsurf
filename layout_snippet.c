/*
 * Copyright 2005 Richard Wilson <info@tinct.net>
 * Copyright 2006 James Bursa <bursa@users.sourceforge.net>
 * Copyright 2008 Michael Drake <tlsa@netsurf-browser.org>
 * Copyright 2003 Phil Mellor <monkeyson@users.sourceforge.net>
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
 * HTML layout implementation.
 *
 * Layout is carried out in two stages:
 *
 * 1. + calculation of minimum / maximum box widths, and
 *    + determination of whether block level boxes will have >zero height
 *
 * 2. + layout (position and dimensions)
 *
 * In most cases the functions for the two stages are a corresponding pair
 * layout_minmax_X() and layout_X().
 */

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dom/dom.h>

#include "utils/log.h"
#include "utils/talloc.h"
#include "utils/utils.h"
#include "utils/nsoption.h"
#include "utils/corestrings.h"
#include "utils/nsurl.h"
#include "netsurf/inttypes.h"
#include "netsurf/content.h"
#include "netsurf/browser_window.h"
#include "netsurf/layout.h"
#include "content/content.h"
#include "content/content_protected.h"
#include "css/utils.h"
#include "desktop/scrollbar.h"
#include "desktop/textarea.h"

#include "html/html.h"
#include "html/html_save.h"
#include "html/private.h"
#include "html/box.h"
#include "html/box_inspect.h"
#include "html/font.h"
#include "html/form_internal.h"
#include "html/layout.h"
#include "html/layout_internal.h"
#include "html/table.h"

/** Array of per-side access functions for computed style margins. */
const css_len_func margin_funcs[4] = {
	[TOP]    = css_computed_margin_top,
	[RIGHT]  = css_computed_margin_right,
	[BOTTOM] = css_computed_margin_bottom,
	[LEFT]   = css_computed_margin_left,
};

/** Array of per-side access functions for computed style paddings. */
const css_len_func padding_funcs[4] = {
	[TOP]    = css_computed_padding_top,
	[RIGHT]  = css_computed_padding_right,
	[BOTTOM] = css_computed_padding_bottom,
	[LEFT]   = css_computed_padding_left,
};

/** Array of per-side access functions for computed style border_widths. */
const css_len_func border_width_funcs[4] = {
	[TOP]    = css_computed_border_top_width,
	[RIGHT]  = css_computed_border_right_width,
	[BOTTOM] = css_computed_border_bottom_width,
	[LEFT]   = css_computed_border_left_width,
};

/** Array of per-side access functions for computed style border styles. */
const css_border_style_func border_style_funcs[4] = {
	[TOP]    = css_computed_border_top_style,
	[RIGHT]  = css_computed_border_right_style,
	[BOTTOM] = css_computed_border_bottom_style,
	[LEFT]   = css_computed_border_left_style,
};

/** Array of per-side access functions for computed style border colors. */
const css_border_color_func border_color_funcs[4] = {
	[TOP]    = css_computed_border_top_color,
	[RIGHT]  = css_computed_border_right_color,
	[BOTTOM] = css_computed_border_bottom_color,
	[LEFT]   = css_computed_border_left_color,
};

/* forward declaration to break cycles */
static void layout_minmax_block(
		struct box *block,
		const struct gui_layout_table *font_func,
		const html_content *content);

static void
find_sides(struct box *fl,
	   int y0, int y1,
	   int *x0, int *x1,
	   struct box **left,
	   struct box **right);

/**
 * Compute the size of an iframe with auto dimensions.
 *
 * \param  box         Box with iframe
 * \param  width       Width value in px or AUTO.  If AUTO, updated to value in px.
 * \param  height      Height value in px or AUTO. If AUTO, updated to value in px.
 * \param  min_width   Box's min width.
 * \param  max_width   Box's max width.
 * \param  min_height  Box's min height.
 * \param  max_height  Box's max height.
 */

/**
 * Apply vertical margin if necessary.
 *
 * \param  box             box to apply margin to
 * \param  margin_collapse  box that current margin collapses to
 * \param  in_margin       whether a margin is pending
 * \param  cy              current y coordinate, updated
 * \param  max_pos_margin  maximum positive margin, updated to 0
 * \param  max_neg_margin  maximum negative margin, updated to 0
 */
static void layout_block_context_apply_margin(
		struct box *box,
		struct box *margin_collapse,
		bool *in_margin,
		int *cy,
		int *max_pos_margin,
		int *max_neg_margin)
{
	if (((box->type == BOX_BLOCK && (box->flags & HAS_HEIGHT)) ||
	     box->type == BOX_FLEX ||
	     box->type == BOX_TABLE ||
	     (box->type == BOX_INLINE_CONTAINER &&
	      !box_is_first_child(box)) ||
	     margin_collapse == box) &&
	    *in_margin == true) {
		/* Margin goes above this box. */
		*cy += *max_pos_margin - *max_neg_margin;
		box->y += *max_pos_margin - *max_neg_margin;

		/* Current margin has been applied. */
		*in_margin = false;
		*max_pos_margin = *max_neg_margin = 0;
	}
}


/**
 * Reflow an object.
 *
 * \param  box     box with object
 * \param  width   available width
 */
static void layout_block_object_reformat(struct box *box, int width)
{
	if (content_can_reformat(box->object)) {
		content_reformat(box->object, false, width, 1);
	}
}


/**
 * Set positions of boxes in a line.
 *
 * \param  first        first box in line
 * \param  last         box after last box in line
 * \param  x0           x offset for alignment
 * \param  y            current y coordinate
 * \param  used_height  updated to actual line height
 */
static void layout_line_set_positions(struct box *first, struct box *last,
		int x0, int *y, int *used_height)
{
	struct box *d;
	int h;

	for (d = first; d != last; d = d->next) {
		d->flags &= ~NEW_LINE;

		if (lh__box_is_absolute(d)) {
			/* positioned inline-blocks:
			 * set static position (x,y) only, rest of positioning
			 * is handled later */
			d->x += x0;
			d->y = *y;
			continue;
		} else if ((d->type == BOX_INLINE &&
				lh__box_is_replace(d) == false) ||
				d->type == BOX_BR ||
				d->type == BOX_TEXT ||
				d->type == BOX_INLINE_END) {
			/* regular (non-replaced) inlines */
			d->x += x0;
			d->y = *y - d->padding[TOP];

			if (d->type == BOX_TEXT && d->height > *used_height) {
				/* text */
				*used_height = d->height;
			}
		} else if ((d->type == BOX_INLINE) ||
				d->type == BOX_INLINE_BLOCK) {
			/* replaced inlines and inline-blocks */
			d->x += x0;
			d->y = *y + d->border[TOP].width + d->margin[TOP];
			h = d->margin[TOP] + d->border[TOP].width +
					d->padding[TOP] + d->height +
					d->padding[BOTTOM] +
					d->border[BOTTOM].width +
					d->margin[BOTTOM];
			if (*used_height < h)
				*used_height = h;
		}
	}
}


/**
 * Handle vertical-align by adjusting box y values.
 *
 * \param  first        first box in line
 * \param  last         box after last box in line
 * \param  used_height  height of line
 */
static void layout_line_vertical_align(struct box *first, struct box *last,
		int used_height)
{
	struct box *d;

	for (d = first; d != last; d = d->next) {
		if ((d->type == BOX_INLINE && d->inline_end) ||
				d->type == BOX_BR ||
				d->type == BOX_TEXT ||
				d->type == BOX_INLINE_END) {
			css_fixed value = 0;
			css_unit unit = CSS_UNIT_PX;
			switch (css_computed_vertical_align(d->style, &value,
					&unit)) {
			case CSS_VERTICAL_ALIGN_SUPER:
			case CSS_VERTICAL_ALIGN_TOP:
			case CSS_VERTICAL_ALIGN_TEXT_TOP:
				/* already at top */
				break;
			case CSS_VERTICAL_ALIGN_SUB:
			case CSS_VERTICAL_ALIGN_BOTTOM:
			case CSS_VERTICAL_ALIGN_TEXT_BOTTOM:
				d->y += used_height - d->height;
				break;
			default:
			case CSS_VERTICAL_ALIGN_BASELINE:
				d->y += 0.75 * (used_height - d->height);
				break;
			}
		}
	}
}


/**
 * Move a table down to avoid floats if necessary.
 *
 * \param  block  box responsible for current block formatting context
 * \param  cx     current x coordinate
 * \param  cy     current y coordinate
 * \param  box    table box
 * \param  y      updated to new y coordinate
 * \param  x0     updated to new x coordinate
 */
static void layout_block_context_position_table(
		struct box *block,
		int cx, int cy,
		struct box *box,
		int *y, int *x0)
{
	int x1;
	struct box *left, *right;

	*y = cy;
	while (1) {
		enum css_width_e wtype;
		css_fixed width = 0;
		css_unit unit = CSS_UNIT_PX;

		wtype = css_computed_width(box->style,
				&width, &unit);

		*x0 = cx;
		x1 = cx + box->parent->width;
		find_sides(block->float_children, *y,
				*y + box->height,
				x0, &x1, &left, &right);
		if (wtype == CSS_WIDTH_AUTO)
			break;
		if (box->width <= x1 - *x0)
			break;
		if (!left && !right)
			break;
		else if (!left)
			*y = right->y + right->height + 1;
		else if (!right)
			*y = left->y + left->height + 1;
		else if (left->y + left->height <
				right->y + right->height)
			*y = left->y + left->height + 1;
		else
			*y = right->y + right->height + 1;
	}
}


/**
 * Calculate the minimum required left and right margins to avoid floats.
 *
 * \param  block           box responsible for current block formatting context
 * \param  cx              current x coordinate
 * \param  cy              current y coordinate
 * \param  y               current clearance y coordinate
 * \param  max_pos_margin  maximum positive margin encountered
 * \param  max_neg_margin  maximum negative margin encountered
 * \param  box             box to calculate margins for
 * \param  lm              updated to minimum left margin
 * \param  rm              updated to minimum right margin
 */
static void layout_block_context_bfc_margins(
		struct box *block,
		int cx, int cy, int y,
		int max_pos_margin, int max_neg_margin,
		struct box *box,
		int *lm, int *rm)
{
	int x0, x1, top;
	struct box *left, *right;

	top = cy + max_pos_margin - max_neg_margin;
	top = (top > y) ? top : y;
	x0 = cx;
	x1 = cx + box->parent->width -
			box->parent->padding[LEFT] -
			box->parent->padding[RIGHT];
	find_sides(block->float_children, top, top,
			&x0, &x1, &left, &right);
	/* calculate min required left & right margins
	 * needed to avoid floats */
	*lm = x0 - cx;
	*rm = cx + box->parent->width -
			box->parent->padding[LEFT] -
			box->parent->padding[RIGHT] -
			x1;
}


static void
layout_get_iframe_dimensions(struct box *box,
			     int *width, int *height,
			     int min_width, int max_width,
			     int min_height, int max_height)
{
	assert(box->flags & IFRAME);
	assert(width != NULL && height != NULL);

	if (*width == AUTO)
		*width = 300;
	if (*height == AUTO)
		*height = 150;

	if (min_width > 0 && min_width > *width)
		*width = min_width;
	if (max_width >= 0 && max_width < *width)
		*width = max_width;
	if (min_height > 0 && min_height > *height)
		*height = min_height;
	if (max_height >= 0 && max_height < *height)
		*height = max_height;
}


/**
 * Compute the size of replaced boxes with auto dimensions, according to
 * content.
 *
 * \param  box     Box with object
 * \param  width   Width value in px or AUTO.  If AUTO, updated to value in px.
 * \param  height  Height value in px or AUTO. If AUTO, updated to value in px.
 * \param  min_width  Box's min width, as given by layout_find_dimensions.
 * \param  max_width  Box's max width, as given by layout_find_dimensions.
 * \param  min_height  Box's min height, as given by layout_find_dimensions.
 * \param  max_height  Box's max height, as given by layout_find_dimensions.
 *
 * See CSS 2.1 sections 10.3 and 10.6.
 */
static void
layout_get_object_dimensions(struct box *box,
			     int *width, int *height,
			     int min_width, int max_width,
			     int min_height, int max_height)
{
	assert(box->object != NULL);
	assert(width != NULL && height != NULL);

	if (*width == AUTO && *height == AUTO) {
		/* No given dimensions */

		bool scaled = false;
		int intrinsic_width = content_get_width(box->object);
		int intrinsic_height = content_get_height(box->object);

		/* use intrinsic dimensions */
		*width = intrinsic_width;
		*height = intrinsic_height;

		/* Deal with min/max-width first */
		if (min_width >  0 && min_width > *width) {
			*width = min_width;
			scaled = true;
		}
		if (max_width >= 0 && max_width < *width) {
			*width = max_width;
			scaled = true;
		}

		if (scaled && (intrinsic_width != 0)) {
			/* Update height */
			*height = (*width * intrinsic_height) /
					intrinsic_width;
		}

		scaled = false;
		/* Deal with min/max-height */
		if (min_height >  0 && min_height > *height) {
			*height = min_height;
			scaled = true;
		}
		if (max_height >= 0 && max_height < *height) {
			*height = max_height;
			scaled = true;
		}

		if (scaled && (intrinsic_height != 0)) {
			/* Update width */
			*width = (*height * intrinsic_width) /
					intrinsic_height;
		}

	} else if (*width == AUTO) {
		/* Have given height; width is calculated from the given height
		 * and ratio of intrinsic dimensions */
		int intrinsic_width = content_get_width(box->object);
		int intrinsic_height = content_get_height(box->object);

		if (intrinsic_height != 0)
			*width = (*height * intrinsic_width) /
					intrinsic_height;
		else
			*width = intrinsic_width;

		if (min_width >  0 && min_width > *width)
			*width = min_width;
		if (max_width >= 0 && max_width < *width)
			*width = max_width;

	} else if (*height == AUTO) {
		/* Have given width; height is calculated from the given width
		 * and ratio of intrinsic dimensions */
		int intrinsic_width = content_get_width(box->object);
		int intrinsic_height = content_get_height(box->object);

		if (min_width >  0 && min_width > *width)
			*width = min_width;
		if (max_width >= 0 && max_width < *width)
			*width = max_width;

		if (intrinsic_width != 0)
			*height = (*width * intrinsic_height) /
					intrinsic_width;
		else
			*height = intrinsic_height;
	}
}


/**
 * Calculate the text-indent length.
 *
 * \param  style  style of block
 * \param  width  width of containing block
 * \return  length of indent
 */
static int layout_text_indent(
		const css_unit_ctx *unit_len_ctx,
		const css_computed_style *style, int width)
{
	css_fixed value = 0;
	css_unit unit = CSS_UNIT_PX;

	css_computed_text_indent(style, &value, &unit);

	if (unit == CSS_UNIT_PCT) {
		return FPCT_OF_INT_TOINT(value, width);
	} else {
		return FIXTOINT(css_unit_len2device_px(style, unit_len_ctx,
				value, unit));
	}
}


/**
 * Calculate minimum and maximum width of a table.
 *
 * \param table box of type TABLE
 * \param font_func Font functions
 * \param content  The HTML content we are laying out.
 * \post  table->min_width and table->max_width filled in,
 *        0 <= table->min_width <= table->max_width
 */
static void layout_minmax_table(struct box *table,
		const struct gui_layout_table *font_func,
		const html_content *content)
{
	unsigned int i, j;
	int width;
	int border_spacing_h = 0;
	int table_min = 0, table_max = 0;
	int extra_fixed = 0;
	float extra_frac = 0;
	struct column *col;
	struct box *row_group, *row, *cell;

	/* check if the widths have already been calculated */
	if (table->max_width != UNKNOWN_MAX_WIDTH)
		return;

	if (table_calculate_column_types(&content->unit_len_ctx, table) == false) {
		NSLOG(netsurf, ERROR,
				"Could not establish table column types.");
		return;
	}
	col = table->col;

	/* start with 0 except for fixed-width columns */
	for (i = 0; i != table->columns; i++) {
		if (col[i].type == COLUMN_WIDTH_FIXED)
			col[i].min = col[i].max = col[i].width;
		else
			col[i].min = col[i].max = 0;
	}

	/* border-spacing is used in the separated borders model */
	if (css_computed_border_collapse(table->style) ==
			CSS_BORDER_COLLAPSE_SEPARATE) {
		css_fixed h = 0, v = 0;
		css_unit hu = CSS_UNIT_PX, vu = CSS_UNIT_PX;

		css_computed_border_spacing(table->style, &h, &hu, &v, &vu);

		border_spacing_h = FIXTOINT(css_unit_len2device_px(
				table->style,
				&content->unit_len_ctx,
				h, hu));
	}

	/* 1st pass: consider cells with colspan 1 only */
	for (row_group = table->children; row_group; row_group =row_group->next)
	for (row = row_group->children; row; row = row->next)
	for (cell = row->children; cell; cell = cell->next) {
		assert(cell->type == BOX_TABLE_CELL);
		assert(cell->style);
		assert(cell->columns != 0);

		if (cell->columns != 1)
			continue;

		layout_minmax_block(cell, font_func, content);
		i = cell->start_column;

		if (col[i].positioned)
			continue;

		/* update column min, max widths using cell widths */
		if (col[i].min < cell->min_width)
			col[i].min = cell->min_width;
		if (col[i].max < cell->max_width)
			col[i].max = cell->max_width;
	}

	/* 2nd pass: cells which span multiple columns */
	for (row_group = table->children; row_group; row_group =row_group->next)
	for (row = row_group->children; row; row = row->next)
	for (cell = row->children; cell; cell = cell->next) {
		unsigned int flexible_columns = 0;
		int min = 0, max = 0, fixed_width = 0, extra;

		if (cell->columns == 1)
			continue;

		layout_minmax_block(cell, font_func, content);
		i = cell->start_column;

		/* find min width so far of spanned columns, and count
		 * number of non-fixed spanned columns and total fixed width */
		for (j = 0; j != cell->columns; j++) {
			min += col[i + j].min;
			if (col[i + j].type == COLUMN_WIDTH_FIXED)
				fixed_width += col[i + j].width;
			else
				flexible_columns++;
		}
		min += (cell->columns - 1) * border_spacing_h;

		/* distribute extra min to spanned columns */
		if (min < cell->min_width) {
			if (flexible_columns == 0) {
				extra = 1 + (cell->min_width - min) /
						cell->columns;
				for (j = 0; j != cell->columns; j++) {
					col[i + j].min += extra;
					if (col[i + j].max < col[i + j].min)
						col[i + j].max = col[i + j].min;
				}
			} else {
				extra = 1 + (cell->min_width - min) /
						flexible_columns;
				for (j = 0; j != cell->columns; j++) {
					if (col[i + j].type !=
							COLUMN_WIDTH_FIXED) {
						col[i + j].min += extra;
						if (col[i + j].max <
								col[i + j].min)
							col[i + j].max =
								col[i + j].min;
					}
				}
			}
		}

		/* find max width so far of spanned columns */
		for (j = 0; j != cell->columns; j++)
			max += col[i + j].max;
		max += (cell->columns - 1) * border_spacing_h;

		/* distribute extra max to spanned columns */
		if (max < cell->max_width && flexible_columns) {
			extra = 1 + (cell->max_width - max) / flexible_columns;
			for (j = 0; j != cell->columns; j++)
				if (col[i + j].type != COLUMN_WIDTH_FIXED)
					col[i + j].max += extra;
		}
	}

	for (i = 0; i != table->columns; i++) {
		if (col[i].max < col[i].min) {
			box_dump(stderr, table, 0, true);
			assert(0);
		}
		table_min += col[i].min;
		table_max += col[i].max;
	}

	/* fixed width takes priority, unless it is too narrow */
	if (css_computed_width_px(table->style, &content->unit_len_ctx,
			-1, &width) == CSS_WIDTH_SET) {
		if (table_min < width)
			table_min = width;
		if (table_max < width)
			table_max = width;
	}

	/* add margins, border, padding to min, max widths */
	calculate_mbp_width(&content->unit_len_ctx,
			table->style, LEFT, true, true, true,
			&extra_fixed, &extra_frac);
	calculate_mbp_width(&content->unit_len_ctx,
			table->style, RIGHT, true, true, true,
			&extra_fixed, &extra_frac);
	if (extra_fixed < 0)
		extra_fixed = 0;
	if (extra_frac < 0)
		extra_frac = 0;
	if (1.0 <= extra_frac)
		extra_frac = 0.9;
	table->min_width = (table_min + extra_fixed) / (1.0 - extra_frac);
	table->max_width = (table_max + extra_fixed) / (1.0 - extra_frac);
	table->min_width += (table->columns + 1) * border_spacing_h;
	table->max_width += (table->columns + 1) * border_spacing_h;

	assert(0 <= table->min_width && table->min_width <= table->max_width);
}

/**
 * Helper to check if a box has percentage max width.
 *
 * \param[in]  b  Box to check.
 * \return true iff box has percnetage max width.
 */
static inline bool box_has_percentage_max_width(struct box *b)
{
	css_unit unit = CSS_UNIT_PX;
	enum css_max_width_e type;
	css_fixed value = 0;

	assert(b != NULL);

	type = css_computed_max_width(b->style, &value, &unit);
	return ((type == CSS_MAX_WIDTH_SET) && (unit == CSS_UNIT_PCT));
}

/**
 * Calculate minimum and maximum width of a line.
 *
 * \param first       a box in an inline container
 * \param line_min    updated to minimum width of line starting at first
 * \param line_max    updated to maximum width of line starting at first
 * \param first_line  true iff this is the first line in the inline container
 * \param line_has_height  updated to true or false, depending on line
 * \param font_func Font functions.
 * \return  first box in next line, or 0 if no more lines
 * \post  0 <= *line_min <= *line_max
 */
static struct box *
layout_minmax_line(struct box *first,
		   int *line_min,
		   int *line_max,
		   bool first_line,
		   bool *line_has_height,
		   const struct gui_layout_table *font_func,
		   const html_content *content)
{
	int min = 0, max = 0, width, height, fixed;
	float frac;
	size_t i, j;
	struct box *b;
	struct box *block;
	plot_font_style_t fstyle;
	bool no_wrap;

	assert(first->parent);
	assert(first->parent->parent);
	assert(first->parent->parent->style);

	block = first->parent->parent;
	no_wrap = (css_computed_white_space(block->style) ==
			CSS_WHITE_SPACE_NOWRAP ||
			css_computed_white_space(block->style) ==
			CSS_WHITE_SPACE_PRE);

	*line_has_height = false;

	/* corresponds to the pass 1 loop in layout_line() */
	for (b = first; b; b = b->next) {
		enum css_width_e wtype;
		enum css_height_e htype;
		enum css_box_sizing_e bs;
		css_fixed value = 0;
		css_unit unit = CSS_UNIT_PX;

		assert(lh__box_is_inline_content(b));

		NSLOG(layout, DEBUG, "%p: min %i, max %i", b, min, max);

		if (b->type == BOX_BR) {
			b = b->next;
			break;
		}

		if (lh__box_is_float_box(b)) {
			assert(b->children);
			if (b->children->type == BOX_TABLE)
				layout_minmax_table(b->children, font_func,
						content);
			else
				layout_minmax_block(b->children, font_func,
						content);
			b->min_width = b->children->min_width;
			b->max_width = b->children->max_width;
			if (min < b->min_width)
				min = b->min_width;
			max += b->max_width;
			continue;
		}

		if (b->type == BOX_INLINE_BLOCK || b->type == BOX_INLINE_FLEX) {
			layout_minmax_block(b, font_func, content);
			if (min < b->min_width)
				min = b->min_width;
			max += b->max_width;

			if (b->flags & HAS_HEIGHT)
				*line_has_height = true;
			continue;
		}

		assert(b->style);
		font_plot_style_from_css(&content->unit_len_ctx, b->style, &fstyle);

		if (b->type == BOX_INLINE && !b->object &&
				!(b->flags & REPLACE_DIM) &&
				!(b->flags & IFRAME)) {
			fixed = frac = 0;
			calculate_mbp_width(&content->unit_len_ctx,
					b->style, LEFT, true, true, true,
					&fixed, &frac);
			if (!b->inline_end)
				calculate_mbp_width(&content->unit_len_ctx,
						b->style, RIGHT,
						true, true, true,
						&fixed, &frac);
			if (0 < fixed)
				max += fixed;
			*line_has_height = true;
			/* \todo  update min width, consider fractional extra */
		} else if (b->type == BOX_INLINE_END) {
			fixed = frac = 0;
			calculate_mbp_width(&content->unit_len_ctx,
					b->inline_end->style, RIGHT,
					true, true, true,
					&fixed, &frac);
			if (0 < fixed)
				max += fixed;

			if (b->next) {
				if (b->space == UNKNOWN_WIDTH) {
					font_func->width(&fstyle, " ", 1,
							 &b->space);
				}
				max += b->space;
			}

			*line_has_height = true;
			continue;
		}

		if (lh__box_is_replace(b) == false) {
			/* inline non-replaced, 10.3.1 and 10.6.1 */
			bool no_wrap_box;
			if (!b->text)
				continue;

			no_wrap_box = (css_computed_white_space(b->style) ==
					CSS_WHITE_SPACE_NOWRAP ||
					css_computed_white_space(b->style) ==
					CSS_WHITE_SPACE_PRE);

			if (b->width == UNKNOWN_WIDTH) {
				/** \todo handle errors */

				/* If it's a select element, we must use the
				 * width of the widest option text */
				if (b->parent->parent->gadget &&
						b->parent->parent->gadget->type
						== GADGET_SELECT) {
					int opt_maxwidth = 0;
					struct form_option *o;

					for (o = b->parent->parent->gadget->
							data.select.items; o;
							o = o->next) {
						int opt_width;
						font_func->width(&fstyle,
								o->text,
								strlen(o->text),
								&opt_width);

						if (opt_maxwidth < opt_width)
							opt_maxwidth =opt_width;
					}

					b->width = opt_maxwidth;
					if (nsoption_bool(core_select_menu))
						b->width += SCROLLBAR_WIDTH;

				} else {
					font_func->width(&fstyle, b->text,
						b->length, &b->width);
					b->flags |= MEASURED;
				}
			}
			max += b->width;
			if (b->next) {
				if (b->space == UNKNOWN_WIDTH) {
					font_func->width(&fstyle, " ", 1,
							 &b->space);
				}
				max += b->space;
			}

			if (no_wrap) {
				/* Don't wrap due to block style,
				 * so min is the same as max */
				min = max;

			} else if (no_wrap_box) {
				/* This inline box can't be wrapped,
				 * for min, consider box's width */
				if (min < b->width)
					min = b->width;

			} else if (b->parent->flags & NEED_MIN) {
				/* If we care what the minimum width is,
				 * calculate it.  (It's only needed if we're
				 * shrinking-to-fit.) */
				/* min = widest single word */
				i = 0;
				do {
					for (j = i; j != b->length &&
							b->text[j] != ' '; j++)
						;
					font_func->width(&fstyle, b->text + i,
							 j - i, &width);
					if (min < width)
						min = width;
					i = j + 1;
				} while (j != b->length);
			}

			*line_has_height = true;

			continue;
		}

		/* inline replaced, 10.3.2 and 10.6.2 */
		assert(b->style);

		bs = css_computed_box_sizing(block->style);

		/* calculate box width */
		wtype = css_computed_width_px(b->style,
				&content->unit_len_ctx, -1, &width);
		if (wtype == CSS_WIDTH_SET) {
			if (bs == CSS_BOX_SIZING_BORDER_BOX) {
				fixed = frac = 0;
				calculate_mbp_width(&content->unit_len_ctx,
						block->style, LEFT,
						false, true, true,
						&fixed, &frac);
				calculate_mbp_width(&content->unit_len_ctx,
						block->style, RIGHT,
						false, true, true,
						&fixed, &frac);
				if (width < fixed) {
					width = fixed;
				}
			}
			if (width < 0)
				width = 0;
		} else {
			width = AUTO;
		}

		/* height */
		htype = css_computed_height(b->style, &value, &unit);
		if (htype == CSS_HEIGHT_SET) {
			height = FIXTOINT(css_unit_len2device_px(
					b->style,
					&content->unit_len_ctx,
					value, unit));
		} else {
			height = AUTO;
		}

		if (b->object || (b->flags & REPLACE_DIM)) {
			if (b->object) {
				int temp_height = height;
				layout_get_object_dimensions(b,
						&width, &temp_height,
						INT_MIN, INT_MAX,
						INT_MIN, INT_MAX);
			}

			fixed = frac = 0;
			if (bs == CSS_BOX_SIZING_BORDER_BOX) {
				calculate_mbp_width(&content->unit_len_ctx,
						b->style, LEFT,
						true, false, false,
						&fixed, &frac);
				calculate_mbp_width(&content->unit_len_ctx,
						b->style, RIGHT,

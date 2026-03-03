# Layout Engine Audit and Remediation

This document tracks the technical debt, TODOs, and completed improvements within the NetSurf layout engine.

## Remaining TODOs

The following items are identified in the source code as requiring further attention:

| File | Line | Description |
| ---- | ---- | ----------- |
| `content/handlers/html/box_construct.c` | 421 | Improve URL handling (LWC string vs nsurl) for list markers. |
| `content/handlers/html/box_construct.c` | 693 | Improve URL handling (LWC string vs nsurl) for background images. |
| `content/handlers/html/box_construct.c` | 1119 | Proper handling of tabs in text nodes. |
| `content/handlers/html/html.c` | 1797 | Sniff for text content in certain elements. |
| `content/handlers/html/css.c` | 677 | Pass sheet in anyway. |
| `content/handlers/html/box_special.c` | 717 | Verify certain special element conversions. |
| `content/handlers/html/box_special.c` | 1043 | Handle hidden iframes at browser_window generation. |
| `content/handlers/html/object.c` | 111 | Mapping for other box types. |
| `content/handlers/html/object.c` | 179 | Avoid knowledge of box internals in object handling. |

## Completed Improvements

The following limitations have been addressed:

### Flexbox
- **Visual Ordering**: Implemented the `order` property by physically reordering the layout box tree. This ensures correct painting order (z-index) and visual layout.
- **Justify Content**: Full support for `flex-start`, `flex-end`, `center`, `space-between`, `space-around`, and `space-evenly`.
- **Align Content**: Full support for multi-line distribution modes.
- **Cross-axis Stretching**: Enhanced `stretch` alignment to respect `min/max` constraints and trigger necessary item re-layouts.
- **Constraints**: Added `min_cross` and `max_cross` support to `flex_item_data` to ensure cross-axis size constraints are respected during alignment.

### Table Layout
- **Vertical Spare Height Distribution**: Tables with a specified `height` or `min-height` now distribute excess space among rows proportionally.
- **Row-Spanning Logic**: Fixed `rowspan` height summation and distribution across spanned rows.
- **Intrinsic Sizing**: Corrected height calculation for empty tables or row groups.
- **Safety**: Added guards against division-by-zero and out-of-bounds access in distribution logic.
- **Colspan "0"**: Enhanced support for `colspan="0"` to correctly span remaining columns in a row group.
- **Normalization**: Reset column span information at the start of each row group to prevent `rowspan` from crossing row group boundaries.

### Inline Layout
- **Text Indent**: Fixed interaction with horizontal floats. The indent is now applied as a content offset to the first inline box on a line, ensuring spec-compliant additive behavior.
- **Line Height Safety**: Implemented a fallback height (13pt) for anonymous boxes with NULL styles to ensure valid line spacing.

### General Layout
- **Percentage Heights**: Fixed resolution logic for `max-height` and `min-height` properties.
- **Intrinsic Height**: Blocks with fixed `min-height` now correctly contribute to parent height even if empty.
- **Box Sizing**: Correctly applied `box-sizing: border-box` adjustments to `min-height` and `max-height`.
- **IFRAME Sizing**: Improved dimension resolution for IFRAMEs with percentage widths/heights.
- **Traversal Order**: Improved `html_scroll_at_point` to visit the deepest box first by traversing to the leaf and iterating backwards.
- **CSS Handling**: Implemented filtering to load only the first preferred stylesheets and ensured the full media query string is passed to the selection context.
- **Interaction Feedback**: Enabled status bar feedback for interactions (e.g., textarea mouse actions) using localized messages.
- **Performance**: Refactored dimension resolution to use a shared inline cache-checking wrapper.

---
*Last updated: November 2024*

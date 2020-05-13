#include <u.h>
#include <libc.h>
#include <draw.h>
#include <mouse.h>
#include <nuklear.h>

#include "altid.h"

#define WIN_MIN_SIZE 120
/* nuklear - v1.00 - public domain */

void
md_ui(struct nk_context *ctx, Data *d) {
	if (nk_begin(ctx, "ubqt", nk_rect(0, 0, Dx(screen->r), Dy(screen->r)), NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR)) {
		// tabs 
		
		// title + menu layout
		nk_layout_row_template_begin(ctx, 0);
		nk_layout_row_template_push_variable(ctx, WIN_MIN_SIZE - 40);
		nk_layout_row_template_push_static(ctx, 40);
		nk_layout_row_template_end(ctx);
		nk_text(ctx, d->title, 27, NK_TEXT_LEFT);
		if (nk_button_label(ctx, "MENU")) {
			;
			/* Toggle navbar, toggle sidebar, quit, etc */
		}
		// navibar | Main document body | sidebar
		//statusbar
		nk_layout_row_dynamic(ctx, 0, 1);
		nk_text(ctx, "beef", 10, NK_TEXT_LEFT);
		// input 
	}
	nk_end(ctx);
}


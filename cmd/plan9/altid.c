#include <u.h>
#include <libc.h>
#include <stdio.h>

#include <draw.h>
#include <mouse.h>
#include <nuklear.h>
#include <bio.h>
#include "altid.h"

/* knobs */
int backlog = 5000;
int maxlines = 10;
int sleeptime = 500;
int mainstacksize = 128*1024;

static void
kbdthread(void *v)
{
	int fd;
	long n;
	Channel *c;
	Ioproc *io;
	IOchunk chk;

	c = v;
	io = ioproc();

	if((fd = ioopen(io, "/dev/kbd", OREAD)) < 0)
		sysfatal("open: %r");

	for(;;){
		chk.addr = malloc(64);
		if(chk.addr == nil)
			sysfatal("malloc: %r");

		n = ioread(io, fd, chk.addr, 64);
		if(n < 0)
			sysfatal("read: %r");

		chk.len = n;

		send(c, &chk);
	}
}

void
usage(void)
{
	fprint(2, "usage: %s [-n maxlines] [-d ms]\n", argv0);
	threadexitsall("usage");
}

void
threadmain(int argc, char *argv[])
{
	IOchunk kbdchk;
	Channel *kbd, *dctl;
	Data *dat;
	Message msg;
	Mouse m;
	Mousectl *mctl;

	struct nk_user_font nkfont;
	struct nk_context sctx;
	struct nk_context *ctx = &sctx;

	ARGBEGIN{
	case 'n':
		maxlines = atoi(EARGF(usage()));
		break;
	case 'd':
		sleeptime = atoi(EARGF(usage()));
		break;
	default:
		usage();
	}ARGEND

	if(initdraw(nil, nil, "altid") < 0)
		sysfatal("initdraw: %r");

	kbd = chancreate(sizeof(IOchunk), 0);

	threadcreate(kbdthread, kbd, 8192);

	dctl = chancreate(sizeof(Event), 0);
	mctl = initmouse(nil, screen);
	if(mctl == nil)
		sysfatal("initmouse: %r");

	/* create nuklear font from default libdraw font */
	nk_plan9_makefont(&nkfont, font);

	/* initialize nuklear */
	if(!nk_init_default(ctx, &nkfont))
		sysfatal("nk_init: %r");


	/* Initialize Data */
	if(!md_init_data(argv[1], &dat))
		sysfatal("md_init_data: %r");

	enum { ADATA, AKBD, AMOUSE, ARESIZE, AEND };
	Alt alts[] = {
		[ADATA]		{ dctl,			&msg,		CHANRCV },
		[AKBD]		{ kbd,			&kbdchk,	CHANRCV },
		[AMOUSE]	{ mctl->c,		&m,		CHANRCV },
		[ARESIZE]	{ mctl->resizec,	nil,		CHANRCV },
		[AEND]		{ nil,			nil,		CHANEND },
	};

	for(;;){
		nk_input_begin(ctx);
		switch(alt(alts)){
		case ADATA:
			switch(msg.type){
			case DTabs:
				dat->tabs = strdup(msg.data);
			case DStatus:
				dat->status = strdup(msg.data);
			case DAside:
				dat->aside = strdup(msg.data);
			case DNavi:
				dat->navi = strdup(msg.data);
			case DTitle:
				dat->title = strdup(msg.data);
			case DMain:
				dat->page = strdup(msg.data);
			case DFeed:
				snprintf(dat->page, backlog, "%s\n%s", dat->page, msg.data);
			}
			break;
		case AKBD:
			nk_plan9_handle_kbd(ctx, kbdchk.addr, kbdchk.len);
			free(kbdchk.addr);
			kbdchk.addr = nil;
			break;
		case AMOUSE:
			nk_plan9_handle_mouse(ctx, m, screen->r.min);
			break;
		case ARESIZE:
			if(getwindow(display, Refnone) < 0)
				sysfatal("getwindow: %r");
		}

		nk_input_end(ctx);

		/* handle usual plan 9 exit key */
		if(nk_input_is_key_down(&ctx->input, NK_KEY_DEL))
			threadexitsall(nil);
		if(nk_input_is_key_down(&ctx->input, NK_KEY_ENTER)){
			md_input(&dat->input);
		}
		//md_ui(ctx, dat);

		/* render everything */
		draw(screen, screen->r, display->black, nil, ZP);
		nk_plan9_render(ctx, screen);
		flushimage(display, 1);
	}
}

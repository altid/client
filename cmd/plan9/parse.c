#include <u.h>
#include <libc.h>
#include <String.h>
#include <bio.h>
#include "altid.h"

struct md_state {
	Channel *item;
	char *path;
	int tokentype;
	int count;
	int (*fn)(String *curr, Biobuf *src, struct md_state *s);
};

int md_url(String *curr, Biobuf *src, struct md_state *s);
int md_bold(String *curr, Biobuf *src, struct md_state *s);
int md_image(String *curr, Biobuf *src, struct md_state *s);
int md_color(String *curr, Biobuf *src, struct md_state *s);
int md_normal(String *curr, Biobuf *src, struct md_state *s);
int md_strike(String *curr, Biobuf *src, struct md_state *s);
int md_emphasis(String *curr, Biobuf *src, struct md_state *s);
int md_maybe_url(String *curr, Biobuf *src, struct md_state *s);
int md_underline(String *curr, Biobuf *src, struct md_state *s);
int md_colorcode(String *curr, Biobuf *src, struct md_state *s);
int md_image_url(String *curr, Biobuf *src, struct md_state *s);
int md_maybe_color(String *curr, Biobuf *src, struct md_state *s);
int md_maybe_image(String *curr, Biobuf *src, struct md_state *s);


void
md_runmachine(void *v){
	struct md_state *s;
	Biobuf *src;
	String *curr; 
	s = v;
	src = Bopen(s->path, OREAD);

	curr = s_new();

	/* Set initial function */
	s->fn = (*md_normal);
	s->tokentype = ENormalText;

	while((s->fn(curr, src, s) > 0));
	chanclose(s->item);
}

int
md_accept(Biobuf *src, String *curr, char *run){
	char c;
	c = Bgetc(src);
	do {
		if (run[0] == c) {
			s_putc(curr, c);
			return 1;
		}
	} while(*run++);
	Bungetc(src);
	return 0;
}

void
md_emit(String *curr, struct md_state *s){
	s_terminate(curr);
	if (s_len(curr) < 1){
		/* Exit early if we have no data */
		return;
	}
	s->count++;
	Token *t;
	t = malloc(sizeof(Token));
	t->data = strdup(s_to_c(curr));
	t->type = s->tokentype;
	t->counter = s->count;
	s_reset(curr);
	send(s->item, t);
	free(t);
}

Channel *
md_tokenize(char *path){
	Channel *tokens;
	struct md_state *s;
	tokens = chancreate(sizeof(struct Token), 0);
	s = malloc(sizeof(struct md_state));
	s->path = strdup(path);
	s->item = tokens;
	s->count = 0;
	threadcreate(md_runmachine, s, 4096);
	return tokens;
}

int
md_normal(String *curr, Biobuf *src, struct md_state *s){
	s->tokentype = ENormalText;
	char c;
	for(;;){
		c = Bgetc(src);
		switch(c){
		case -1:
			md_emit(curr, s);
			return 0;
		case '\\':
			md_accept(src, curr, "\\!#([])*_-~`");
			continue;
		case '%':
			s->fn = (*md_maybe_color);
			return 1;
		case '[':
			s->fn = (*md_maybe_url);
			return 1;
		case '!':
			s->fn = (*md_maybe_image);
			return 1;
		case '*':
			s->fn = (*md_bold);
			return 1;
		case '-':
			s->fn = (*md_emphasis);
			return 1;
		case '~':
			s->fn = (*md_strike);
			return 1;
		case '_':
			s->fn = (*md_underline);
			return 1;
		}
		if(Bbuffered(src) <= 1){
			return 0;
		}
		s_putc(curr, c);
	}
}

int
md_maybe_color(String *curr, Biobuf *src, struct md_state *s){
	char c = Bgetc(src);
	switch(c){
	case -1:
		md_emit(curr, s);
		s_reset(curr);
		return 0;
	case '[':
		md_emit(curr, s);
		s_reset(curr);
		s->fn = (*md_color);
		return 1;
	default:
		s_putc(curr, '[');
		s->fn = (*md_normal);
		return 1;
	}
}

int
md_color(String *curr, Biobuf *src, struct md_state *s){
	s->tokentype = EColorText;
	char c;
	for(;;){
		c = Bgetc(src);
		switch(c){
		case -1:
			md_emit(curr, s);
			return 0;
		case '[':
			continue;
		case ']':
			md_emit(curr, s);
			s->fn = (*md_colorcode);
			return 1;
		case '\\':
			md_accept(src, curr, "\!#([])*_-~`");
			continue;
		}
		if (Bbuffered(src) <= 1){
			return 0;
		}
		s_putc(curr, c);

	}
}

int
md_colorcode(String *curr, Biobuf *src, struct md_state *s){
	s->tokentype = EColorName;
	md_accept(src, curr, "(");
	s_reset(curr);
	while(md_accept(src, curr, "1234567890ABCDEFabcdefghijklmnopqrstuvwxyz,"));
	md_emit(curr, s);
	md_accept(src, curr, ")");
	s_reset(curr);
	s->fn = (*md_normal);
	return 1;
}

int
md_maybe_url(String *curr, Biobuf *src, struct md_state *s){
	char c = Bgetc(src);
	switch(c){
	case -1:
		md_emit(curr, s);
		return 0;
	case '!':
		md_emit(curr, s);
		s->fn = (*md_image_url);
		return 1;
	default:
		s_putc(curr, '!');
		s->fn = (*md_url);
		return 1;
	}
}

int
md_url(String *curr, Biobuf *src, struct md_state *s){
	USED(curr); USED(src); USED(s);
	return 0;
}

int
md_image_url(String *curr, Biobuf *src, struct md_state *s){
	USED(curr); USED(src); USED(s);
	return 0;
}

int
md_maybe_image(String *curr, Biobuf *src, struct md_state *s){
	char c = Bgetc(src);
	switch(c){
	case -1:
		md_emit(curr, s);
		return 0;
	case '[':
		md_emit(curr, s);
		s->fn = (*md_image);
		return 1;
	default:
		s_putc(curr, '[');
		s->fn = (*md_normal);
		return 1;
	}
}

int
md_image(String *curr, Biobuf *src, struct md_state *s){
	USED(curr); USED(src); USED(s);
	return 0;
}

int
md_bold(String *curr, Biobuf *src, struct md_state *s){
	s->tokentype = EBoldText;
	md_emit(curr, s);
	for(;;){
		char c = Bgetc(src);
		switch(c){
		case -1:
			md_emit(curr, s);
			return 0;
		case '*':
			md_emit(curr, s);
			s->fn = (*md_normal);
			return 1;
		}
		if (Bbuffered(src) <= 1){
			return 0;
		}
		s_putc(curr, c);
	}
}

int
md_emphasis(String *curr, Biobuf *src, struct md_state *s){
	md_emit(curr, s);
	s->tokentype = EEmText;
	for(;;){
		char c = Bgetc(src);
		switch(c){
		case -1:
			md_emit(curr, s);
			return 0;
		case '-':
			md_emit(curr, s);
			s->fn = (*md_normal);
			return 1;
		}
		if(Bbuffered(src) <= 1){
			return 0;
		}
		s_putc(curr, c);
	}
}

int
md_strike(String *curr, Biobuf *src, struct md_state *s){
	s->tokentype = EStrikeText;
	md_emit(curr, s);
	for(;;){
		char c = Bgetc(src);
		switch(c){
		case -1:
			md_emit(curr, s);
			return 0;
		case '~':
			md_emit(curr, s);
			s->fn = (*md_normal);
			return 1;
		}
		if(Bbuffered(src) <= 1){
			return 0;
		}
		s_putc(curr, c);
	}
}

int
md_underline(String *curr, Biobuf *src, struct md_state *s){
	s->tokentype = EUnderlineText;
	md_emit(curr, s);
	for(;;){
		char c = Bgetc(src);
		switch(c){
		case -1:
			md_emit(curr, s);
			return 0;
		case '_':
			md_emit(curr, s);
			s->fn = (*md_normal);
			return 1;
		}
		if(Bbuffered(src) <= 1){
			return 0;
		}
		s_putc(curr, c);
	}
}

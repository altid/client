#include <u.h>
#include <libc.h>

#include "altid.h"

void
md_ctrl(char *i){
	if(strcmp(i, "quit") == 0){
		threadexitsall(nil);
	}
}

void
md_input(char **i){
	if (strlen(*i) < 1){
		return;
	}
	if (*i[0] == '/'){
		md_ctrl(*i+1);
		memset(*i, 0, 256);
		return;
	}
}

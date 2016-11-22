#include "ubqt.h"

/* We have data on a fd, update our internal structure */
void
ubqt_update_data() {
		// If we have new element, make sure we are drawing it
}

/* We have input on stdin, update our internal structure */
void
ubqt_update_input() {
		// Figure out which element we're on, 
		// Figure out which mode, if applicable
		// Input mode - input
		// Normal mode - further processing
}

void
ubqt_remove_data() {
		
}
/*
char *buf = NULL;
FILE *fp;
int str_sz;

fp = fopen(path, "r");

if (fp) {
		fseek(fp, 0, SEEK_END);
		str_sz = ftell(fp);
		rewind(fp);
		buf = (char*) malloc(sizeof(char) * (str_sz + 1));
		fread(buf, sizeof(char), str_sz, fp);
		buf[str_sz] = '\0';
		fclose(fp);
}
*/

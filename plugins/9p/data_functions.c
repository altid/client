#include "c9/c9.h"
#include "../../src/ubqt.h"

struct C9ctx *c;

int
ubqt_data_init(char *path)
{

	
	//auth as default:default
	//9p data connection goes here
	return 0;

}

int
ubqt_data_write(char *name, char *buffer)
{

	//Send Rwrite
	return 0;

}

char *
ubqt_data_read(char *name, char *path)
{

	//Send Rread
	//-codeblock- ignore
	return "test";

}

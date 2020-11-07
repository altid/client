#include <stdlib.h>
#include "altid.h"

Buffer *
list_tabs(void)
{
    return NULL;
}

void
swap(Buffer *buff)
{
    Buffer *tmp;
    while(buff != NULL){
        tmp = buff;
        buff = buff->next;
        free(buff);
    }
}

void
freebuff(Buffer *buff)
{

}


#include <stdlib.h>
#include "altid.h"

void
freeservice(Service *service)
{
    Service *tmp;
    while(service != NULL){
        tmp = service;
        service = service->next;
        free(service);
    }
}
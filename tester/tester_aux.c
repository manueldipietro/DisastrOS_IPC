#include "tester_aux.h"
#include "tester.h"

#include "disastrOS.h"

#include <stdio.h>

void tester_aux_sleeper(){
    while(1) {getc(stdin);}
    return;
}
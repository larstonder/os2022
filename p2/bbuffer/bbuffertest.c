#include <stdio.h>
#include <stdlib.h>
#include "bbuffer.h"

int main() {
    BNDBUF *bb = bb_init(5);

    bb_add(bb,12);
    bb_add(bb,1134);
    bb_add(bb,13);
    bb_add(bb,542);
    bb_add(bb,-213);
    printf("t1 = %d\n", bb_get(bb));
    printf("t2 = %d\n", bb_get(bb));
    printf("t3 = %d\n", bb_get(bb));
    printf("t4 = %d\n", bb_get(bb));
    printf("t5 = %d\n", bb_get(bb));

    bb_add(bb,212);
    bb_add(bb,21134);
    bb_add(bb,-2213);
    printf("u1 = %d\n", bb_get(bb));
    printf("u2 = %d\n", bb_get(bb));
    printf("u3 = %d\n", bb_get(bb));

    return EXIT_SUCCESS;
}
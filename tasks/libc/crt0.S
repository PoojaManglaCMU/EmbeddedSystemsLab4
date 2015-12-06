@ crt0.S: Execution startup routine
@
@ Authors: Soumyaroop Dutta
@ Date:   11th october 11.30 pm
#include <bits/swi.h>

        .file   "crt0.S"
        .text

        .global _start
        .global main

_start:
        @load the argc to 1st register.
        ldr   r0, [sp], #4
        @post indexing, makes to point it to
        @the next pointer. load it to r1. argv[][]
        mov   r1, sp
LOOP:
        bl    main
	b     LOOP

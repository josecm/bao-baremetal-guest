#ifndef WFI_H
#define WFI_H

#pragma inline_asm wfi
static void wfi() {
    snooze
}

#endif

/** 
 * Bao, a Lightweight Static Partitioning Hypervisor 
 *
 * Copyright (c) Bao Project (www.bao-project.org), 2019-
 *
 * Authors:
 *      Jose Martins <jose.martins@bao-project.org>
 *      Sandro Pinto <sandro.pinto@bao-project.org>
 *
 * Bao is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 2 as published by the Free
 * Software Foundation, with a special exception exempting guest code from such
 * license. See the COPYING file in the top-level directory for details. 
 *
 */

#include <core.h>
#include <stdlib.h>
#include <stdio.h>
#include <cpu.h>
#include <zdma.h>
#include <spinlock.h>
#include <timer.h>
#include <wfi.h>

struct zdma_ch_hw *zdma_ch_0 = (void*)0x00FFA80000;
struct zdma_ch_hw *zdma_ch_1 = (void*)0x00FFA90000;
struct zdma_ch_hw *zdma_ch_2 = (void*)0x00FFAA0000;

#define DMA_SIZE (64)
__attribute__((aligned(DMA_SIZE))) const char dma_src0[DMA_SIZE] = "success!";
__attribute__((aligned(DMA_SIZE))) const char dma_src1[DMA_SIZE] = "success!";
__attribute__((aligned(DMA_SIZE))) const char dma_src2[DMA_SIZE] = "success!";
__attribute__((aligned(DMA_SIZE))) char dma_dst0[DMA_SIZE*4] = "failed!";
__attribute__((aligned(DMA_SIZE))) char dma_dst1[DMA_SIZE*4] = "failed!";
__attribute__((aligned(DMA_SIZE))) char dma_dst2[DMA_SIZE*4] = "failed!";

void abort() {
    printf("abort!\n");
    printf("far: 0x%lx\n", sysreg_far_el1_read());
    printf("esr: 0x%lx\n", sysreg_esr_el1_read());
    while (true) { }
}

void dma_test(void) {

    printf("Starting DMA0 transfer: %s\n", dma_src0);
    zdma_ch_init(zdma_ch_0, ZDMA_MODE_SIMPLE);
    zdma_ch_set_src_addr(zdma_ch_0, (uintptr_t) dma_src0);
    zdma_ch_set_dst_addr(zdma_ch_0, (uintptr_t) dma_dst0);
    zdma_ch_set_size(zdma_ch_0, DMA_SIZE);
    zdma_ch_start(zdma_ch_0);
    zdma_ch_poll_end(zdma_ch_0);
    printf("Finished DMA0 transfer. Destination buffer: %s\n", dma_dst0);

    printf("\n");

    // printf("Starting DMA1 transfer: %s\n", dma_src1);
    // zdma_ch_init(zdma_ch_1, ZDMA_MODE_SIMPLE);
    // zdma_ch_set_src_addr(zdma_ch_1, (uintptr_t) dma_src1);
    // zdma_ch_set_dst_addr(zdma_ch_1, (uintptr_t) dma_dst1);
    // zdma_ch_set_size(zdma_ch_1, DMA_SIZE);
    // zdma_ch_start(zdma_ch_1);
    // zdma_ch_poll_end(zdma_ch_1);
    // printf("Finished DMA1 transfer. Destination buffer: %s\n", dma_dst1);

    // printf("\n");

    // printf("Starting DMA2 transfer: %s\n", dma_src2);
    // zdma_ch_init(zdma_ch_2, ZDMA_MODE_SIMPLE);
    // zdma_ch_set_src_addr(zdma_ch_2, (uintptr_t) dma_src2);
    // zdma_ch_set_dst_addr(zdma_ch_2, (uintptr_t) dma_dst2);
    // zdma_ch_set_size(zdma_ch_2, DMA_SIZE);
    // zdma_ch_start(zdma_ch_2);
    // zdma_ch_poll_end(zdma_ch_2);
    // printf("Finished DMA2 transfer. Destination buffer: %s\n", dma_dst2);
}


spinlock_t print_lock = SPINLOCK_INITVAL;

void main(void){

    if (!cpu_is_master()) {
        return;
    }

    dma_test();

    while(true) {
        wfi();
    }
}

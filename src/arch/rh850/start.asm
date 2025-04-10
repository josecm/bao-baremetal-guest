;
; SPDX-License-Identifier: Apache-2.0
; Copyright (c) Bao Project and Contributors. All rights reserved.
;

CPU_MASTER .set  0
STACK_SIZE .set  0x100 ; 1KiB

.section ".text", text
.align	2
    .public _start
_start:
    di ; Disable interrupts

    ; get current CPU
    ; r5 = PEID
    stsr 0, r5, 2 ; get PEID (regID 0, selID 2)

stall:
    cmp r0, r5
    bne stall

    mov 0x3f08020, r2
    ldsr r2, 5, 0 ; set PSW.EBV

    mov #_vector_table, r2
    ori 0x2, r2, r2 ; select direct vector method
    ldsr r2, 3, 1 ; set EBASE (regID 3, selID 1)

    mov #_interrupt_table, r2
    ldsr r2, 4, 1 ; set INTBP (regID 4, selID 1)

    ; disable memory protections
    mov r0, r2 ; MPM.MPE (and all else) disabled
    ldsr r2, 0, 5 ; set MPM

    ;; cover all memory with protection check
    ldsr r0, 8, 5 ; Set the address to MCA
    ldsr r0, 9, 5 ; Set the size to MCS

    ;; individual protections per cpu TODO should it be otherwise?
    ldsr r0, 12, 5 ; set MCI to 0
    ldsr r0, 0, 1 ; set SPID host SPID 0

    jarl clear_mpu, lp

    ; ; initialize local RAM
    ; mov 0xFDE00000, r20
    ; mov 0xFDE0FFFF, r21
    ; jarl _ram_init, lp

    ; enable faults ?

    ; check if current CPU is CPU_MASTER
    mov CPU_MASTER, r10 ; TODO: get value from CPU_MASTER_FIXED
    cmp r5, r10
    bne skip

    ; ; initialize cluster RAM
    ; mov 0xFE000000, r20
    ; mov 0xFE07FFFF, r21
    ; jarl _ram_init, lp

    ; mov 0xFE100000, r20
    ; mov 0xFE17FFFF, r21
    ; jarl _ram_init, lp

    ; mov 0xFE400000, r20
    ; mov 0xFE5FFFFF, r21
    ; jarl _ram_init, lp

    ; mov 0xFE800000, r20
    ; mov 0xFE83FFFF, r21
    ; jarl _ram_init, lp
    
    ; copy non .text segments to ram
    mov #__s.data, r20
    mov #__e.data, r21 ; need to copy until
    mov 0xfe000000, r22 ; TODO hopefully use the linker otherwise macro
    ;; copy from [r20] until [r21] to [r22]
    jarl copy_data, lp

    ; clear .bss
    ;; .bss start
    mov #__s.bss, r20
    mov #__e.bss, r21
    ;; clear from [r20] to [r21]
    jarl boot_clear, lp

skip:
    ; Initialize stack pointer
    mov #_stack_base, r20
    mov STACK_SIZE, r21
    add r21, r20
    mulh r5, r21
    add r21, r20
    mov r20, sp

    br __init

; ; r20: start of region
; ; r21: end of region
; _ram_init:
;     br _ram_init_2
; _ram_init_1:
;     st.w r0, 0[r20]
;     add 4, r20
; _ram_init_2:
;     cmp r20, r21
;     bh _ram_init_1
;     jmp [lp]

; r20: start of region
; r21: end of region
boot_clear:
boot_clear_1:
    cmp r21, r20
    bge boot_clear_exit
    st.w r0, 0[r20]     ; Store 0 to clear memory
    addi 4, r20, r20
    br boot_clear_1
boot_clear_exit:
    jmp [lp]


; r20: start of the source region
; r21: end of the source region
; r22: start of the destination region
; uses r23
copy_data:
copy_data_1:
    ld.w 0[r20], r23
    st.w r23, 0[r22]
    addi 4, r20, r20
    addi 4, r22, r22
    cmp r20, r21
    bne copy_data_1
    jmp [lp]


clear_mpu:
    mov r0, r20
clear_mpu_1:
    cmp 32, r20  ; TODO # of mpu entries platform defined
    be clear_mpu_exit

    ldsr r20, 16, 5 ; set MPIDX

    ldsr r0, 20, 5 ; set MPLA
    ldsr r0, 21, 5 ; set MPUA
    ldsr r0, 22, 5 ; set MPAT

    addi 1, r20, r20
    bne clear_mpu_1
clear_mpu_exit:
    jmp [lp]

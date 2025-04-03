.section "VECTAB", text
.public _vector_table
.extern _start
_vector_table:
	.align	512
	jr32	_start ; RESET

	.align	16
	syncp
	jr32	_handle_exception ; SYSERR ; Guest

	.align	16
	jr32	_handle_exception ; (R.F.U)

	.align	16
	jr32	_handle_exception ; FETRAP ; Guest?

	.align	16
	jr32	_handle_exception ; TRAP0 ; Guest?

	.align	16
	jr32	_handle_exception ; TRAP1 ; Guest?

	.align	16
	jr32	_handle_exception ; RIE

	.align	16
	syncp
	jr32	_handle_exception ; FPP/FPI

	.align	16
	jr32	_handle_exception ; UCPOP

	.align	16
	jr32	_handle_exception ; MIP/MDP ; Guest

	.align	16
	jr32	_handle_exception ; RIE

	.align	16
	jr32	_handle_exception ; PIE

	.align	16
	jr32	_handle_exception ; MAE

	.align	16
	jr32	_handle_exception ; UCPOP

	.align	16
	syncp
	jr32	_handle_exception ; FENMI

	.align	16
	syncp
	jr32	_handle_exception ; FEINT

	; only used with direct vector method
	.align	16
	syncp
	jr32	_Interrupt_EI ; INTn(priority0)

	.align	16
	syncp
	jr32	_Interrupt_EI ; INTn(priority1)

	.align	16
	syncp
	jr32	_Interrupt_EI ; INTn(priority2)

	.align	16
	syncp
	jr32	_Interrupt_EI ; INTn(priority3)

	.align	16
	syncp
	jr32	_Interrupt_EI ; INTn(priority4)

	.align	16
	syncp
	jr32	_Interrupt_EI ; INTn(priority5)

	.align	16
	syncp
	jr32	_Interrupt_EI ; INTn(priority6)

	.align	16
	syncp
	jr32	_Interrupt_EI ; INTn(priority7)

	.align	16
	syncp
	jr32	_Interrupt_EI ; INTn(priority8)

	.align	16
	syncp
	jr32	_Interrupt_EI ; INTn(priority9)

	.align	16
	syncp
	jr32	_Interrupt_EI ; INTn(priority10)

	.align	16
	syncp
	jr32	_Interrupt_EI ; INTn(priority11)

	.align	16
	syncp
	jr32	_Interrupt_EI ; INTn(priority12)

	.align	16
	syncp
	jr32	_Interrupt_EI ; INTn(priority13)

	.align	16
	syncp
	jr32	_Interrupt_EI ; INTn(priority14)

	.align	16
	syncp
	jr32	_Interrupt_EI ; INTn(priority15)

; only used with table reference method
.section "EINTTBL", const ; External interrupt table
.public _interrupt_table
_interrupt_table:
	.align	512
	.dw	#_Interrupt_EI ; INT0
	.dw	#_Interrupt_EI ; INT1
	.dw	#_Interrupt_EI ; INT2
	.rept	2048 - 3
	.dw	#_Interrupt_EI ; INTn
	.endm

.section ".text", text
	.align	2

select_xxret:
	stsr 5, r31, 0
	andi 0x80, r31, r31 ; PSW.NP
	cmp r0, r31
	be ei_ret
	stsr 28, r31, 0
    feret
ei_ret:
	stsr 28, r31, 0
	eiret

SAVE_REGS .macro

    sub (31 * 4), sp

    st.dw r0,    0[sp]
    st.dw r2,    8[sp]
    st.dw r4,   16[sp]
    st.dw r6,   24[sp]
    st.dw r8,   32[sp]
    st.dw r10,  40[sp]
    st.dw r12,  48[sp]
    st.dw r14,  56[sp]
    st.dw r16,  64[sp]
    st.dw r18,  72[sp]
    st.dw r20,  80[sp]
    st.dw r22,  88[sp]
    st.dw r24,  96[sp]
    st.dw r26, 104[sp]
    st.dw r28, 112[sp]
    st.dw r30, 120[sp]
.endm

RESTORE_REGS .macro
    ld.dw   0[sp], r0
    ld.w    8[sp], r2 ; dont restore r3 which is the sp
    ld.dw  16[sp], r4
    ld.dw  24[sp], r6
    ld.dw  32[sp], r8
    ld.dw  40[sp], r10
    ld.dw  48[sp], r12
    ld.dw  56[sp], r14
    ld.dw  64[sp], r16
    ld.dw  72[sp], r18
    ld.dw  80[sp], r20
    ld.dw  88[sp], r22
    ld.dw  96[sp], r24
    ld.dw 104[sp], r26
    ld.dw 112[sp], r28
    ld.dw 120[sp], r30

    add (31 * 4), sp
.endm

.extern _irq_handle

_handle_exception:
	br _handle_exception

_Interrupt_EI:

    SAVE_REGS
	
	; copy int_id to r6
	stsr 13, r6, 0 ; get int cause from EEIC
	mov 0x7FF, r20
	and r20, r6 ; mask EIINT source

    jarl _irq_handle, lp

    RESTORE_REGS

    eiret

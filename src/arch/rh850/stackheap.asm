TOTAL_STACK_SIZE   .set 0x1000
TOTAL_HEAP_SIZE   .set 0x1000

.section ".stackheap", bss

.align	4
.public _stack_base
.ds (TOTAL_STACK_SIZE)
_stack_base:

.align	4
.ds (TOTAL_HEAP_SIZE)
.public __heap_base
__heap_base:

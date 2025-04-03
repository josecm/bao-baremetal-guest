/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

 #ifndef __ARCH_SPINLOCK__
 #define __ARCH_SPINLOCK__
 
 #include <core.h>
 
 typedef uint32_t spinlock_t;
 
 #define SPINLOCK_INITVAL 0

 static inline void spinlock_init(spinlock_t* lock)
 {
     *lock = 0;
 }
 
 #pragma inline_asm spin_lock
 static void spin_lock(spinlock_t* lock)
 {
    .LOCAL Lock
    .LOCAL Lock_wait
    .LOCAL Lock_success

    Lock:
        ldl.w[r6], r19
        cmp r0, r19
        bnz Lock_wait
        mov 1, r19
        stc.W r19, [r6]
        cmp r0,r19
        bnz Lock_success
    Lock_wait:
        SNOOZE
        br Lock
    Lock_success:
 }
 
 #pragma inline_asm spin_unlock
 static void spin_unlock(spinlock_t* lock)
 {
    st.w r0, 0 [r6]
 }
 
 #endif /* __ARCH_SPINLOCK__ */
 
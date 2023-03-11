#include <core.h>
#include <cpu.h>

#include <stdio.h>
#include <string.h>

#include "optee_msg.h"
#include "tee_api_defines.h"

#define U(X)    (X ## U)

#include <sbi.h>

static inline struct sbiret sbi_call(
    unsigned long extid,
    unsigned long fid,
    unsigned long arg0, 
    unsigned long arg1, 
    unsigned long arg2,
    unsigned long arg3)
{
    unsigned long register _a7 asm("a7") = extid;
    unsigned long register _a6 asm("a6") = fid;
    unsigned long register _a0 asm("a0") = arg0;
    unsigned long register _a1 asm("a1") = arg1;
    unsigned long register _a2 asm("a2") = arg2;
    unsigned long register _a3 asm("a3") = arg3;

    __asm__ volatile("ecall"
                 : "+r"(_a0), "+r"(_a1)
                 : "r"(_a2), "r"(_a3), "r"(_a6), "r"(_a7)
                 : "memory");
    
    return (struct sbiret){ .error = _a0, .value = _a1 };
}

#define SBI_EXTID_BAO (0x08000ba0)
#define SBI_BAO_TEE_HC (4)

#define TEEHC_FUNCID_CLIENT_FLAG     (0x80000000)
#define TEEHC_FUNCID_MASK           (~TEEHC_FUNCID_CLIENT_FLAG)

#define OPTEE_SMC_32			U(0)
#define OPTEE_SMC_64			U(0x40000000)
#define OPTEE_SMC_FAST_CALL		U(0x80000000)
#define OPTEE_SMC_STD_CALL		U(0)

#define OPTEE_SMC_OWNER_MASK		U(0x3F)
#define OPTEE_SMC_OWNER_SHIFT		U(24)

#define OPTEE_SMC_FUNC_MASK		U(0xFFFF)

#define OPTEE_SMC_IS_FAST_CALL(smc_val)	((smc_val) & OPTEE_SMC_FAST_CALL)
#define OPTEE_SMC_IS_64(smc_val)	((smc_val) & OPTEE_SMC_64)
#define OPTEE_SMC_FUNC_NUM(smc_val)	((smc_val) & OPTEE_SMC_FUNC_MASK)
#define OPTEE_SMC_OWNER_NUM(smc_val) \
	(((smc_val) >> OPTEE_SMC_OWNER_SHIFT) & OPTEE_SMC_OWNER_MASK)

#define OPTEE_SMC_CALL_VAL(type, calling_convention, owner, func_num) \
			((type) | (calling_convention) | \
			(((owner) & OPTEE_SMC_OWNER_MASK) << \
				OPTEE_SMC_OWNER_SHIFT) |\
			((func_num) & OPTEE_SMC_FUNC_MASK))

#define OPTEE_SMC_STD_CALL_VAL(func_num) \
	OPTEE_SMC_CALL_VAL(OPTEE_SMC_32, OPTEE_SMC_STD_CALL, \
			   OPTEE_SMC_OWNER_TRUSTED_OS, (func_num))
#define OPTEE_SMC_FAST_CALL_VAL(func_num) \
	OPTEE_SMC_CALL_VAL(OPTEE_SMC_32, OPTEE_SMC_FAST_CALL, \
			   OPTEE_SMC_OWNER_TRUSTED_OS, (func_num))

#define OPTEE_SMC_OWNER_ARCH		U(0)
#define OPTEE_SMC_OWNER_CPU		U(1)
#define OPTEE_SMC_OWNER_SIP		U(2)
#define OPTEE_SMC_OWNER_OEM		U(3)
#define OPTEE_SMC_OWNER_STANDARD	U(4)
#define OPTEE_SMC_OWNER_TRUSTED_APP	U(48)
#define OPTEE_SMC_OWNER_TRUSTED_OS	U(50)

#define OPTEE_SMC_OWNER_TRUSTED_OS_OPTEED U(62)
#define OPTEE_SMC_OWNER_TRUSTED_OS_API	U(63)

#define OPTEE_SMC_FUNCID_CALL_WITH_ARG OPTEE_MSG_FUNCID_CALL_WITH_ARG
#define OPTEE_SMC_CALL_WITH_ARG \
	OPTEE_SMC_STD_CALL_VAL(OPTEE_SMC_FUNCID_CALL_WITH_ARG)
#define OPTEE_SMC_CALL_WITH_RPC_ARG \
	OPTEE_SMC_STD_CALL_VAL(OPTEE_SMC_FUNCID_CALL_WITH_RPC_ARG)
#define OPTEE_SMC_CALL_WITH_REGD_ARG \
	OPTEE_SMC_STD_CALL_VAL(OPTEE_SMC_FUNCID_CALL_WITH_REGD_ARG)

#define OPTEE_SHARED_MEM_BASE (0x88f00000)

struct optee_msg_arg *optee_args = (void*)OPTEE_SHARED_MEM_BASE;

typedef struct {
	uint32_t timeLow;
	uint16_t timeMid;
	uint16_t timeHiAndVersion;
	uint8_t clockSeqAndNode[8];
} TEE_UUID;

#define TA_HELLO_WORLD_UUID \
	{ 0x8aaaf200, 0x2450, 0x11e4, \
		{ 0xab, 0xe2, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b} } 

void tee_uuid_to_octets(uint8_t *d, const TEE_UUID *s)
{
	d[0] = s->timeLow >> 24;
	d[1] = s->timeLow >> 16;
	d[2] = s->timeLow >> 8;
	d[3] = s->timeLow;
	d[4] = s->timeMid >> 8;
	d[5] = s->timeMid;
	d[6] = s->timeHiAndVersion >> 8;
	d[7] = s->timeHiAndVersion;
	memcpy(d + 8, s->clockSeqAndNode, sizeof(s->clockSeqAndNode));
}

void main(void){

    printf("Bao baremetal OPTEE test guest \n");

    TEE_UUID ta_uuid = (TEE_UUID) TA_HELLO_WORLD_UUID;

    // Opening session for hello word example TA

    optee_args->cmd = OPTEE_MSG_CMD_OPEN_SESSION;
    optee_args->num_params = 2;
    optee_args->params[0].attr = OPTEE_MSG_ATTR_META | OPTEE_MSG_ATTR_TYPE_VALUE_INPUT;
    tee_uuid_to_octets((void*)&optee_args->params[0].u.value, &ta_uuid);
    optee_args->params[1].attr = OPTEE_MSG_ATTR_META | OPTEE_MSG_ATTR_TYPE_VALUE_INPUT;
    optee_args->params[1].u.value.c = TEE_LOGIN_PUBLIC;
    struct sbiret ret = sbi_call(SBI_EXTID_BAO, SBI_BAO_TEE_HC, OPTEE_SMC_CALL_WITH_ARG | TEEHC_FUNCID_CLIENT_FLAG, 0, (uintptr_t)optee_args, 0);
    if (ret.error < 0) {
        printf("open session failed\n");
        return;
    }


    // Invoking TA test comannds increment and decrement

    optee_args->cmd = OPTEE_MSG_CMD_INVOKE_COMMAND;
    optee_args->num_params = 0;
    optee_args->func = 0; // inc
    ret = sbi_call(SBI_EXTID_BAO, SBI_BAO_TEE_HC, OPTEE_SMC_CALL_WITH_ARG | TEEHC_FUNCID_CLIENT_FLAG, 0, (uintptr_t)optee_args, 0);
    if (ret.error < 0) {
        printf("invoke inc failed\n");
    }
    optee_args->func = 1; // dec
    ret = sbi_call(SBI_EXTID_BAO, SBI_BAO_TEE_HC, OPTEE_SMC_CALL_WITH_ARG | TEEHC_FUNCID_CLIENT_FLAG, 0, (uintptr_t)optee_args, 0);
    if (ret.error < 0) {
        printf("invoke dec failed\n");
    }

    // Closing session

    optee_args->cmd = OPTEE_MSG_CMD_CLOSE_SESSION;
    optee_args->num_params = 0;
    ret = sbi_call(SBI_EXTID_BAO, SBI_BAO_TEE_HC, OPTEE_SMC_CALL_WITH_ARG | TEEHC_FUNCID_CLIENT_FLAG, 0, (uintptr_t)optee_args, 0);
    if (ret.error < 0) {
        printf("close session failed\n");
        return;
    }

    printf("Baremetal OPTEE test guest finished!\n");
}

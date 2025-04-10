
gen_ld_file:=$(BUILD_DIR)/linker.ld

inner_c_srcs:=$(filter $(ROOT_DIR)/%.c, $(C_SRC))
inner_asm_srcs:=$(filter $(ROOT_DIR)/%.asm, $(ASM_SRC))
outer_c_srcs:=$(filter-out $(inner_c_srcs), $(C_SRC))
outer_asm_srcs:=$(filter-out $(inner_asm_srcs), $(ASM_SRC))

external_build_dir:=$(BUILD_DIR)/external

objs:=$(inner_c_srcs:$(ROOT_DIR)/%.c=$(BUILD_DIR)/%.o) \
	$(inner_asm_srcs:$(ROOT_DIR)/%.asm=$(BUILD_DIR)/%.o) \
	$(outer_c_srcs:%.c=$(external_build_dir)/%.o) \
	$(outer_asm_srcs:%.asm=$(external_build_dir)/%.o)

deps:=$(objs:%=%.d) $(gen_ld_file).d
dirs:=$(sort $(dir $(objs) $(deps)))

cpp=ccrh
cc=ccrh
ld=rlink
as=asrh
HOST_CC:=gcc


OPT_LEVEL = nothing
DEBUG_LEVEL =

debug_flags:= -g$(DEBUG_LEVEL) $(arch_debug_flags) $(platform_debug_flags)
GENERIC_FLAGS = -Xcommon=rh850 $(ARCH_GENERIC_FLAGS) -O$(OPT_LEVEL) $(debug_flags)
CPPFLAGS += $(ARCH_CPPFLAGS) $(addprefix -I, $(INC_DIRS))
ifneq ($(STD_ADDR_SPACE),)
CPPFLAGS+=-DSTD_ADDR_SPACE
endif
ifneq ($(MPU),)
CPPFLAGS+=-DMPU
endif
ifneq ($(MEM_BASE),)
CPPFLAGS+=-DMEM_BASE=$(MEM_BASE)
endif
ifneq ($(MEM_SIZE),)
CPPFLAGS+=-DMEM_SIZE=$(MEM_SIZE)
endif
ifneq ($(SINGLE_CORE),)
CPPFLAGS+=-DSINGLE_CORE=y
endif
ifneq ($(NO_FIRMWARE),)
CPPFLAGS+=-DNO_FIRMWARE=y
endif
ASFLAGS += $(GENERIC_FLAGS) $(CPPFLAGS) $(ARCH_ASFLAGS) 
CFLAGS += $(GENERIC_FLAGS) $(CPPFLAGS) $(ARCH_CFLAGS) -lang=c99 
LDFLAGS += $(GENERIC_FLAGS) $(ARCH_LDFLAGS) -nostartfiles

target:=$(BUILD_DIR)/$(NAME)
all: $(target).bin

ifneq ($(MAKECMDGOALS), clean)
-include $(deps)
endif

%.bin: %.elf
	@echo "generating binary	$(patsubst $(cur_dir)/%, %, $@)"
	@$(ld) -subcommand="$(gen_ld_file)" -form=binary -output="$@"

$(target).elf: $(objs) $(gen_ld_file)
	@echo "Linking			$(patsubst $(cur_dir)/%, %, $@)"
	@$(ld) -subcommand="$(gen_ld_file)" -form=absolute -output="$@"
	@readelf -aW $@ > $@.txt

$(BUILD_DIR):
	mkdir -p $@

$(objs): | $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(ROOT_DIR)/%.c
	@echo $@
	@$(HOST_CC) $(CPPFLAGS) -M -MF $(basename $@).d $<
	@$(cc) $(CFLAGS) -c $< -o$@

$(external_build_dir)/%.o: %.c
	@echo $@
	@$(HOST_CC) $(CPPFLAGS) -M -MF $(basename $@).d $<
	@$(cc) $(CFLAGS) -c $< -o$@

$(BUILD_DIR)/%.o: $(ROOT_DIR)/%.asm
	@echo $@
	@$(HOST_CC) $(CPPFLAGS) -x assembler-with-cpp -M -MF $(basename $@).d $<
	@$(cc) $(ASFLAGS) -c $< -o$@

$(external_build_dir)/%.o: %.asm
	@echo $@
	@$(HOST_CC) $(CPPFLAGS) -x assembler-with-cpp -M -MF $(basename $@).d $<
	@$(cc) $(ASFLAGS) -c $< -o$@

$(gen_ld_file):
	@echo $(foreach obj,$(objs),-input="$(obj)\n") > $@
	@echo " -list" >> $@
	@echo " -nologo" >> $@
	@echo ' -library="$(shell dirname $(shell dirname $(shell which $(cc))))/lib/v850e3v5/rhs8n.lib"' >> $@
	@echo " -start=VECTAB,EINTTBL,.text,.const,.data/10000,.data.R,.bss,.stackheap/fe000000" >> $@
	@echo " -rom=.data*=.data.*R" >> $@

.SECONDEXPANSION:

$(objs) $(deps): | $$(@D)/

$(dirs):
	mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: all clean

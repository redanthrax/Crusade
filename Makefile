#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/gba_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# DATA is a list of directories containing binary data
# MUSIC is a directory of Maxmod tracker modules (mmutil soundbank input)
#
# All directories are specified relative to the project directory where
# the makefile is found
#---------------------------------------------------------------------------------
TARGET		:= crusade
BUILD		:= build
SOURCES		:= src src/acts data/maps data/text graphics/gen
INCLUDES	:= include graphics/gen
DATA		:=
MUSIC		:= audio

GAME_TITLE	:= CRUSADE
GAME_CODE	:= CRUS
MAKER_CODE	:= RA

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-mthumb -mthumb-interwork

CFLAGS	:=	-g -Wall -O2 -std=c99\
		-mcpu=arm7tdmi -mtune=arm7tdmi\
		$(ARCH)

CFLAGS	+=	$(INCLUDE)

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-g $(ARCH) -Wl,-Map,$(notdir $*.map)

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:= -ltonc -lmm -lgba

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBTONC	:=	$(DEVKITPRO)/libtonc
LIBDIRS	:=	$(LIBTONC) $(LIBGBA)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

ifneq ($(strip $(MUSIC)),)
	ifneq ($(strip $(wildcard $(MUSIC)/*.*)),)
		export AUDIOFILES	:=	$(foreach dir,$(notdir $(wildcard $(MUSIC)/*.*)),$(CURDIR)/$(MUSIC)/$(dir))
		BINFILES += soundbank.bin
	endif
endif

export LD	:=	$(CC)

export OFILES_BIN := $(addsuffix .o,$(BINFILES))
export OFILES_SOURCES := $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES := $(OFILES_BIN) $(OFILES_SOURCES)
export HFILES := $(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-iquote $(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean run playtest

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).gba

#---------------------------------------------------------------------------------
run: $(TARGET).gba
	mgba $(TARGET).gba

playtest: $(BUILD)
	scripts/playtest.sh

#---------------------------------------------------------------------------------
else

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

$(OUTPUT).gba	:	$(OUTPUT).elf
$(OUTPUT).elf	:	$(OFILES)
$(OFILES_SOURCES) : $(HFILES)

#---------------------------------------------------------------------------------
# rule to build soundbank from music files (audio/*.mod, *.s3m, ...)
#---------------------------------------------------------------------------------
soundbank.bin soundbank.h : $(AUDIOFILES)
	@echo built ... soundbank
	@mmutil $^ -osoundbank.bin -hsoundbank.h

#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPSDIR)/*.d
#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------

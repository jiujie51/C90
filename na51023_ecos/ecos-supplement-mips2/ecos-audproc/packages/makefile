# eCos supplement makefile

ifeq "$(shell uname)" "Linux"
export HOST := LINUX
else
export HOST := CYGWIN
endif
ifeq ($(HOST),CYGWIN)
export ECOS_PREFIX := $(shell realpath ../ecos)
export COMMAND_PREFIX := mips-sde-elf-
else
export ECOS_PREFIX := $(shell realpath ../../../../ecos-kit/ThirdParty/eCos)
export COMMAND_PREFIX := /opt/im/mipsel-sde-elf-4.4.3/bin/mipsel-sde-elf-
endif
export ECOS_THIRDPARTY := $(shell realpath $(ECOS_PREFIX)/..)
export ECOS_PUBLISH := $(ECOS_THIRDPARTY)/$(notdir $(shell realpath ..))
export PREFIX := $(shell realpath ..)/install
export REPOSITORY := $(shell realpath ../packages)
export CC := $(COMMAND_PREFIX)gcc
export OBJCOPY := $(COMMAND_PREFIX)objcopy
export AR := $(COMMAND_PREFIX)ar
ifeq ($(HOST),CYGWIN)
export SHA1_TOOL := $(ECOS_PREFIX)/../../../uitron/tools/nvt-ld-op.exe
else
export SHA1_TOOL := $(ECOS_PREFIX)/../../../uitron/tools/nvt-ld-op
endif

.PHONY: default build clean tests headers

build: $(PREFIX)/../packages/pkgconf/ecos_supp.mak $(ECOS_PREFIX)/include/pkgconf/ecos.mak headers
	$(MAKE) -r -C nvt/audproc/v2_0_60 $@
	@echo $@ finished

clean: $(PREFIX)/../packages/pkgconf/ecos_supp.mak
	$(MAKE) -r -C nvt/audproc/v2_0_60 $@
	rm $(PREFIX)/../packages/pkgconf/ecos_supp.mak
	rm -r $(PREFIX)
	@echo $@ finished

tests: build
	$(MAKE) -r -C nvt/audproc/v2_0_60 $@
	@echo $@ finished

headers: $(PREFIX)/../packages/pkgconf/ecos_supp.mak
	$(MAKE) -r -C nvt/audproc/v2_0_60 $@
	@echo $@ finished

install: $(PREFIX)/../packages/pkgconf/ecos_supp.mak
	$(MAKE) -r -C nvt/audproc/v2_0_60 $@
	@echo $@ finished

$(PREFIX):
	mkdir -p $@

$(ECOS_PREFIX)/include/pkgconf/ecos.mak:
	$(if $(wildcard $(ECOS_PREFIX)),,$(error "$(ECOS_PREFIX) link is missing, stop."))

$(PREFIX)/../packages/pkgconf/ecos_supp.mak: $(PREFIX) makefile $(ECOS_PREFIX)/include/pkgconf/ecos.mak
	@cp -f $(ECOS_PREFIX)/include/pkgconf/ecos.mak $@
	@echo 'PREFIX = $(PREFIX)' >> $@
	@echo 'REPOSITORY = $(REPOSITORY)' >> $@
	@echo 'ECOS_PREFIX = $(ECOS_PREFIX)' >> $@
	@echo 'CC = $(CC)' >> $@
	@echo 'OBJCOPY = $(OBJCOPY)' >> $@
	@echo 'AR = $(AR)' >> $@

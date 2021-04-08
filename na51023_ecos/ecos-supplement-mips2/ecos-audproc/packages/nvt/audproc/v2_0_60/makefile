# eCos makefile

# This is a generated file - do not edit

include ../../../../packages/pkgconf/ecos_supp.mak

PACKAGE := nvt/audproc/v2_0_60
OBJECT_PREFIX := audproc
CFLAGS := $(ECOS_GLOBAL_CFLAGS)
LDFLAGS := $(ECOS_GLOBAL_LDFLAGS)
VPATH := $(REPOSITORY)/$(PACKAGE)
INCLUDE_PATH := $(INCLUDE_PATH) -I$(PREFIX)/include $(foreach dir,$(VPATH),-I$(dir) -I$(dir)/include -I$(dir)/src -I$(dir)/tests) -I. -I$(ECOS_PREFIX)/include -I$(ECOS_PREFIX)/../ecos-nvtipc/include
MLT := $(wildcard $(REPOSITORY)/$(PACKAGE)/include/pkgconf/mlt*.ldi $(REPOSITORY)/$(PACKAGE)/include/pkgconf/mlt*.h)
TESTS :=

build: headers libaudproc.a.stamp

install: build
	mkdir -p $(ECOS_PUBLISH)
	cp -rf $(PREFIX)/* $(ECOS_PUBLISH)

LIBRARY := libaudproc.a
COMPILE := src/audproc.c src/audprocVerInfo.c
OBJECTS := $(COMPILE:.cxx=.o.d)
OBJECTS := $(OBJECTS:.cpp=.o.d)
OBJECTS := $(OBJECTS:.c=.o.d)
OBJECTS := $(OBJECTS:.S=.o.d)

$(LIBRARY).stamp: $(OBJECTS)
	@mkdir -p $(PREFIX)/lib
	$(AR) rcsD $(PREFIX)/lib/$(@:.stamp=) $(foreach obj,$?,$(if $(obj:%.o=),$(dir $(obj))$(OBJECT_PREFIX)_$(notdir $(obj:.o.d=.o)),$(obj)))
	$(SHA1_TOOL) --arc-sha1 $(PREFIX)/lib/$(@:.stamp=)
	@cat $(foreach obj,$^,$(obj:.o=.o.d)) > $(@:.stamp=.deps)
	@touch $@

headers: mlt_headers $(PREFIX)/include/cyg/audproc/audproc.h $(PREFIX)/include/cyg/audproc/api_wakeup.h

$(PREFIX)/include/cyg/audproc/audproc.h: $(REPOSITORY)/$(PACKAGE)/include/audproc.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@

$(PREFIX)/include/cyg/audproc/api_wakeup.h: $(REPOSITORY)/$(PACKAGE)/include/api_wakeup.h
	@mkdir -p $(dir $@)
	@cp $< $@
	@chmod u+w $@
	
include $(REPOSITORY)/pkgconf/rules_supp.mak


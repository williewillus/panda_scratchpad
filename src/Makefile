CC =  gcc
G++ = g++
OPT = -std=gnu++11
GOTPSSO = -shared -fPIC

KERNEL_VERSION := $(shell uname -r)
KERNEL_MAJ := $(shell echo $(KERNEL_VERSION) | cut -f1 -d.)
KERNEL_MIN := $(shell echo $(KERNEL_VERSION) | cut -f2 -d.)

XATTR_DEF_FLAG :=
ifeq ($(shell [ $(KERNEL_MAJ) -gt 4 -o \
	\( $(KERNEL_MAJ) -eq 4 -a $(KERNEL_MIN) -ge 15 \) ] && echo true), true)
XATTR_DEF_FLAG := -DNEW_XATTR_INC
endif

BUILDDIR = ../build
ROOTDIR = ../

TARGET = $(ROOTDIR)harness

.PHONY: all

all: $(BUILDDIR) $(TARGET)

$(BUILDDIR): 
	@mkdir -p $(BUILDDIR)

$(TARGET): \
	$(ROOTDIR)/src/main.cpp \
	$(BUILDDIR)/communication/ClientSocket.o \
	$(BUILDDIR)/tests/BaseTestCase.o \
	$(BUILDDIR)/results/DataTestResult.o \
	$(BUILDDIR)/wrapper/DiskMod.o \
	$(BUILDDIR)/wrapper/wrapper.o \
	$(BUILDDIR)/wrapper/actions.o \
	$(BUILDDIR)/utils/FSCommands.o
	$(G++) $(OPT) $^ -ldl -o $@

$(BUILDDIR)/communication/%.o: communication/%.cpp
	@mkdir -p $(@D)
	$(G++) -c $(OPT) -o $@ $<

$(BUILDDIR)/wrapper/%.o: wrapper/%.cpp
	@mkdir -p $(@D)
	$(G++) -c $(OPT) -o $@ $<

$(BUILDDIR)/results/%.o: results/%.cpp
	@mkdir -p $(@D)
	$(G++) -c $(OPT) -o $@ $<

$(BUILDDIR)/tests/%.o: tests/%.cpp
	@mkdir -p $(@D)
	$(G++) -c $(OPT) -o $@ $<

$(BUILDDIR)/utils/%.o: utils/%.cpp
	@mkdir -p $(@D)
	$(G++) -c $(OPT) -o $@ $<

$(BUILD_DIR)/tests/%.so: \
		tests/%.cpp \
		$(BUILD_DIR)/tests/BaseTestCase.o \
		$(BUILD_DIR)/wrapper/actions.o \
		$(BUILD_DIR)/wrapper/workload.o \
		$(BUILD_DIR)/wrapper/wrapper.o \
		$(BUILD_DIR)/wrapper/DiskMod.o \
		$(BUILD_DIR)/results/DataTestResult.o
	mkdir -p $(@D)
	$(G++) $(OPT) $(GOTPSSO) $(XATTR_DEF_FLAG) -Wl,-soname,$(notdir $@) \
-o $@ $^
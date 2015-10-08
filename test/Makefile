THIS_SHARED_LIB=$(shell find $(CMSSW_BASE)/tmp/ |grep libL1TriggerL1TCaloLayer1.so | grep -v prod | grep -v dep)

CFLAGS = -g -I ../src -I ../plugins
LIBDIR = .
BINDIR = .
SHLIB =
LDFLAGS = -L $(LIBDIR) $(THIS_SHARED_LIB)
CXXFLAGS = $(CFLAGS)

include $(HOME)/mkdep_makefile/config

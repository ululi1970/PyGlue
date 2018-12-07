# An executable will be created for each name in ebase.
ebase := main

# Where do the Chombo libraries live?
CHOMBO_HOME = $(HOME)/Chombo-3.1_GCC/lib

# Do you want to trap floating point errors?
TRAPFPE = FALSE


# BEGIN: Don't mess with this stuff.
MACHINE = $(shell uname)
UNAMEM = $(shell uname -m)

include $(CHOMBO_HOME)/mk/Make.defs
include $(CHOMBO_HOME)/mk/Make.defs.config
LibNames :=  AMRElliptic AMRTools AMRTimeDependent BoxTools

base_dir =  .
# src_dirs = $(shell find ../src -maxdepth 256 -type d | tr '\n' ' ')
# src_dirs  = $(shell find ../src/Grade1_Basics            -maxdepth 256 -type d | tr '\n' ' ')
# src_dirs += $(shell find ../src/Grade2_AnisotropicChombo -maxdepth 256 -type d | tr '\n' ' ')
# src_dirs += $(shell find ../src/Grade3_Calculus          -maxdepth 256 -type d | tr '\n' ' ')
# src_dirs += $(shell find ../src/Grade4_MappedChombo      -maxdepth 256 -type d | tr '\n' ' ')
# src_dirs += $(shell find ../src/Grade5_SOMAR             -maxdepth 256 -type d | tr '\n' ' ')
inc_dirs  = $(CHOMBO_HOME)/include $(src_dirs)
cxxoptflags += -std=c++11
include $(CHOMBO_HOME)/mk/Make.example
# END: Don't mess with this stuff.

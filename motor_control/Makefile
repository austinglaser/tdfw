##############################################################################
# Build global options
# NOTE: Can be overridden externally.
#

# Determine root directory
ROOT_DIR:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))

# Compiler options here.
ifeq ($(USE_OPT),)
  USE_OPT = -O0 -ggdb -fomit-frame-pointer -falign-functions=16
endif

# C specific options here (added to USE_OPT).
ifeq ($(USE_COPT),)
  USE_COPT = -DCHPRINTF_USE_FLOAT=TRUE -DCORTEX_USE_FPU=TRUE
endif

# C++ specific options here (added to USE_OPT).

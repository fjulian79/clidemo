DEVICE      = stm32f103rb
BINARY      = clidemo
DEPENDS     = libopencm3 libcli
SRCEXT      = cpp
DEFS        =
INCLUDE     =
OPT         = -Og
WARN_FLAGS  = -Wall

###############################################################################
# Call generic application makefile
###############################################################################
include $(MYBUILDROOTPATH)/makefiles/application.mk

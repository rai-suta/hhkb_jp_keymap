# Build Options
#   change to "no" to disable the options, or define them in the Makefile in 
#   the appropriate keymap folder that will get included automatically
#
OPT_DEFS += -DHHKB_JP -DJIS_KEYCODE
#OPT_DEFS += -DPRINT_MATRIX_ENABLE		# Enable "print_matrix()"

SRC += matrix_task.c

BOOTMAGIC_ENABLE  = no    # Virtual DIP switch configuration(+1000)
MOUSEKEY_ENABLE   = no    # Mouse keys(+4700)
EXTRAKEY_ENABLE   = yes   # Audio control and System control(+450)
CONSOLE_ENABLE    = yes   # Console for debug(+400)
COMMAND_ENABLE    = yes   # Commands for debug and configuration
CUSTOM_MATRIX     = yes   # Custom matrix file for the HHKB

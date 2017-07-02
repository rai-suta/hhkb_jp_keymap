# Build Options
#   change to "no" to disable the options, or define them in the Makefile in 
#   the appropriate keymap folder that will get included automatically
#
OPT_DEFS += -DHHKB_JP -DJIS_KEYCODE
#OPT_DEFS += -DPRINT_MATRIX_ENABLE		# Enable "print_matrix()"

SRC += matrix_task.c

BOOTMAGIC_ENABLE = yes       	# Virtual DIP switch configuration(+1000)
CONSOLE_ENABLE = yes         	# Console for debug(+400)

ifndef QUANTUM_DIR
	include ../../../../Makefile
endif

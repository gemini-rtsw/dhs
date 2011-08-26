# Makefile

NAME = dhsConsole

SOURCES = dhsConsole.tcl dhgUtils.tcl tgUtils.tcl utils.tcl \
 arrayDisplay.tcl choiceDialog.tcl configGen.tcl datasetDisplay.tcl dhgMain.tcl \
 dhgStatus.tcl dhsCommands.tcl dtsCommands.tcl form.tcl help.tcl labeledWidget.tcl \
 led.tcl logMonitor.tcl progressBar.tcl statusBox.tcl stgMain.tcl stoCommands.tcl table.tcl

INSTALL_TARGET = install_cmd

include ../conf/tcl.mk

install_cmd:
	cp dhsConsole ${RELEASE_DIR}/scripts

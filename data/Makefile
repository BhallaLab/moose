PWD:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
MOOSECORE_DIR=$(PWD)/moose-core
MOOSEGUI_DIR=$(PWD)/moose-gui

MOOSECORE_BUILD_DIR=_moose_core_build_dir_

MOOSEBIN:=$(MOOSECORE_BUILD_DIR)/moose.bin
MOOSESDIST:=$(MOOSECORE_BUILD_DIR)/moose-3.0.2.tar.gz

## Prefix 
PREFIX:=/usr

all: build_moose

build_moose: build_moose_core build_moose_gui

build_moose_core $(MOOSEBIN) $(MOOSESDIST): configure_moose_core 
	cd $(MOOSECORE_BUILD_DIR) && make -j`proc`

configure_moose_core:
	echo "Configuring moose-core"
	mkdir -p $(MOOSECORE_BUILD_DIR)
	cd $(MOOSECORE_BUILD_DIR) && cmake -DCMAKE_INSTALL_PREFIX=$(PREFIX) $(MOOSECORE_DIR)


clean:
	git clean -fxd


.PNONY: clean uninstall install

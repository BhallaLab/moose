PWD:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
MOOSECORE_DIR=$(PWD)/moose-core
MOOSEGUI_DIR=$(PWD)/moose-gui

MOOSECORE_BUILD_DIR=_moose_core_build_dir_

## Prefix 
PREFIX:=/usr

all: build_moose

build_moose: build_moose_core build_moose_gui

build_moose_core: configure_moose_core 
	cd $(MOOSECORE_BUILD_DIR) && $(MAKE)

configure_moose_core:
	mkdir -p $(MOOSECORE_BUILD_DIR)
	cd $(MOOSECORE_BUILD_DIR) 
	cmake -DCMAKE_INSTALL_PREFIX=$(PREFIX) $(MOOSECORE_DIR)



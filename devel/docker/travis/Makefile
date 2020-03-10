NAME:=dilawars/moose-travis
VERSION:=$(shell date +"%Y%m%d")

PWD:=$(shell pwd)
PROJECT_ROOT_DIR:=$(PWD)/../../..
DOCKERFILE:=$(PWD)/Dockerfile

all : build

build : Dockerfile
	cd $(PROJECT_ROOT_DIR) && docker build -t $(NAME):$(VERSION) -f $(DOCKERFILE) .
	cd $(PROJECT_ROOT_DIR) && docker build -t $(NAME):latest -f $(DOCKERFILE) .

upload :
	docker push $(NAME):$(VERSION)
	docker push $(NAME):latest

run :
	docker run -it $(NAME):latest bash


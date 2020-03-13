NAME:=dilawars/centos-moose
VERSION:=$(shell date +%Y.%m.%d)

PROJECT_ROOT_DIR:=(PWD)/../..

all : build

build : Dockerfile
	cd ../../ && docker build -t $(NAME):$(VERSION) -f $(PWD)/Dockerfile .
	cd ../../ && docker build -t $(NAME):latest -f $(PWD)/Dockerfile .

upload :
	docker push $(NAME):$(VERSION)
	docker push $(NAME):latest

run :
	docker run $(NAME):$(VERSION) /bin/bash -c "python3 -c 'import moose; moose.test()'"


VERSION := 3.2.dev$(shell date +"%Y%m%d")

all : wheels

DOCKERFILE:="bhallalab/python-wheels"

wheels : ./Dockerfile 
	docker build  -t $(DOCKERFILE):$(VERSION) .
	docker build  -t $(DOCKERFILE):latest .

upload:
	docker push $(DOCKERFILE):$(VERSION)
	docker push $(DOCKERFILE):latest

run: 
	docker run -it $(DOCKERFILE):latest bash

all: web

web: Dockerfile
	docker build . -t faupbox
	gotty -w docker run -it --rm faupbox

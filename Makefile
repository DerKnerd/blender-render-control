GOCMD=go
GOBUILD=$(GOCMD) build

.PHONY: server

server:
	rm -rf out/server
	cd server/ && GOOS=linux GOARCH=amd64 $(GOBUILD) -o ../out/server/blender-render-control

server-install:
	cp out/server/blender-render-control /usr/local/bin
	mkdir -p /usr/local/share/blender-render-control
	cp server/config.yml.example /usr/local/share/blender-render-control/config.yml
	chown -R blender:blender /usr/local/share/blender-render-control
	systemctl enable blender-render-control
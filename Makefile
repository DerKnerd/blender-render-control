GOCMD=go
GOBUILD=$(GOCMD) build

.PHONY: server

server:
	rm -rf out/server
	cd server/ && GOOS=linux GOARCH=amd64 $(GOBUILD) -o ../out/server/blender-render-control

server-install:
	adduser -M -s /bin/false
	cp out/server/blender-render-control /usr/local/bin
	mkdir -p /usr/local/share/blender-render-control
	cp server/config.server.yml.example /usr/local/share/blender-render-control/config.yml
	cp server/render_all_cameras.py /usr/local/share/blender-render-control/render_all_cameras.py
	chown -R blender:blender /usr/local/share/blender-render-control
	cp server/blender-render-control.service /etc/systemd/system/
	systemctl enable blender-render-control
version = 1.2
release = 1
all : memwatch
memwatch : memwatch.cpp 
	g++ -O3 -Wall -Wextra -Wpedantic -std=c++17 -o memwatch memwatch.cpp 
clean :
	rm -rf memwatch memwatch_$(version)-$(release) memwatch_$(version)-$(release).deb
fake_install :
	install -m 755 -D memwatch $(prefix)/usr/sbin/memwatch
	install -m 755 -D hook.example $(prefix)/usr/libexec/memwatch/hook.example
	install -m 644 -D LICENSE $(prefix)/usr/share/doc/memwatch/LICENSE
	install -m 644 -D README.md $(prefix)/usr/share/doc/memwatch/README.md
	install -m 644 -D memwatch.service $(prefix)/usr/lib/systemd/system/memwatch.service
install : prefix = /
install : fake_install
deb : prefix = ./memwatch_$(version)-$(release)/
deb : all fake_install
	sed -i 's/Version: .*/Version: $(version)-$(release)/' DEBIAN/control
	cp -ar DEBIAN memwatch_$(version)-$(release)
	dpkg-deb --build --root-owner-group memwatch_$(version)-$(release)

PACK_DIR = $(abspath $(CURDIR))
ROOT_DIR = $(abspath $(CURDIR)/..)

VERSION := $(shell cat $(ROOT_DIR)/VERSION)

CMAKE = cmake

.PHONY: all clean

all: moonycode moonycode-dev

moonycode:
	@mkdir -p build/install/DEBIAN
	@cd build && \
		$(CMAKE) $(ROOT_DIR) -DBUILD_SHARED:BOOL=ON -DCMAKE_INSTALL_PREFIX=install/usr && \
		$(CMAKE) --build . --target install
	@cat DEBIAN/$@.control | sed "s/@VERSION@/$(VERSION)/g" > build/install/DEBIAN/control
	@echo "Building DEB..."
	@dpkg-deb --build build/install $@_$(VERSION)_amd64.deb
#	@echo Building RPM...
#	@mv $@/usr/lib $@/usr/lib64
#	@rpmbuild -bb RPM/$@.spec --define "_version $(API_VERSION).$(MORPHRUS_DICT_VER).$(MORPHRUS_CODE_VER)" --define "source_dir $(PACKS_DIR)/$@"

moonycode-dev:
	@mkdir -p build/install-dev/DEBIAN
	@cd build && \
		$(CMAKE) $(ROOT_DIR) -DBUILD_STATIC:BOOL=ON -DCMAKE_INSTALL_PREFIX=install-dev/usr && \
		$(CMAKE) --build . --target install
	@cat DEBIAN/$@.control | sed "s/@VERSION@/$(VERSION)/g" > build/install-dev/DEBIAN/control
	@dpkg-deb --build build/install-dev $@_$(VERSION)_amd64.deb
#	@echo Building RPM...
#	@mv $@/usr/lib $@/usr/lib64
#	@rpmbuild -bb RPM/$@.spec --define "_version $(API_VERSION).$(MORPHRUS_DICT_VER).$(MORPHRUS_CODE_VER)" --define "source_dir $(PACKS_DIR)/$@"

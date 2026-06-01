.PHONY: all deb clean test

CXX      ?= g++
CXXFLAGS ?= -std=c++11 -O2
ARCH     := $(shell dpkg --print-architecture)
MAINT    := Eli Fulkerson <elifulkerson@gmail.com>
VERSION  := 0.8
DISTDIR  := dist
BUILDDIR := build

all: ips

ips: ips.cpp
	g++ -std=c++11 -o ips ips.cpp

# Build the ips binary .deb (compiled at build time; no compiler needed on
# the target).
.ONESHELL:
deb: ips.cpp ips.1
	@set -e
	rm -rf $(BUILDDIR)
	mkdir -p $(DISTDIR)
	$(CXX) $(CXXFLAGS) -o $(DISTDIR)/ips ips.cpp
	strip $(DISTDIR)/ips
	root="$(BUILDDIR)/ips"
	install -d "$$root/DEBIAN" "$$root/usr/bin" "$$root/usr/share/man/man1" "$$root/usr/share/doc/ips"
	install -m0755 "$(DISTDIR)/ips" "$$root/usr/bin/ips"
	gzip -9 -n -c ips.1 > "$$root/usr/share/man/man1/ips.1.gz"
	desc="$$(printf ' ips reads arbitrary text from stdin or files and finds the IP addresses in\n it, grep-style. Output can be filtered by range, subnet or category and\n reshaped (basic, line numbers, context, count, sort, unique). Supports\n IPv4/IPv6 and follows grep exit-code conventions.\n .\n It is the extractor counterpart to the xips(1) transformer.')"
	printf 'Package: ips\nVersion: %s-1\nSection: net\nPriority: optional\nArchitecture: %s\nDepends: libc6, libgcc-s1, libstdc++6\nMaintainer: %s\nHomepage: http://www.elifulkerson.com\nDescription: %s\n%s\n' \
		"$(VERSION)" "$(ARCH)" "$(MAINT)" 'grep for IP addresses' "$$desc" > "$$root/DEBIAN/control"
	printf 'Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/\nUpstream-Name: ips\nSource: http://www.elifulkerson.com\n\nFiles: *\nCopyright: %s\nLicense: see upstream\n' \
		"$(MAINT)" > "$$root/usr/share/doc/ips/copyright"
	printf 'ips (%s-1) unstable; urgency=low\n\n  * Packaged ips %s.\n\n -- %s  %s\n' \
		"$(VERSION)" "$(VERSION)" "$(MAINT)" "$$(date -R)" > "$$root/usr/share/doc/ips/changelog.Debian"
	gzip -9 -n "$$root/usr/share/doc/ips/changelog.Debian"
	dpkg-deb --root-owner-group --build "$$root" "$(DISTDIR)/ips_$(VERSION)-1_$(ARCH).deb"
	echo "Built: ips/$(DISTDIR)/ips_$(VERSION)-1_$(ARCH).deb"

clean:
	rm -f ips $(DISTDIR)/ips
	rm -rf $(BUILDDIR)

test:
	python maketests.py > tests.sh && sh ./tests.sh

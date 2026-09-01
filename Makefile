CXX = g++
CC = gcc

CXXFLAGS = -std=c++20 -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -Wshadow -Werror -D_POSIX_C_SOURCE=200809L
CFLAGS   = -std=c11 -Wall -Wextra -D_POSIX_C_SOURCE=200809L

VERSION    := $(shell cat VERSION 2>/dev/null || echo "1.0.0")
GIT_COMMIT ?= $(shell git rev-parse --short HEAD 2>/dev/null || echo "unknown")
BUILD_DATE := $(shell date -u +"%Y-%m-%dT%H:%M:%SZ")


PKGS = wayland-client cairo pango pangocairo libinput libudev xkbcommon gtk+-3.0 appindicator3-0.1

INCLUDES = -Iinclude -I. $(shell pkg-config --cflags $(PKGS))
LIBS     = $(shell pkg-config --libs $(PKGS)) -lm

SRCS = src/color.cpp \
       src/config.cpp \
       src/buffer.cpp \
       src/shm.cpp \
       src/icons.cpp \
       src/draw.cpp \
       src/window.cpp \
       src/tray.cpp \
       src/wl_setup.cpp \
       src/xkb_handler.cpp \
       src/input.cpp \
       src/app.cpp

OBJS = $(SRCS:.cpp=.o) xdg-shell-protocol.o

TARGET = wayshadow

WAYLAND_PROTOCOLS_DIR ?= /usr/share/wayland-protocols

PREFIX  ?= /usr
BINDIR  ?= $(PREFIX)/bin
MANDIR  ?= $(PREFIX)/share/man

all: $(TARGET)

$(TARGET): $(OBJS) src/main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp include/wayshadow/version.hpp xdg-shell-client-protocol.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

xdg-shell-protocol.o: xdg-shell-protocol.c xdg-shell-client-protocol.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

xdg-shell-protocol.c:
	wayland-scanner private-code $(WAYLAND_PROTOCOLS_DIR)/stable/xdg-shell/xdg-shell.xml $@

xdg-shell-client-protocol.h:
	wayland-scanner client-header $(WAYLAND_PROTOCOLS_DIR)/stable/xdg-shell/xdg-shell.xml $@

include/wayshadow/version.hpp: VERSION
	@mkdir -p include/wayshadow
	@echo "/* auto-generated, do not edit */"       > $@
	@echo "#pragma once"                            >> $@
	@echo "#include <string_view>"                  >> $@
	@echo "namespace wayshadow {"                      >> $@
	@echo "inline constexpr std::string_view APP_VERSION = \"$(VERSION)\";"    >> $@
	@echo "inline constexpr std::string_view GIT_COMMIT  = \"$(GIT_COMMIT)\";" >> $@
	@echo "inline constexpr std::string_view BUILD_DATE  = \"$(BUILD_DATE)\";" >> $@
	@echo "}"                                       >> $@

TEST_SRCS = $(SRCS) tests/test_color.cpp tests/test_config.cpp tests/test_buffer.cpp tests/test_main.cpp
TEST_OBJS = $(TEST_SRCS:.cpp=.o) xdg-shell-protocol.o

test_runner: $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

test: test_runner
	./test_runner

clean:
	rm -f src/*.o tests/*.o *.o $(TARGET) test_runner
	rm -f xdg-shell-protocol.c xdg-shell-client-protocol.h
	rm -f include/wayshadow/version.hpp

install: $(TARGET)
	install -D -m 755 $(TARGET)         $(DESTDIR)$(BINDIR)/$(TARGET)
	install -D -m 644 man/wayshadow.1      $(DESTDIR)$(MANDIR)/man1/wayshadow.1
	install -D -m 644 man/wayshadow.conf.5 $(DESTDIR)$(MANDIR)/man5/wayshadow.conf.5

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)
	rm -f $(DESTDIR)$(MANDIR)/man1/wayshadow.1
	rm -f $(DESTDIR)$(MANDIR)/man5/wayshadow.conf.5

.PHONY: all clean install uninstall test

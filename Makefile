CC			= clang-19
CFLAGS		= -Wall -Wpedantic -std=c23
MODE		= Debug
BUILDDIR	= build

LIBCURL	:= $(shell pkg-config --libs --cflags libcurl)

RESD_TARGET		:= $(BUILDDIR)/resd
RESD_SOURCES	:= Sources/main.c Sources/disk.c Sources/discord.c config.c

ifeq ($(MODE), Debug)
CFLAGS	+= -O0 -g -DRD_DEBUG -fsanitize=address
else ifeq ($(MODE), Release)
CFLAGS	+= -O3 -flto
else
$(error invalid MODE)
endif

.PHONY: all clean

all: $(RESD_TARGET)

$(RESD_TARGET): $(RESD_SOURCES) | $(BUILDDIR)
	$(CC) $(RESD_SOURCES) $(CFLAGS) $(LIBCURL) -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -r $(BUILDDIR)

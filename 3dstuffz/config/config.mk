# Project: 3dstuffz
# Makefile configurations

BIN		=	$(TOPDIR)/bin/3dstuffz.elf

DBGFLAGS	=	-O0 -g -D__DEBUG__
VERSION		=	v.0.1
#DBGFLAGS	=	-O3 -g
#General flags
CFLAGS		+=	-Wall -Wextra
LDFLAGS		+=	-lm -ldarnit
PREFIX		=	/usr/local

#Makefile tools
RM		=	rm -Rf
MKDIR		=	mkdir -p


ifeq ($(strip $(OS)), Windows_NT)
	#Windows specifics
	BIN	=	$(TOPDIR)/bin/3dstuffz.exe
	PREFIX	=	/mingw
	LDFLAGS	+=	-lwinmm
	WINDRES	=	windres
	PLATFORM=	win32
else 
ifeq ($(BUILDFOR), WIN32)
	#Windows specifics, for cross compiling to windows
	BIN	=	$(TOPDIR)/bin/3dstuffz.exe
	PREFIX	=	/usr/i586-mingw32msvc
	LDFLAGS	+=	-lwinmm
	CC	=	i586-mingw32msvc-gcc
	STRIP	=	i586-mingw32msvc-strip
	AR	=	i586-mingw32msvc-ar
	WINDRES	=	i586-mingw32msvc-windres
	PLATFORM=	win32
else
ifeq ($(BUILDFOR), WIN64)
	#Windows64 specifics, for cross compiling to windows
	BIN	=	$(TOPDIR)/bin/3dstuffz.exe
	PREFIX	=	/usr/x86_64-w64-mingw32
	LDFLAGS	+=	-lwinmm
	CC	=	x86_64-w64-mingw32-gcc
	STRIP	=	x86_64-w64-mingw32-strip
	AR	=	x86_64-w64-mingw32-ar
	WINDRES	=	x86_64-w64-mingw32-windres
	PLATFORM=	win32
else
ifeq ($(strip $(SBOX_UNAME_MACHINE)), arm)
	#Maemo specifics
	CFLAGS	+=	-DUSE_NEON -lGLUES
	LDFLAGS	+=	`sdl-config --libs` 
	PLATFORM=	linux
else
ifneq (,$(findstring -DPANDORA, $(CFLAGS)))
	#Pandora specifics
	PREFIX	=	/usr/local/angstrom/arm/arm-angstrom-linux-gnueabi/usr
	CFLAGS	+=	-DUSE_NEON
	LDFLAGS	+=	-lGLUES
	PLATFORM=	linux
else
	#Linux defaults
	LDFLAGS	+=	-lGLU
	PLATFORM=	linux
endif
endif
endif
endif
endif

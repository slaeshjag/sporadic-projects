# Project: 3dstuffz
include $(TOPDIR)/config/config.mk

DEPENDS		=
DEPFILES	=	

LIB		=	../../bin/ai.code
SRCFILES	=	$(wildcard *.c)
OBJFILES	=	$(SRCFILES:.c=.o)
CFLAGS		+=	-fPIC -Wl,-soname,ai.code -shared
.PHONY: all clean

all: $(OBJFILES) $(DEPENDS)
	@echo " [ LD ] $(LIB)"
	@$(CC) $(CFLAGS) $(OBJFILES) -o $(LIB) $(DEPFILES) $(LDFLAGS)
	@echo "Done."
	@echo
	
clean:
	@echo
	@echo " [ RM ] $(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo "Done."
	@echo 

%.o: %.c %.h
	@echo " [ CC ] src/ai/$<"
	@$(CC) $(CFLAGS) -c -o $@ $<
	


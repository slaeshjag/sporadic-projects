# Project: 3dstuffz
include $(TOPDIR)/config/config.mk

DEPENDS		=	ai
DEPFILES	=	

SRCFILES	=	$(wildcard *.c)
OBJFILES	=	$(SRCFILES:.c=.o)
.PHONY: all clean ai

all: $(OBJFILES) $(DEPENDS)
	@echo " [ LD ] $(BIN)"
	@$(CC) $(CFLAGS) $(OBJFILES) -o $(BIN) $(DEPFILES) $(LDFLAGS)
	@echo "Done."
	@echo
	
ai:
	@echo " [ CD ] src/ai"
	+@make -C ai/

clean:
	@echo
	@echo " [ RM ] $(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo " [ CD ] src/ai"
	+@make -C ai/ clean
	@echo "Done."
	@echo 

%.o: %.c %.h
	@echo " [ CC ] src/$<"
	@$(CC) $(CFLAGS) -c -o $@ $<
	


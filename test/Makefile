include ../MakeRules.mk
include ../MakeNames.mk

EXEC_DIR=../src
INC_DIR=-I$(EXEC_DIR)

TESTS+= wikipedia

###############################################################################
# Global targets
###############################################################################
all : $(TESTS)

wikipedia : wikipedia_test
	@echo "[$^] 1+0+1*1+1+1+1+1+1*1"
	@$^ 1+0+1*1+1+1+1+1+1*1

clean :
	rm -f wikipedia_test wikipedia.o wikipedia.c
	rm -f $(TESTS)

###############################################################################
# Rules
###############################################################################
%_test : %.o
	@echo "[ LINK] $@"
	@$(LD) $(LD_FLAGS) -o $@ $^

%.cpp : %.bnf2c.cpp $(EXEC_DIR)/$(EXEC_BNF2C)
	@echo "[BNF2C] $@"
	@cat $< | $(EXEC_DIR)/$(EXEC_BNF2C) > $@
#	@$(EXEC_DIR)/$(EXEC_BNF2C) $< -o $@
	
%.c : %.bnf2c.c $(EXEC_DIR)/$(EXEC_BNF2C)
	@echo "[BNF2C] $@"
	@cat $< | $(EXEC_DIR)/$(EXEC_BNF2C) > $@
#	@$(EXEC_DIR)/$(EXEC_BNF2C) $< -o $@

###############################################################################
# Targets
###############################################################################
$(EXEC_DIR)/$(EXEC_BNF2C) :
	@(cd $(EXEC_DIR); $(MAKE))
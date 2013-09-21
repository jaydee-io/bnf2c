include ../MakeRules.mk
include ../MakeNames.mk

EXEC_DIR=../src
INC_DIR=-I$(EXEC_DIR)

TESTS+= wikipedia_sample_test

###############################################################################
# Global targets
###############################################################################
all : $(TESTS)

wikipedia_sample : wikipedia_sample.o
	@echo "[ LINK] $@"
	@$(LD) $(LD_FLAGS) -o $@ $^
	
wikipedia_sample_test : wikipedia_sample
	@echo "[$^] 1+0+1*1+1+1+1+1+1*1"
	@$^ 1+0+1*1+1+1+1+1+1*1

clean :
	rm -f wikipedia_sample wikipedia_sample.o wikipedia_sample.c
	rm -f $(TESTS)

###############################################################################
# Rules
###############################################################################
%.c : %.bnf2c.c $(EXEC_DIR)/$(EXEC_BNF2C)
	@echo "[BNF2C] $@"
	@cat $< | $(EXEC_DIR)/$(EXEC_BNF2C) > $@
#	@$(EXEC_DIR)/$(EXEC_BNF2C) $< -o $@

###############################################################################
# Targets
###############################################################################
$(EXEC_DIR)/$(EXEC_BNF2C) :
	@(cd $(EXEC_DIR); $(MAKE))
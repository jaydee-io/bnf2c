include ../MakeRules.mk
include ../MakeNames.mk

EXEC_DIR=../src
INC_DIR=-I$(EXEC_DIR)

TESTS+= bnf_sample_test

###############################################################################
# Global targets
###############################################################################
all : $(TESTS)

bnf_sample : bnf_sample.o
	@echo "[ LINK] $@"
	@$(LD) $(LD_FLAGS) -o $@ $^
	
bnf_sample_test : bnf_sample
	@echo "[$@] 1+0+1*1+1+1+1+1+1*1"
	@$^ 1+0+1*1+1+1+1+1+1*1

clean :
	rm -f bnf_sample bnf_sample.o bnf_sample.c
	rm -f $(TESTS)

###############################################################################
# Rules
###############################################################################
%.c : %.bnf2c.c $(EXEC_DIR)/$(EXEC_BNF2C)
	@echo "[BNF2C] $@"
	@$(EXEC_DIR)/$(EXEC_BNF2C) $^ > $@

###############################################################################
# Targets
###############################################################################
$(EXEC_DIR)/$(EXEC_BNF2C) :
	@(cd $(EXEC_DIR); $(MAKE))
include MakeNames.mk

###############################################################################
# Global targets
###############################################################################
.PHONY : clean test $(EXEC_BNF2C)

all : $(EXEC_BNF2C)

# bnf2c
$(EXEC_BNF2C) :
	@(cd src; $(MAKE))

# Test
test :
	@(cd test; $(MAKE))


# Clean
clean :
	@(cd src;  $(MAKE) $@)
	@(cd test; $(MAKE) $@)
	
clean_test :
	@(cd test; $(MAKE) clean)
include MakeNames.mk

###############################################################################
# Global targets
###############################################################################
.phony : clean

all : $(EXEC_BNF2C)

# bnf2c
$(EXEC_BNF2C) : src/$(EXEC_BNF2C)

src/$(EXEC_BNF2C) :
	@(cd src; $(MAKE))

# Test
test : test/$(EXEC_TEST)

test/$(EXEC_TEST) :
	@(cd test; $(MAKE))


# Clean
clean :
	@(cd src;  $(MAKE) $@)
	@(cd test; $(MAKE) $@)
include ../MakeRules.mk
include ../MakeNames.mk

OBJS+= Token.o
OBJS+= LexerBNF.o
OBJS+= ParserBNF.o
OBJS+= Dictionnary.o
OBJS+= Symbol.o
OBJS+= Rule.o
OBJS+= Grammar.o
OBJS+= ParserState.o
OBJS+= ParsingTable.o
OBJS+= OutputFormatter.o
OBJS+= main.o

###############################################################################
# Global targets
###############################################################################
.PHONY : clean

all : $(EXEC_BNF2C)

clean :
	rm -f $(EXEC_BNF2C)
	rm -f *.o

###############################################################################
# Targets
###############################################################################
$(EXEC_BNF2C) : $(OBJS)
	@echo "[ LINK] $@"
	@$(LD) $(LD_FLAGS) -o $@ $^
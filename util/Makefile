SRCDIR 					:= src
INCDIR					:= headers
OUTDIR 					:= lib
OBJDIR					:= $(OUTDIR)/obj

TARGET					:= $(OUTDIR)/libutil.a
LIBS					:=
INCLUDE 				:= -I../util/include

CC  					:= @gcc -c
AR						:= @ar rcs
RM                      := @rm -rf
MKDIR                	:= @mkdir -p
RUN						:= @./$(TARGET)



OS						?= $(shell uname)

INC						:= $(wildcard $(INCDIR)/*.h)
SRC						:= $(wildcard $(SRCDIR)/*.c)
OBJ						:= $(addprefix $(OBJDIR)/, $(notdir $(SRC:.c=.o)))

INCLUDE					+= $(addprefix -I, $(INCDIR))
CFLAGS					+= -Wall -Werror
CLFLAGS					+=

ECHO					:= @echo
ECHO_BEGIN				:= $(ECHO)
ECHO_END 				:= $(ECHO) "done."

.PHONY: all clean clean-all run

all: $(TARGET) clean

$(TARGET): $(OBJ)
	$(MKDIR) $(@D)
	$(ECHO_BEGIN) "linking $@..."
	$(AR) $@ $^ $(LIBS)
	$(ECHO_END)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INC)
	$(MKDIR) $(@D)
	$(ECHO_BEGIN) "compiling $< into $@..."
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@
	$(ECHO_END)

clean:
	$(ECHO_BEGIN) "removing object files..."
	$(RM) $(OBJDIR)
	$(ECHO_END)

clean-all:
	$(ECHO_BEGIN) "removing $(OUTDIR)..."
	$(RM) $(OUTDIR)
	$(ECHO_END)

run: $(TARGET)
	$(ECHO) "start $(TARGET)"
	$(ECHO) "________________________________"
	$(RUN)

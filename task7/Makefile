SRCDIR 					:= src
INCDIR					:=
OUTDIR 					:= out
OBJDIR					:= $(OUTDIR)/obj

TARGET					:= $(OUTDIR)/task
LIBS					:= ../util/lib/libutil.a
INCLUDE 				:= -I../util/include

CC  					:= @gcc -c
CL						:= @gcc
RM                      := @rm -rf
MKDIR					:= @mkdir -p
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

ifeq ($(OS), SunOS)
CFLAGS 					+= -threads
endif

.PHONY: all clean clean-all run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(MKDIR) $(@D)
	$(ECHO_BEGIN) "linking $@..."
	$(CL) $(CFLAGS) $(CLFLAGS) $^ $(LIBS) -o $@
	$(ECHO_END)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INC)
	$(MKDIR) $(@D)
	$(ECHO_BEGIN) "compiling $< into $@..."
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@
	$(ECHO_END)

clean:
	$(ECHO_BEGIN) "removing object files..."
	$(RM) $(OBJ)
	$(ECHO_END)

clean-all:
	$(ECHO_BEGIN) "removing $(OUTDIR)..."
	$(RM) $(OUTDIR)
	$(ECHO_END)

run: $(TARGET)
	$(ECHO) "start $(TARGET)"
	$(ECHO) "________________________________"
	$(RUN) $(ARGS)

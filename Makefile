# compiler
CC          := clang++-7

# program target binary name
TARGET      := program.bin

# project structure
SRCDIR      := src
INCDIR      := inc # src/misc ?
BUILDDIR    := tmp
TARGETDIR   := bin
RESDIR      := res

# file extensions
SRCEXT      := cpp
DEPEXT      := d
OBJEXT      := o

# flags, libraries, and includes (TODO: make -g or -g3 conditional?)

#CFLAGS      := -Wall -Wformat -std=c++1z -O0 -g3
CFLAGS      := -Wall -Wformat -std=c++1z -O3
LIB         := -lGL -lGLU -lglfw -lGLEW -lassimp
INC         := -I$(INCDIR) -I/usr/local/include
INCDEP      := -I$(INCDIR)

#------------------------------------------------------------#
#               DO NOT EDIT BELOW THIS LINE                  #
#------------------------------------------------------------#

SOURCES     := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

# default make
all: compiledb

# generate compile database (compile_commands.json) silently & filtered
compiledb:
	@compiledb make resources $(TARGET) -s
	@printf "\n"

# remake
remake: fullclean all

# copy resources (from resources dir to target dir)
resources: directories
	@cp $(RESDIR)/* $(TARGETDIR)/

# make the dirs
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

# clean (only objects)
clean:
	@$(RM) -rf $(BUILDDIR)

# full clean (objects and binaries)
fullclean: clean
	@$(RM) -rf $(TARGETDIR)

# pull in dependency info for existing object files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

# link
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(TARGET) $^ $(LIB)

# compile -- ANSI version: @printf "Compiling '\e[4;93m%s\e[24;0m'\n" $<
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	@printf "Compiling '%s'\n" $<
	@$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

# non-file targets
.PHONY: all remake clean fullclean resources

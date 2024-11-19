NAME = opengl-example

### Cross-platform stuff

RESET	=	"\033[0m"
RED		=	"\033[0;31m"
GREEN	=	"\033[0;32m"

OSFLAG := 
ifeq ($(OS),Windows_NT)
	OSFLAG := windows
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		OSFLAG := linux
	endif
	ifeq ($(UNAME_S),Darwin)
		OSFLAG := macos
	endif
endif
ifeq ($(OSFLAG),)
_:=$(error "Could not estimate the current target platform")
endif

### Compilation

ifeq ($(MODE),cpp)
COMPILER = $(CXX)
COMPILERFLAGS = $(CXXFLAGS)
else
COMPILER = $(CC)
COMPILERFLAGS = $(CFLAGS)
endif

# C compiler
CC = $(CC_$(OSFLAG))
CC_windows	= x86_64-w64-mingw32-gcc
CC_linux	= gcc
CC_macos	= gcc
# C compiler flags
CFLAGS = \
	-Wall \
	-Wextra \
	-Winline \
	-MMD \
	$(CFLAGS_$(OSFLAG)) \
	-g # "-g" for debug, "-O" for release
CFLAGS_windows	= -mwindows -I./ -L./ -I./lib
CFLAGS_linux	= -Wno-unused-result #-fsanitize=address -ldl
CFLAGS_macos	= 

# C++ compiler
CXX = $(CXX_$(OSFLAG))
CXX_windows	= x86_64-w64-mingw32-g++
CXX_linux	= g++
CXX_macos	= g++
# C++ compiler flags
CXXFLAGS = \
	-Wall \
	-Wextra \
	-Winline \
	-MMD \
	$(CXXFLAGS_$(OSFLAG)) \
	-g # "-g" for debug, "-O" for release
CXXFLAGS_windows	= -mwindows -I./ -L./
CXXFLAGS_linux	= -Wno-unused-result #-fsanitize=address -ldl
CXXFLAGS_macos	= 

# Linker
LDFLAGS = $(LDFLAGS_$(OSFLAG))
LDFLAGS_windows	= $(LIBS)
LDFLAGS_linux	= $(LIBS)
LDFLAGS_macos	= $(LIBS)

### Libraries

LIBS = $(LIBGLFW) $(LIBMATH)

INCLUDE	= $(INCLUDE_$(OSFLAG))
INCLUDE_windows	= 
INCLUDE_linux	= 
INCLUDE_macos	= 

LIBMATH = -lm

# window/input system: GLFW
LIBGLFW = $(LIBGLFW_$(OSFLAG))
LIBGLFW_windows	= $(LIBDIR)/glfw/lib-mingw-w64/libglfw3.a -lgdi32 -lopengl32
LIBGLFW_linux	= $$( pkg-config --libs glfw3 ) -lGL
LIBGLFW_macos	= -lglfw -framework Cocoa -framework OpenGL

# window/input system: GLUT
LIBGLUT = $(LIBGLUT_$(OSFLAG))
LIBGLUT_windows	= -lopengl32 -lglu32 -lglut32
LIBGLUT_linux	= -lGL -lGLU -lglut
LIBGLUT_macos	= -framework Carbon -framework OpenGL -framework GLUT

# window/input system: SDL2
LIBSDL2 = $(LIBSDL2_$(OSFLAG))
LIBSDL2_windows	= -L$(SDLDIR) -lSDL2
LIBSDL2_linux	= -L$(SDLDIR) -lSDL2
LIBSDL2_macos	= -L$(SDLDIR)/SDL2.framework/Versions/Current -F. -framework SDL2

# list of libraries that need to be built with their own makefiles
LIBRARIES = 

### Folders

LIBDIR = ./lib
SRCDIR = ./src
OBJDIR = ./obj
BINDIR = ./bin

### Files

HDRS = \

SRCS = \
example.c \

# object files, for minimal recompiling
OBJS = ${SRCS:%.c=$(OBJDIR)/$(OSFLAG)/%.o}
# object file include dependency lists, for minimal recompiling
DEPS = ${OBJS:.o=.d}

### Rules

all: libraries build

build: ./$(BINDIR)/$(OSFLAG)/$(NAME)

libraries:
	@for library in $(LIBRARIES) ; do $(MAKE) -C $(LIBDIR)/$$library ; done

clean:
	@for library in $(LIBRARIES) ; do $(MAKE) -C $(LIBDIR)/$$library clean ; done
	@printf "Deleting object files...\n"
	@rm -f $(OBJS)

fclean: clean
	@for library in $(LIBRARIES) ; do $(MAKE) -C $(LIBDIR)/$$library fclean ; done
	@printf "Deleting program: "$(NAME)"\n"
	@rm -f $(NAME)

re: fclean all

test: all
	@./$(BINDIR)/$(OSFLAG)/$(NAME)

$(BINDIR)/$(OSFLAG)/$(NAME): $(OBJS) $(HDRS)
	@mkdir -p `dirname $@`
	@printf "Compiling program: "$@" -> "
	@$(COMPILER) $(OBJS) -o $@ $(COMPILERFLAGS) $(LDFLAGS)
	@printf $(GREEN)"OK!"$(RESET)"\n"

$(OBJDIR)/$(OSFLAG)/%.o : $(SRCDIR)/%.c
	@mkdir -p `dirname $@`
	@printf "Compiling file: "$@" -> "
	@$(COMPILER) $(COMPILERFLAGS) $(INCLUDE) -c $< -o $@ -MF $(OBJDIR)/$*.d
	@printf $(GREEN)"OK!"$(RESET)"\n"

-include ${DEPS}

# used to have makefile understand these rules are not named after files
.PHONY: all build libraries clean fclean re test

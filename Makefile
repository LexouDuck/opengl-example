NAME = opengl-example

### Folders

LIBDIR = ./lib
SRCDIR = ./src
OBJDIR = ./obj
BINDIR = ./bin

### Files

HDRS = \

SRCS := main.cpp
#$(call find $(SRCDIR) -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp")
#$(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.hpp)
#$(warning $(SRCS))

# object files, for minimal recompiling
OBJS = ${SRCS:%.cpp=$(OBJDIR)/$(OSFLAG)/%.o}
# object file include dependency lists, for minimal recompiling
DEPS = ${OBJS:.o=.d}

# window/input system chosen
WINDOWER ?= GLFW
#WINDOWER ?= GLUT
#WINDOWER ?= SFML
#WINDOWER ?= SDL2
#WINDOWER ?= IMGUI

# list of libraries that need to be built with their own makefiles
LIBRARIES = 

### Cross-platform stuff

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

COMPILER      ?= $(CC)
COMPILERFLAGS ?= $(CFLAGS)
ifeq ($(MODE),cpp)
COMPILER      = $(CXX)
COMPILERFLAGS = $(CXXFLAGS)
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
CFLAGS_windows	= -I./ -I$(LIBDIR) -mwindows
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
CXXFLAGS_windows	= -mwindows
CXXFLAGS_linux	= -Wno-unused-result #-fsanitize=address -ldl
CXXFLAGS_macos	= 

# Linker
LDFLAGS = $(LDFLAGS_$(OSFLAG))
LDFLAGS_windows	= -L./ -L$(LIBDIR) $(LIBS)
LDFLAGS_linux	= $(LIBS)
LDFLAGS_macos	= $(LIBS)

### Libraries

LIBS = $(LIBGLFW) $(LIBMATH)

INCLUDE	= $(INCLUDE_$(OSFLAG))
INCLUDE_windows	= -I./ -I$(LIBDIR) -I$(SRCDIR)
INCLUDE_linux	= -I./ -I$(LIBDIR) -I$(SRCDIR)
INCLUDE_macos	= -I./ -I$(LIBDIR) -I$(SRCDIR)

LIBMATH = -lm

# window/input system: GLFW -> https://www.glfw.org/
LIBGLFW = $(LIBGLFW_$(OSFLAG))
LIBGLFW_windows	= $(LIBDIR)/glfw/lib-mingw-w64/libglfw3.a -lgdi32 -lopengl32
LIBGLFW_linux	= $$( pkg-config --libs glfw3 ) -lGL
LIBGLFW_macos	= -lglfw -framework Cocoa -framework OpenGL
PKGGLFW = $(PKGGLFW_$(OSFLAG))
PKGGLFW_windows	= 
PKGGLFW_linux	= libglfw3 libglfw3-dev
PKGGLFW_macos	= libglfw3 libglfw3-dev

# window/input system: GLUT -> https://www.opengl.org/resources/libraries/glut/
LIBGLUT = $(LIBGLUT_$(OSFLAG))
LIBGLUT_windows	= -lglu32 -lglut32 -lopengl32
LIBGLUT_linux	= -lGL -lGLU -lglut
LIBGLUT_macos	= -framework Carbon -framework OpenGL -framework GLUT
PKGGLUT = $(PKGGLUT_$(OSFLAG))
PKGGLUT_windows	= mingw-w64-x86_64-freeglut
PKGGLUT_linux	= freeglut3-dev
PKGGLUT_macos	= freeglut

# window/input system: SFML -> https://www.sfml-dev.org/
LIBSFML = $(LIBSFML_$(OSFLAG))
LIBSFML_windows	= -lglu32 -lglut32 -lopengl32
LIBSFML_linux	= -lGL -lGLU -lglut
LIBSFML_macos	= -framework Carbon -framework OpenGL -framework GLUT
PKGSFML = $(PKGSFML_$(OSFLAG))
PKGSFML_windows	= 
PKGSFML_linux	= libsfml-dev
PKGSFML_macos	= libsfml-dev

# window/input system: SDL2 -> https://www.libsdl.org/
LIBSDL2 = $(LIBSDL2_$(OSFLAG))
LIBSDL2_windows	= -L$(SDLDIR) -lSDL2
LIBSDL2_linux	= -L$(SDLDIR) -lSDL2
LIBSDL2_macos	= -L$(SDLDIR)/SDL2.framework/Versions/Current -F. -framework SDL2
PKGSDL2 = $(PKGSDL2_$(OSFLAG))
PKGSDL2_windows	= 
PKGSDL2_linux	= libsdl2-dev
PKGSDL2_macos	= libsdl2-dev

# window/input system: IMGUI -> https://www.dearimgui.com/
LIBIMGUI = $(LIBSDL2_$(OSFLAG))
LIBIMGUI_windows	= 
LIBIMGUI_linux	= 
LIBIMGUI_macos	= 
PKGIMGUI = $(PKGSDL2_$(OSFLAG))
PKGIMGUI_windows	= 
PKGIMGUI_linux	= 
PKGIMGUI_macos	= 

### General utility stuff

RESET	=	"\033[0m"
RED		=	"\033[0;31m"
GREEN	=	"\033[0;32m"

#! This allows us to use 'sudo' for certain operations while remaining cross-platform
ifeq ($(OS),Windows_NT)
	SUDO =
else
	SUDO = sudo
endif

#! The shell command to install a prerequisite program/library (uses the appropriate OS-specific package manager)
#	@param 1	The name of the program/library/package to install
install_prereq = \
	if   [ -x "`command -v apk     `" ]; then $(SUDO) apk add --no-cache $(1) ; \
	elif [ -x "`command -v apt-get `" ]; then $(SUDO) apt-get install    $(1) ; \
	elif [ -x "`command -v brew    `" ]; then $(SUDO) brew    install    $(1) ; \
	elif [ -x "`command -v choco   `" ]; then $(SUDO) choco   install    $(1) ; \
	elif [ -x "`command -v pacman  `" ]; then $(SUDO) pacman  -S         $(1) ; \
	elif [ -x "`command -v yum     `" ]; then $(SUDO) yum     install    $(1) ; \
	elif [ -x "`command -v dnf     `" ]; then $(SUDO) dnf     install    $(1) ; \
	elif [ -x "`command -v zypp    `" ]; then $(SUDO) zypp    install    $(1) ; \
	elif [ -x "`command -v zypper  `" ]; then $(SUDO) zypper  install    $(1) ; \
	else \
		printf $(RED)"No package manager program was found. You must manually install: $(1)"$(RESET) >&2 ; \
	fi

### Rules

all: libraries build

build: ./$(BINDIR)/$(OSFLAG)/$(NAME)

prereq:
	@$(call install_prereq,$(PKG$(WINDOWER)))

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

$(OBJDIR)/$(OSFLAG)/%.o : $(SRCDIR)/%.cpp
	@mkdir -p `dirname $@`
	@printf "Compiling file: "$@" -> "
	@$(COMPILER) $(COMPILERFLAGS) $(INCLUDE) -c $< -o $@ -MF $(OBJDIR)/$*.d
	@printf $(GREEN)"OK!"$(RESET)"\n"

-include ${DEPS}

# used to have makefile understand these rules are not named after files
.PHONY: all build prereq libraries clean fclean re test

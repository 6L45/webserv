##############################################################################
#	MAKEFILE by Dmercadi
##############################################################################

##############################################################################
#	VARIABLES DEFINITION
##############################################################################
#	Executable output name
NAME_EXECUTABLE = client_simulator
#	Directory paths
HEADER_DIRECTORY = includes
SOURCE_DIRECTORY = .
#	File Objects
SRCS = $(foreach dir, ${SOURCE_DIRECTORY}, ${wildcard $(dir)/*.cpp})
OBJS = ${SRCS:.cpp=.o}
#	Compiler Flags
COMPILER_FLAGS = #-Wall -Wextra -Werror -std=c++98 #-g -fsanitize=address
INCLUDE_FLAGS = -I${HEADER_DIRECTORY}
DEBUG_FLAGS = -DWS_DEBUG
#	Compiler
CXX = c++

#	Terminal output coloring
_END=\x1b[0m
_BOLD=\x1b[1m
_UNDER=\x1b[4m
_REV=\x1b[7m

_GREY=\x1b[30m
_RED=\x1b[31m
_GREEN=\x1b[32m
_YELLOW=\x1b[33m
_BLUE=\x1b[34m
_PURPLE=\x1b[35m
_CYAN=\x1b[36m
_WHITE=\x1b[37m

_IGREY=\x1b[40m
_IRED=\x1b[41m
_IGREEN=\x1b[42m
_IYELLOW=\x1b[43m
_IBLUE=\x1b[44m
_IPURPLE=\x1b[45m
_ICYAN=\x1b[46m
_IWHITE=\x1b[47m

.cpp.o:
	${CXX} ${COMPILER_FLAGS} ${INCLUDE_FLAGS} -c $< -o ${<:.cpp=.o}

${NAME_EXECUTABLE}: ${OBJS}
	@echo "${_YELLOW}Compiling ${NAME_EXECUTABLE}"
	@${CXX} ${COMPILER_FLAGS} ${OBJS} -o ${NAME_EXECUTABLE} && echo "${_GREEN}${NAME_EXECUTABLE} compiled sucessfully" || echo "${_RED}compilation failed"
	@echo "${_END}"

all: ${NAME_EXECUTABLE}

clean:
	@rm -f ${OBJS}

fclean: clean
	@rm -f ${NAME_EXECUTABLE}

re: fclean ${NAME_EXECUTABLE}

.PHONY: all, clean, fclean, re, bonus
ifeq ($(origin CC),default)
  CC = g++
endif


SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

CFLAGS ?= 
LDFLAGS = $(SFML_FLAGS)

BUILD_TYPE ?= debug
# BUILD_TYPE ?= release


ifeq ($(BUILD_TYPE), release)
	CFLAGS += -DNDEBUG -O3 -ffast-math -flto -g0 -fvisibility=hidden -march=native -s
endif

ifeq ($(BUILD_TYPE), debug)
	CFLAGS += -g -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ 								         \
			  -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported 							      \
			  -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations 					           \
			  -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op 					            \
			  -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith 				         \
			  -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral 			              \
			  -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wstack-usage=8192                	   \
			  -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel              \
			  -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types             \
			  -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused           \
			  -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing          \
			  -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector         \
			  -pie -fPIE -Werror=vla 																		                 \
			  -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr \


	LDFLAGS += -fsanitize=address,undefined -lasan -lubsan
endif

-include make/common.mk

OUT_O_DIR       ?= bin
EXECUTABLE_DIR ?= build
INCLUDE 	    = -I./$(BACK_DIR)/include $(COMMON_INC)
SRC 		    = ./src
EXECUTABLE     ?= backend


override CFLAGS += $(INCLUDE)

CSRC =  $(BACK_DIR)/main.cpp                                      \
		$(BACK_DIR)/src/stack/hash.cpp                             \
		$(BACK_DIR)/src/stack/stack.cpp                             \
		$(BACK_DIR)/src/console/consoleCmd.cpp                       \
		$(BACK_DIR)/src/assembler/assembler.cpp                       \
		$(BACK_DIR)/src/processor/processor.cpp                        \
		$(COMMON_DIR)/src/lib/lib.cpp                                   \
		$(COMMON_DIR)/src/read-file/read-file.cpp                        \
		$(COMMON_DIR)/src/tree/read-write-tree/read-tree/read-tree.cpp    \



ifeq ($(BUILD_TYPE), debug)
	CSRC += $(COMMON_DIR)/src/log/log.cpp                 \
			$(COMMON_DIR)/src/dump/global-dump.cpp         \
			$(COMMON_DIR)/src/tree/tree-dump/tree-dump.cpp  \


endif

COBJ := $(addprefix $(OUT_O_DIR)/,$(CSRC:.cpp=.o))
DEPS = $(COBJ:.o=.d)

.PHONY: all
all: $(EXECUTABLE_DIR)/$(EXECUTABLE)

$(EXECUTABLE_DIR)/$(EXECUTABLE): $(COBJ)
	@mkdir -p $(@D)
	$(CC) $^ -o $@ $(LDFLAGS)

$(COBJ) : $(OUT_O_DIR)/%.o : %.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(DEPS) : $(OUT_O_DIR)/%.d : %.cpp
	@mkdir -p $(@D)
	@$(CC) -E $(CFLAGS) $< -MM -MT $(@:.d=.o) > $@


#======= run ==========================================

.PHONY: processor compile run rebuild makeCodeDir

ifeq ($(CODE_DIR), )
makeCodeDir:
else
makeCodeDir:
	@mkdir -p $(CODE_DIR)
endif

run:
	@./$(EXECUTABLE_DIR)/$(EXECUTABLE) -run $(CODE_DIR)/$(BIN_FILE)

rebuild:
	make clean && make

rerun:
	make && make run

#======= clean ========================================

.PHONY: clean cleanDirs cleanLog
clean:
	rm -rf $(COBJ) $(DEPS) $(EXECUTABLE_DIR)/$(EXECUTABLE) $(OUT_O_DIR)/$(SRC)

cleanDirs:
	rm -rf $(OUT_O_DIR) $(EXECUTABLE_DIR)

cleanLog:
	rm -rf Log/

#======================================================

NODEPS = clean

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
include $(DEPS)
endif

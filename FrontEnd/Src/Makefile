THIS_FILE := $(abspath $(lastword $(MAKEFILE_LIST)))
THIS_MAKEFILE_WAY := $(dir $(THIS_FILE))
MAIN_MAKEFILE_WAY ?= $(THIS_MAKEFILE_WAY)
RELATIVE_PATH = $(subst $(MAIN_MAKEFILE_WAY),,$(THIS_MAKEFILE_WAY))


ifeq ($(origin CC),default)
  CC = g++
endif


CFLAGS ?=
LDFLAGS =

# BUILD_TYPE ?= debug
BUILD_TYPE ?= release


ifeq ($(BUILD_TYPE), release)
	CFLAGS += -D _NDEBUG -O3
endif 

ifeq ($(BUILD_TYPE), debug)
	CFLAGS += -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++                                     \
			  -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations                       \
			  -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported                         \
			  -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral               \
			  -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith                 \
			  -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wstack-usage=8192             \
			  -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel     \
			  -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -pie -fPIE -Werror=vla     \
			  -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types     \
			  -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused   \
			  -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing  \
			  -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector \
			  -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr \

	LDFLAGS += -fsanitize=address,undefined -lasan -lubsan
endif



OUT_O_DIR	   ?= $(RELATIVE_PATH)bin
EXECUTABLE_DIR ?= $(RELATIVE_PATH)build
COMMONINC 	    = -I./$(RELATIVE_PATH)include
SRC 			= $(RELATIVE_PATH)src
EXECUTABLE 	   ?= frontend


-include $(RELATIVE_PATH)local.mk

override CFLAGS += $(COMMONINC)

CSRC =  $(RELATIVE_PATH)main.cpp 					  			   			     \
		$(RELATIVE_PATH)src/lib/lib.cpp										     \
		$(RELATIVE_PATH)src/tree/tree.cpp							             \
		$(RELATIVE_PATH)src/tree/nameTable/hash.cpp      				  	     \
		$(RELATIVE_PATH)src/tree/nameTable/nametable.cpp    				     \
		$(RELATIVE_PATH)src/tree/readTree/tokens/tokens.cpp 			         \
		$(RELATIVE_PATH)src/tree/fprintfTree/fprintfTree.cpp					 \
		$(RELATIVE_PATH)src/tree/readTree/fileread/fileread.cpp			         \
		$(RELATIVE_PATH)src/tree/readTree/syntaxErr/syntaxErr.cpp                \
		$(RELATIVE_PATH)src/tree/readTree/recursiveDescent/recursiveDescent.cpp  \

ifeq ($(BUILD_TYPE), debug)

CSRC += $(RELATIVE_PATH)src/log/log.cpp								             \
		$(RELATIVE_PATH)src/tree/treeDump/treeDump.cpp 				             \
		$(RELATIVE_PATH)src/tree/treeDump/globalDump.cpp 				     	 \
	    $(RELATIVE_PATH)src/tree/nameTable/nameTableLog/nameTableLog.cpp         \
		$(RELATIVE_PATH)src/tree/readTree/tokens/tokensDump/tokensDump.cpp       \

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

run:
	./$(EXECUTABLE_DIR)/$(EXECUTABLE)

rebuild:
	make clean && make

rerun:
	make && make run

#======= clean ========================================

.PHONY: clean clean_dirs clean_log clean_dot

clean:
	rm -rf $(COBJ) $(DEPS) $(EXECUTABLE_DIR)/$(EXECUTABLE) $(OUT_O_DIR)/$(SRC)

clean_dirs:
	rm -rf $(OUT_O_DIR) $(EXECUTABLE_DIR)

clean_log:
	rm -rf $(RELATIVE_PATH)Log/

clean_dot:
	rm -rf FrontEnd/dot/

#========= iwyu ======================================

.PHONY: iwyu

f ?= $(RELATIVE_PATH)main.cpp

iwyu:
	iwyu $(COMMONINC) $(f)

#==================================================

NODEPS = clean clean_dirs clean_log clean_dot iwyu

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
include $(DEPS)
endif

-include common.mk
-include local.mk

ifeq ($(origin CC),default)
  CC = g++
endif


CFLAGS ?=
LDFLAGS =

BUILD_TYPE ?= debug
# BUILD_TYPE ?= release


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



OUT_O_DIR	   ?= bin
EXECUTABLE_DIR ?= build
COMMONINC 	    = -I./include
SRC 			= src
EXECUTABLE 	   ?= frontend
FRONT_DIR      := frontend


override CFLAGS += $(COMMONINC)

CSRC =  $(FRONT_DIR)/main.cpp 					  			   			     \
		$(FRONT_DIR)/src/tree/tree.cpp							             \
		$(FRONT_DIR)/src/tree/nameTable/hash.cpp      				  	     \
		$(FRONT_DIR)/src/tree/nameTable/nametable.cpp    				     \
		$(FRONT_DIR)/src/tree/readTree/tokens/tokens.cpp 			         \
		$(FRONT_DIR)/src/tree/readTree/fileread/fileread.cpp	             \
		$(FRONT_DIR)/src/tree/readTree/syntaxErr/syntaxErr.cpp               \
		$(FRONT_DIR)/src/tree/readTree/recursiveDescent/recursiveDescent.cpp \
		$(COMMON_DIR)/src/lib/lib.cpp								         \
		$(COMMON_DIR)/src/tree/writeTree/writeTree.cpp					     \

ifeq ($(BUILD_TYPE), debug)

CSRC += $(FRONT_DIR)src/log/log.cpp								             \
		$(FRONT_DIR)src/tree/treeDump/treeDump.cpp 				             \
		$(FRONT_DIR)src/tree/treeDump/globalDump.cpp 				     	 \
	    $(FRONT_DIR)src/tree/nameTable/nameTableLog/nameTableLog.cpp         \
		$(FRONT_DIR)src/tree/readTree/tokens/tokensDump/tokensDump.cpp       \

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
	make clean_dot
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
	rm -rf ../Log/

clean_dot:
	rm -rf ../dot/

#========= iwyu ======================================

.PHONY: iwyu

f ?= main.cpp

iwyu:
	iwyu $(COMMONINC) $(f)

#==================================================

NODEPS = clean clean_dirs clean_log clean_dot iwyu

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
include $(DEPS)
endif

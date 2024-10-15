# Compiler
CC=g++-11


# Flags for debugging compilation
DEBUG_FLAGS=-D _DEBUG -ggdb3 -std=c++17 -O0 -Wall $\
-Wextra -Weffc++ -Waggressive-loop-optimizations $\
-Wc++14-compat -Wmissing-declarations -Wcast-align $\
-Wcast-qual -Wchar-subscripts -Wconditionally-supported $\
-Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal $\
-Wformat-security -Wformat-signedness $\
-Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor $\
-Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith $\
-Winit-self -Wredundant-decls -Wshadow -Wsign-conversion $\
-Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 $\
-Wsuggest-attribute=noreturn -Wsuggest-final-methods $\
-Wsuggest-final-types -Wsuggest-override -Wswitch-default $\
-Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code $\
-Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix $\
-Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast $\
-Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation $\
-fstack-protector -fstrict-overflow -flto-odr-type-merging $\
-fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 $\
-pie -fPIE -mavx2 -fsanitize=address,alignment,bool,$\
bounds,enum,float-cast-overflow,float-divide-by-zero,$\
integer-divide-by-zero,leak,nonnull-attribute,null,object-size,$\
return,returns-nonnull-attribute,shift,signed-integer-overflow,$\
undefined,unreachable,vla-bound,vptr


# Flags for release version compilation
RELEASE_FLAGS=-Wmissing-declarations -Wempty-body -DNDEBUG -DLOG_SWITCH_OFF -mavx2 $\
																	-DDEBUG_SWITCH_OFF


#-----------------------------------------------------------------------------------------


OBJECTS_DIR=objects


# Make OBJECTS_DIR if it doesn't exist
objects_dir:
	@if [ ! -d $(OBJECTS_DIR) ]; \
	then                         \
		mkdir $(OBJECTS_DIR);    \
	fi


# Clean objects dir
clean:
	@rm -f $(OBJECTS_DIR)/*.o $(EXECUTABLE)


#-----------------------------------------------------------------------------------------


# Log files and directory
LOG_SUBDIR=stack/logPrinter

LOG_SOURCE_FILES=logPrinter.cpp
LOG_HEADER_FILES=logPrinter.h logPrinterConfigs.h

LOG_SOURCES=$(patsubst %.cpp,$(LOG_SUBDIR)/%.cpp,$(LOG_SOURCE_FILES))
LOG_HEADERS=$(patsubst %.h,$(LOG_SUBDIR)/%.h,$(LOG_HEADER_FILES))
LOG_OBJECTS=$(patsubst %.cpp,$(OBJECTS_DIR)/%.o,$(LOG_SOURCE_FILES))


$(OBJECTS_DIR)/%.o: $(LOG_SUBDIR)/%.cpp $(LOG_HEADERS)
	@$(CC) -c $(DEBUG_FLAGS) $< -o $@


# Clean log files
log_clean:
	@rm -f logs/log.txt logs/emergencyLog.txt


#-----------------------------------------------------------------------------------------


STACK_DIR=stack
STACK_SOURCE_DIR=$(STACK_DIR)/sources
STACK_HEADER_DIR=$(STACK_DIR)/headers


STACK_SOURCE_FILES=stack.cpp myRecalloc.cpp canary.cpp hash.cpp
STACK_HEADER_FILES=stack.h myRecalloc.h canary.h stackConfigs.h hash.h

STACK_SOURCES=$(patsubst %.cpp,$(STACK_SOURCE_DIR)/%.cpp,$(STACK_SOURCE_FILES))
STACK_HEADERS=$(patsubst %.h,$(STACK_HEADER_DIR)/%.h,$(STACK_HEADER_FILES))
STACK_OBJECTS=$(patsubst %.cpp,$(OBJECTS_DIR)/%.o,$(STACK_SOURCE_FILES)) 


$(OBJECTS_DIR)/%.o: $(STACK_SOURCE_DIR)/%.cpp $(STACK_HEADERS) $(LOG_HEADERS)
	@$(CC) -c $(DEBUG_FLAGS) $< -o $@


#-----------------------------------------------------------------------------------------


# VM - virtual machine
VM_SOURCE_DIR=sources
VM_HEADER_DIR=headers

VM_SOURCE_FILES=processor.cpp assembler.cpp
VM_HEADER_FILES=processor.h	assembler.h

VM_SOURCES=$(patsubst %.cpp,$(VM_SOURCE_DIR)/%.cpp,$(VM_SOURCE_FILES))
VM_HEADERS=$(patsubst %.h,$(VM_HEADER_DIR)/%.h,$(VM_HEADER_FILES))
VM_OBJECTS=$(patsubst %.cpp,$(OBJECTS_DIR)/%.o,$(VM_SOURCE_FILES)) 


$(OBJECTS_DIR)/%.o: $(VM_SOURCE_DIR)/%.cpp $(VM_HEADERS) $(STACK_HEADERS) $(LOG_HEADERS)
	@$(CC) -c $(DEBUG_FLAGS) $< -o $@


#-----------------------------------------------------------------------------------------


MAIN_SOURCE=$(VM_SOURCE_DIR)/main.cpp
MAIN_OBJECT=$(patsubst $(VM_SOURCE_DIR)/%.cpp, $(OBJECTS_DIR)/%.o, $(MAIN_SOURCE))

EXECUTABLE=virtualMachine


# Make release version of program
all: release


# Run program`
run: $(EXECUTABLE)
	@./$<


# Make release version
release: objects_dir clean
	@$(CC) $(RELEASE_FLAGS) $(MAIN_SOURCE) $(VM_SOURCES) $(STACK_SOURCES) \
														$(LOG_SOURCES) -o $(EXECUTABLE)


# Make debug version
debug: objects_dir $(MAIN_OBJECT) $(VM_OBJECTS) $(VM_HEADERS) $(STACK_OBJECTS) $\
						 				$(STACK_HEADERS) $(LOG_OBJECTS) $(LOG_HEADERS)
	@$(CC) $(DEBUG_FLAGS) $(MAIN_OBJECT) $(VM_OBJECTS) $(STACK_OBJECTS) $(LOG_OBJECTS) \
																		-o $(EXECUTABLE)


print:
	echo $(MAIN_OBJECT)
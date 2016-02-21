program_NAME := i2c_terminal_report
program_C_SRCS := $(wildcard *.c)
program_C_OBJS := ${program_C_SRCS:.c=.o} $(wildcard ../i2c_driver/*.o)
program_OBJS := $(program_C_OBJS)
program_INCLUDE_DIRS := ../../cgoslib ../i2c_driver
program_LIBRARY_DIRS := ../../cgoslib/Lx
program_LIBRARIES := cgos

CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))


CPPFLAGS += -Wall -Werror
LDFLAGS += -Wl,-rpath=/root/git/internship/source/cgoslib/Lx/


##set debug symbols flag for gdb
CPPFLAGS += -g

##link real time functions
CPPFLAGS += -lrt

##link threading library
CPPFLAGS += -lpthread


.PHONY: all clean distclean head resethard q quick

all: $(program_NAME)

q: quick

quick: resethard clean all

pull:
	git pull

resethard:
	git reset HEAD --hard

$(program_NAME): $(program_OBJS)
	$(LINK.cc) $(program_OBJS) -o $(program_NAME)

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_OBJS)

distclean: clean


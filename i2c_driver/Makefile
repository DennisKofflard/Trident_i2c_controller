program_NAME := i2c_driver
program_C_SRCS := $(wildcard *.c)
program_C_OBJS := ${program_C_SRCS:.c=.o}
program_OBJS := $(program_C_OBJS)
program_INCLUDE_DIRS := ../congatec_cgoslib ../i2c_driver
program_LIBRARY_DIRS := ../congatec_cgoslib/Lx
program_LIBRARIES := cgos

CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))


CPPFLAGS += -Wall -Werror
###LDFLAGS += -Wl,-rpath=/root/git/internship/source/cgoslib/Lx/
LDFLAGS += -Wl,-rpath=../congatec_cgoslib/Lx/


####set debug symbols flag for gdb
##CPPFLAGS += -g

##link real time functions
CPPFLAGS += -lrt

##link threading library
CPPFLAGS += -lpthread


.PHONY: all clean distclean head resethard update

all: $(program_NAME)

pull:
	git pull

resethard:
	git reset HEAD --hard

update:	clean pull resethard all

$(program_NAME): $(program_OBJS)
	##$(LINK.cc) $(program_OBJS) -o $(program_NAME)

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_OBJS)

distclean: clean


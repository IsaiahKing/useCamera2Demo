#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#

# define the C compiler to use
CC = g++

# define the lib.a creater to use
LIBAR = ar

# define the lib.a creater flags
AFLAGS = rcs

# define any compile-time flags
#CFLAGS = -Wall -g -std=gnu++11
CFLAGS = -Wall -fPIC -std=gnu++11 -DLINUX -DFACE_RECOGNITION -DUSE_EMMC

# define any directories containing header files other than /usr/include
#
#INCLUDES = -I../../Lib/include/ -I../../Sources/GTILibrary
INCLUDES = -I../../Lib/include/ -I../../Include -I../../Lib/Linux/OpenCV/ -I../../Sources/GTILibrary

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS =  -L../../Sources/GTILibrary -L../../Lib/Linux -L/usr/local/lib/
LFLAGS += -L/usr/local/GTISDK/Lib/Linux/OpenCV/x86/

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
#LIBS = -lmylib -lm
#LIBS = -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_videoio -lGTILibrary-static -lftd3xx -lpthread -lX11
LIBS = -lopencv_world -lGTILibrary-static -lftd3xx -lpthread -lX11

# define the C source files
SRCS = Sample.cpp Samples.cpp Classify.cpp

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
OBJS = $(SRCS:.cpp=.o)

# define the executable file 
MAIN = cnnSample

# define the static library file 
#MAIN = GTILibrary.a

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean

#all:    $(MAIN)
#	@echo  Simple compiler named mycc has been compiled

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $^ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
#.cpp.o:
#	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

#.a:
#	$(LIBAR) $(AFLAGS) $@ $^

clean:
	$(RM) *.o *~ $(MAIN)

#depend: $(SRCS)
#	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it


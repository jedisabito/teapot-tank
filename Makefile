# This sample Makefile allows you to make a OpenGL application
#   whose source is exactly one .c file.
#
#
# To use this Makefile, you must type:
#
#        make xxxx
#
# where xxxx.c or xxxx.cc is the name of the file you wish to compile

# This Makefile works because the default makefile rules include several
# magic variables that are used during processing, like this: 

# $(CC) $(CFLAGS) $(CPPFLAGS)

# Therefore, all we really need to do is define some variables.  This is
# really cool!

# Makefile written by SDA 8/30/2000


# This magic flag is used during compiling C programs

CFLAGS = -g -I/usr/include/GL

# This magic flag is used during compiling C++ programs

CXXFLAGS = $(CFLAGS)

# This magic flag is used during linking

LDLIBS = -L/usr/include/GL -lglut  -lGL -lGLU

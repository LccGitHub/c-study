
####### define compile toolchains #################################
#CROSS_COMPILE ?= ../prebuilt/toolchains/arm-qcom-linux-gnueabi/bin/arm-oe-linux-gnueabi-
CC:= gcc
CXX:= g++
####################################################################

## define compile option, include dir, library path and file #######
CFLAGS += -I./

#LDFLAGS = -L./../system/usr/lib/  


LDFLAGS += -lpthread

$(warning "CFLAGS is$(CFLAGS))
$(warning "LDFLAGS is$(LDFLAGS))

####################################################################

## define target, src file, object file #############################

#TARGET = test1 test2
#SRCS = $(wildcard test/*.cpp)
#OBJS = $(notdir $(patsubst %.cpp, %.o, $(SRCS)))

# target 1
DEMO1 = msgque
DEMO1_SRCS = $(wildcard ./*.c)
DEMO1_OBJS = $(notdir $(patsubst %.c, %.o, $(DEMO1_SRCS)))



$(warning "SRCS is$(DEMO1_SRCS))
$(warning "OBJS is$(DEMO1_OBJS))
#####################################################################


all: $(DEMO1) 

$(DEMO1): $(DEMO1_SRCS)
	$(CXX) -o $@ $^ $(CFLAGS) $(LDFLAGS)



.PHONY: clean

clean:
	$(hide)rm -rf $(DEMO1)




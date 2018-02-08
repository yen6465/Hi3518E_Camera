
INC_DIR := UsageEnvironment BasicUsageEnvironment liveMedia groupsock LiveStream
INC := $(patsubst %,-I%/include,$(INC_DIR)) -I./LiveStream/

SRC := $(wildcard *.cpp *.c $(patsubst %,%/*.cpp,$(INC_DIR)) $(patsubst %,%/*.c,$(INC_DIR)))
OBJ := $(patsubst %.cpp,%.obj,$(patsubst %.c,%.o,$(SRC)))
DEP := $(patsubst %.obj,%.dep,$(patsubst %.o,%.d,$(OBJ)))

COMPILE_PREFIX := @echo -n ".";
CC := $(COMPILE_PREFIX)cc
CFLAGS := $(INC) -O0 -Wall -DBSD=1 -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64
CXX := $(COMPILE_PREFIX)c++
CXXFLAGS := $(CFLAGS)
LD := $(COMPILE_PREFIX)c++ -o
LDFLAGS :=

TARGET := live555MediaServer

.PHONY : all clean

all : $(TARGET)

%.o : %.c
	$(CC) $(CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c $< -o $@

%.obj : %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -MF"$(@:%.obj=%.dep)" -MT"$(@:%.obj=%.dep)" -c $< -o $@

$(TARGET) : $(OBJ)
	$(LD) $@ $(LDFLAGS) $^

clean:
	@rm -rf $(OBJ) $(DEP) $(TARGET)

sinclude $(DEP)



INC_DIR := ../utils
INC := $(patsubst %,-I%/include,$(INC_DIR)) -I../utils/

SRC := $(wildcard *.c $(patsubst %,%/*.c,$(INC_DIR)))
OBJ := $(patsubst %.c,%.obj,$(patsubst %.c,%.o,$(SRC)))
DEP := $(patsubst %.obj,%.dep,$(patsubst %.o,%.d,$(OBJ)))

#COMPILE_PREFIX := @echo -n ".";
CC := $(COMPILE_PREFIX)cc
CFLAGS := $(INC) -O0 -Wall -DBSD=1 -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64
CXX := $(COMPILE_PREFIX)gcc
CXXFLAGS := $(CFLAGS)
LD := $(COMPILE_PREFIX)gcc -o
LDFLAGS :=
LIBS := -L. -llive555

TARGET := rtsp_server

.PHONY : all clean

all : $(TARGET)

%.o : %.c
	$(CC) $(CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c $< -o $@

%.obj : %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -MF"$(@:%.obj=%.dep)" -MT"$(@:%.obj=%.dep)" -c $< -o $@

$(TARGET) : $(OBJ)
	@cd live555;   make
	$(LD) $@ $(LDFLAGS) $^  $(LIBS)

clean:
	@cd live555;   make clean
	@rm liblive555MediaServer.so
	@rm -rf $(OBJ) $(DEP) $(TARGET)

sinclude $(DEP)



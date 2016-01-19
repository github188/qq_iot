CC		:= $(PREFIX)gcc
ifdef USE_CPP
CXX		:= $(PREFIX)g++
endif
AS		:= $(CC)
ifdef USE_CPP
LD		:= $(CXX)
else
LD		:= $(CC)
endif
STRIP	:= $(PREFIX)strip
ifdef O
BUILD_DIR=$(O)
else
BUILD_DIR=build_$(LOCAL_MODULE)_out
endif

#-g -rdynamic
#-lrt, for danale clock_gettime function
ifdef USE_CPP
CXXFLAGS	:= $(LOCAL_CFLAGS) $(addprefix -I,$(LOCAL_C_INCLUDES)) -Wall -D_GNU_SOURCE -fms-extensions -g -rdynamic
endif
CFLAGS		:= $(LOCAL_CFLAGS) $(addprefix -I,$(LOCAL_C_INCLUDES)) -Wall -D_GNU_SOURCE -std=c99 -fms-extensions -lrt  -rdynamic
LDFLAGS		:= $(LOCAL_LDFLAGS) $(addprefix -l,$(patsubst lib%,%,$(LOCAL_SHARED_LIBRARIES)))

ifdef USE_CPP
OBJSC		:= $(patsubst %.c,$(BUILD_DIR)/%.o,$(LOCAL_SRC_CFILES))
OBJSCXX := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(LOCAL_SRC_CXXFILES))
OBJS :=   $(OBJSCXX) $(OBJSC) 
else
OBJS		:= $(patsubst %.c,$(BUILD_DIR)/%.o,$(LOCAL_SRC_CFILES))
endif

OBJ_DIRS	:= $(sort $(dir $(OBJS)))


.PHONY: all checkdirs clean rebuild

TARGET		:= $(BUILD_DIR)/$(LOCAL_MODULE)

all: checkdirs $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $^ $(LDFLAGS) -o $@ 

ifdef USE_CPP
$(OBJSC): $(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJSCXX): $(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
else
$(OBJS): $(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
endif

checkdirs: $(OBJ_DIRS)

$(OBJ_DIRS):
	mkdir -p $@

install:
	$(STRIP) $(TARGET)
	$(CP) $(TARGET) $(ROOTFS_BIN_DIR)

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all install

.PHONY:	all install clean


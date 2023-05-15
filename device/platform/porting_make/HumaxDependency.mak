HUMAX_CFLAG += -MMD

HUMAX_DEPENDENCY_FILE := $(wildcard $(sub-obj-y:.o=.d))

-include $(HUMAX_DEPENDENCY_FILE)

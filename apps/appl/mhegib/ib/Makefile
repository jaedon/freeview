obj-y += MHEGEngine.o
obj-y += MHEGDisplay.o
obj-y += MHEGCanvas.o
obj-y += MHEGBackend.o
obj-y += MHEGApp.o
obj-y += MHEGColour.o
obj-y += MHEGFont.o
obj-y += MHEGTimer.o
obj-y += MHEGStreamPlayer.o
#obj-y += MHEGVideoOutput.o	// hmkim : commented out (StreamPlayer : CI+ MHEG Profile 에선 불필요. UK MHEG Profile 을 위해서는 추가 작업 필요함!)
#obj-y += videoout_null.o	// hmkim : commented out (StreamPlayer : CI+ MHEG Profile 에선 불필요. UK MHEG Profile 을 위해서는 추가 작업 필요함!)
#obj-y += videoout_xshm.o	// hmkim : commented out (StreamPlayer : CI+ MHEG Profile 에선 불필요. UK MHEG Profile 을 위해서는 추가 작업 필요함!)
#obj-y += MHEGAudioOutput.o	// hmkim : commented out (StreamPlayer : CI+ MHEG Profile 에선 불필요. UK MHEG Profile 을 위해서는 추가 작업 필요함!)

# Classes Begin
obj-y += ActionClass.o
obj-y += ApplicationClass.o
obj-y += AudioClass.o
obj-y += BitmapClass.o
obj-y += Content.o
obj-y += CursorShapeClass.o
obj-y += DynamicLineArtClass.o
obj-y += ElementaryAction.o
obj-y += EntryFieldClass.o
obj-y += EventType.o
obj-y += FontClass.o
obj-y += Generic.o
obj-y += GroupClass.o
obj-y += GroupItem.o
obj-y += HotspotClass.o
obj-y += HyperTextClass.o
obj-y += InterchangedProgramClass.o
obj-y += LineArtClass.o
obj-y += LinkClass.o
obj-y += ListGroupClass.o
obj-y += PaletteClass.o
obj-y += PushButtonClass.o
obj-y += RectangleClass.o
obj-y += Reference.o
obj-y += RemoteProgramClass.o
obj-y += ResidentProgramClass.o
obj-y += RootClass.o
obj-y += RTGraphicsClass.o
obj-y += SceneClass.o
obj-y += SliderClass.o
obj-y += StreamClass.o
obj-y += StreamComponent.o
obj-y += SwitchButtonClass.o
obj-y += TextClass.o
obj-y += TokenGroupClass.o
obj-y += VariableClass.o
obj-y += VideoClass.o
obj-y += VisibleClass.o
# Classes End

obj-y += ISO13522-MHEG-5.o
obj-y += der_decode.o
obj-y += clone.o
obj-y += si.o
obj-y += readpng.o
#obj-y += mpegts.o	// hmkim : commented out (StreamPlayer : CI+ MHEG Profile 에선 불필요. UK MHEG Profile 을 위해서는 추가 작업 필요함!)
obj-y += utils.o

# Adaptation file
obj-y += adaptation.o

###### Debugging command
#obj-$(CONFIG_DEBUG) += cmd/

# Temporary empty header
#ccflags-y += -I3rd_party/mheg/ib

# Basic include path

# Additional include path
ccflags-y += -I$(HUMAX_TOP_DIR)/apps/make/include
ccflags-y += -I$(HUMAX_TOP_DIR)/apps/humaxtv/hlib/include
ccflags-y += -I$(HUMAX_TOP_DIR)/apps/humaxtv/dlibs/datalibs/include
ccflags-y += -I$(HUMAX_TOP_DIR)/apps/humaxtv/octo/include
ccflags-y += -I$(HUMAX_TOP_DIR)/apps/humaxtv/appkit/base
ccflags-y += -I$(HUMAX_TOP_DIR)/apps/humaxtv/hama/include
ccflags-y += -I$(HUMAX_TOP_DIR)/apps/humaxtv/application/mlib/include
ccflags-y += -I../../../packages/libpng/include



scummvm_add_engine(exult
  SRCS src/ExultEngine.cpp src/MetaEngine_Exult.cpp
       adapter/ScummVMWindowAdapter.cpp adapter/ScummVMAudioAdapter.cpp
       adapter/ScummVMFileAdapter.cpp adapter/ScummVMTimerAdapter.cpp
       facade/GumpsFacade.cpp facade/AudioFacade.cpp facade/FileFacade.cpp facade/TimerFacade.cpp
  INC include adapter facade
)
target_link_libraries(exult scummvm-core scummvm-graphics scummvm-audio)


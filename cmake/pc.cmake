MESSAGE(STATUS "pc.cmake")

LIST(APPEND PLATFORM_LINKLIBS
    opengl32.lib
    winmm.lib   # SDL - sound, etc.
    version.lib # SDL - windows keyboard
    imm32.lib   # SDL - windows keyboard
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")


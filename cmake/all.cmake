if(WIN32)
add_compile_options("$<$<CONFIG:RELWITHDEBINFO>:-DTRACY_ENABLE=1>")
endif()

# ------------------------------------------------------------------------------
# Coverage
# ------------------------------------------------------------------------------
if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# ------------------------------------------------------------------------------
# Google Sanitizers
# ------------------------------------------------------------------------------

if(ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
endif()

if(ENABLE_USAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()

if(ENABLE_TSAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
endif()

# ------------------------------------------------------------------------------
# Valgrind
# ------------------------------------------------------------------------------

set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --leak-check=full")
set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --track-fds=yes")
set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --trace-children=yes")
set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --error-exitcode=1")

message(STATUS "System: ${CMAKE_SYSTEM}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Debug Flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Release Flags: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "RelWithDebInfo Flags: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
message(STATUS "Arch: ${PROCESSOR_ARCH}")

# Compiler specific stuff
if(CMAKE_COMPILER_IS_GNUCXX)
  include(${PROJECT_SOURCE_DIR}/cmake/g++.cmake)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  include(${PROJECT_SOURCE_DIR}/cmake/clang++.cmake)
elseif(MSVC)
  include(${PROJECT_SOURCE_DIR}/cmake/msvc.cmake)
else()
  message(WARNING "Unknown compiler, not setting flags")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ARCH_64 TRUE)
  set(PROCESSOR_ARCH "x64")
else()
  set(ARCH_64 FALSE)
  set(PROCESSOR_ARCH "x86")
endif()

# OS specific stuff
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    message(STATUS "TARGET_PC")
    set(TARGET_PC 1)
    include(${PROJECT_SOURCE_DIR}/cmake/pc.cmake)
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    message(STATUS "TARGET_MAC")
    SET(TARGET_MAC 1)
    INCLUDE(${PROJECT_SOURCE_DIR}/cmake/mac.cmake)
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    message(STATUS "TARGET_LINUX")
    SET(TARGET_LINUX 1)
    INCLUDE(${PROJECT_SOURCE_DIR}/cmake/linux.cmake)
endif()



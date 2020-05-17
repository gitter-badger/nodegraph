MESSAGE(STATUS "Linux.cmake")

find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

LIST(APPEND PLATFORM_LINKLIBS
    dl 
    Threads::Threads
    stdc++fs
    ${OPENGL_LIBRARY}
    )

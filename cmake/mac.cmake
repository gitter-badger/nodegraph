MESSAGE(STATUS "Mac.cmake")

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

LIST(APPEND PLATFORM_LINKLIBS
    ${OPENGL_LIBRARY}
    dl
    "-framework CoreFoundation"
    )


cmake_minimum_required(VERSION 3.16)

project(plot VERSION 0.1 LANGUAGES CXX)

# 把路径名和后缀名的排列组合全部写出来
# file(GLOB sources CONFIGURE_DEPENDS Xxw/*.cpp Xxw/*.h)
# 在指定路径自动搜集需要的所有源文件
# aux_source_directory(. sources)
# 通过 GLOB_RECURSE 自动包含所有子文件夹下的文件
#file(GLOB_RECURSE sources CONFIGURE_DEPENDS *.cpp *.h)

set(PLOT_SOURCES
    #${sources}
    Xxw/qcustomplot.cpp
    Xxw/qcustomplot.h
    Xxw/XxwCustomPlot.cpp
    Xxw/XxwCustomPlot.h
    Xxw/XxwTracer.cpp
    Xxw/XxwTracer.h
    Xxw/qcustomplotex.cpp
    Xxw/mousetraceline.h
    Xxw/mousetraceline.cpp
    Xxw/qcustomplotex.h

)

# 静态库
add_library(${PROJECT_NAME} ${PLOT_SOURCES})

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS core PrintSupport)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::PrintSupport)

# 最新支持到6.4
set(USEOPENGL false)
if(${QT_VERSION_MAJOR} LESS 6)
    set(USEOPENGL true)
elseif(QT_VERSION_MINOR  LESS  5)
    set(USEOPENGL true)
endif()
if(${USEOPENGL})
    # 使用opengl
    target_compile_definitions(${PROJECT_NAME} PRIVATE QCUSTOMPLOT_USE_OPENGL)
    #add_definitions(-DQCUSTOMPLOT_USE_OPENGL)

    #find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS OpenGL OpenGLWidgets)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
        OpenGL
        #OpenGLWidgets
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::OpenGL
        #Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    )
    #-lopengl32 -lglu32
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32 glu32)
    message(STATUS "while use opengl")
endif()

# find_dependency(WrapOpenGL REQUIRED)
# if(NOT WrapOpenGL_FOUND)
#   set(Qt6Gui_FOUND FALSE)
#   message(FATAL_ERROR "Qt6Gui requires WrapOpenGL")
# endif()

# CMake内置的OpenGL检测机制
# if(OpenGL_GL_PREFERENCE STREQUAL "GLVND")
#   find_library(OPENGL_opengl_LIBRARY OpenGL)
# else()
#   find_library(OPENGL_opengl_LIBRARY GL)
#   set(QT_OPENGL_PREFER_LEGACY ON) # 强制Qt使用传统接口
#   set(OpenGL_GL_PREFERENCE "LEGACY") # 强制CMake使用libGL.so
#   message(STATUS ${OPENGL_opengl_LIBRARY})
# endif()

target_include_directories(${PROJECT_NAME}
    # PRIVATE：仅当前目标使用
    # PUBLIC：当前目标及依赖它的目标都可用
    # INTERFACE：当前目标不使用，但依赖它的目标可用
    INTERFACE
        ${PROJECT_SOURCE_DIR}/Xxw
)

#include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    # ARCHIVE :静态库路径
    # LIBRARY：共享库路径
    # RUNTIME：运行时路径
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/static
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
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
)

# 静态库
add_library(${PROJECT_NAME} ${PLOT_SOURCES})

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS core PrintSupport)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::PrintSupport)

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
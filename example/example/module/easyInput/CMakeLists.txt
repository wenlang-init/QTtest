cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

project(easyInput LANGUAGES C CXX)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Widgets
    Core
)

# if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
#     find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
#         Core5Compat
#     )
# endif()


file(GLOB EASYINPUT_SOURCE CONFIGURE_DEPENDS
    *.cpp
    *.h
    code/zhtopy.cpp
    code/zhtopy.h
    googleinput/*.cpp
    googleinput/*.h
    googleinput/googlepinyin/*.cpp
    googleinput/googlepinyin/*.h
    t9pinyin/*.c
    t9pinyin/*.h
    widget/*.cpp
    widget/*.h
    widget/basewidget/*.cpp
    widget/basewidget/*.h
    widget/key_input/*.cpp
    widget/key_input/*.h
    widget/virtual_input/*.cpp
    widget/virtual_input/*.h
)

add_library(${PROJECT_NAME} ${EASYINPUT_SOURCE})
# 动态库
# add_library(${PROJECT_NAME}
#     SHARED
#     ${EASYINPUT_SOURCE}
# )

# 拷贝字库到输出目录
set(PTHREADPATH ${PROJECT_SOURCE_DIR}/easyInput)
add_custom_command(TARGET ${PROJECT_NAME}
   POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E
        copy_directory_if_different
        ${PTHREADPATH}
        "${CMAKE_BINARY_DIR}/easyInput"
)
message(STATUS ${PROJECT_SOURCE_DIR})
message(STATUS ${CMAKE_COMMAND})
message(STATUS ${PTHREADPATH})
message(STATUS ${CMAKE_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
)

# if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
#     target_link_libraries(${PROJECT_NAME} PRIVATE
#         Qt6::Core5Compat
#     )
# endif()

# INTERFACE：表示该库是一个接口库
target_include_directories(${PROJECT_NAME}
    INTERFACE
        ${PROJECT_SOURCE_DIR}
)

#include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/static
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

cmake_minimum_required(VERSION 3.16)

# # 获取当前路径的最后一级
# # 函数名:to_current_dir
# # outmsg:得到的相对路径
# # inmsg:输入的绝对路径
# function(to_current_dir outmsg inmsg)
#     # 去掉末尾的'/'
#     string(REGEX REPLACE "/$" "" CURRENT_FOLDER_ABSOLUTE ${inmsg})

#     # 获取当前目录的相对路径名
#     string(REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CURRENT_FOLDER_ABSOLUTE})
#     set(${outmsg} ${CURRENT_FOLDER} PARENT_SCOPE)
#     #message(STATUS "111111 ${CURRENT_FOLDER}")
# endfunction()

# to_current_dir(AAAA ${CMAKE_CURRENT_SOURCE_DIR})
# message(STATUS "(${CMAKE_CURRENT_SOURCE_DIR}|${CMAKE_CURRENT_LIST_LINE}): xxxxxxxxxx ${AAAA}")

#project(Mqtt VERSION 0.1)
# CMAKE_PROJECT_NAME : 顶级项目名
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}|${CMAKE_CURRENT_LIST_LINE}: Configuring ${PROJECT_NAME} Mqtt")

set(LIBS_PATH_BASE ${PROJECT_SOURCE_DIR}/libs)
set(DLL_PATH ${PROJECT_SOURCE_DIR}/libs/llvm_mingw_x64/Qt6Mqtt.dll)
set(LIBS_PATH ${LIBS_PATH_BASE}/llvm_mingw_x64/libQt6Mqtt.a)

if(MINGW)
    set(LIBS_PATH ${LIBS_PATH_BASE}/llvm_mingw_x64/libQt6Mqtt.a)
elseif(MSVC)
    set(LIBS_PATH ${LIBS_PATH_BASE}/msvc2022_x64/Qt6Mqtt.lib)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(LIBS_PATH ${LIBS_PATH_BASE}/arm64_v8a/libQt6Mqtt_arm64-v8a.so)
endif()

message(STATUS "MQTT library path: ${LIBS_PATH}")

#target_link_libraries
#link_libraries(${PROJECT_NAME} ${LIBS_PATH_BASE}/llvm_mingw_x64/libQt6Mqtt.a)
#include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

# add_library(Mqtt INTERFACE IMPORTED GLOBAL)
# set_target_properties(Mqtt PROPERTIES
#     INTERFACE_LINK_LIBRARIES "${LIBS_PATH_BASE}/llvm_mingw_x64/libQt6Mqtt.a"
#     #INTERFACE_COMPILE_OPTIONS "${ffmpeg_DEFINITIONS}"
#     INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/include"
# )
# install(FILES
#     ${LIBS_PATH_BASE}/llvm_mingw_x64/libQt6Mqtt.a
#     DESTINATION "${CMAKE_INSTALL_LIBDIR}"
# )

#####################################################################################
message (STATUS "CMAKE_C_FLAGS_RELEASE:${CMAKE_C_FLAGS_RELEASE}")
message (STATUS "CMAKE_CXX_FLAGS_RELEASE:${CMAKE_CXX_FLAGS_RELEASE}")
message (STATUS "CMAKE_SHARED_LINKER_FLAGS_RELEASE:${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")

message (STATUS "CMAKE_C_FLAGS_DEBUG:${CMAKE_C_FLAGS_DEBUG}")
message (STATUS "CMAKE_CXX_FLAGS_DEBUG:${CMAKE_CXX_FLAGS_DEBUG}")
message (STATUS "CMAKE_C_FLAGS_MINSIZEREL:${CMAKE_C_FLAGS_MINSIZEREL}")
message (STATUS "CMAKE_CXX_FLAGS_MINSIZEREL:${CMAKE_CXX_FLAGS_MINSIZEREL}")
message (STATUS "CMAKE_C_FLAGS_RELWITHDEBINFO:${CMAKE_C_FLAGS_RELWITHDEBINFO}")
message (STATUS "CMAKE_CXX_FLAGS_RELWITHDEBINFO:${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

#下面几个变量在交叉编译时：可以根据不同的目标系统进行条件判断或设置特定的编译选项和路径
#目标系统信息
message (STATUS "CMAKE_SYSTEM:${CMAKE_SYSTEM}")
#目标系统名字
message (STATUS "CMAKE_SYSTEM_NAME:${CMAKE_SYSTEM_NAME}")
#目标系统的处理器类型
message (STATUS "CMAKE_SYSTEM_PROCESSOR:${CMAKE_SYSTEM_PROCESSOR}")
#目标系统使用的C++编译器 如：GNU Clang MSVC
message (STATUS "CMAKE_CXX_COMPILER_ID:${CMAKE_CXX_COMPILER_ID}")
#当前编译环境下指针类型（通常是void*）的大小（以字节为单位）
message (STATUS "CMAKE_SIZEOF_VOID_P:${CMAKE_SIZEOF_VOID_P}")
#当前实际所在系统的处理器类型
message (STATUS "CMAKE_HOST_SYSTEM_PROCESSOR:${CMAKE_HOST_SYSTEM_PROCESSOR}")
message (STATUS "ANDROID:${ANDROID}")

#CMAKE_SYSTEM_PROCESSOR 目标系统处理器类型。交叉编译时，编译好的库只能在CMAKE_SYSTEM_PROCESSOR平台上运行
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86")
    message("x86 32")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    message("x86_64 64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    message("arm 64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7-a")
    message("arm 32")
endif()

message (STATUS "CMAKE_INSTALL_PREFIX:${CMAKE_INSTALL_PREFIX}")

#增加设置项  在执行cmake时，可以通过 -DDEBUG_LOG=ON来开启此flag
#option(BUILD_TEST "Build test" OFF)
#message (STATUS "BUILD_TEST:${BUILD_TEST}")



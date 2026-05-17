cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

project(log LANGUAGES C CXX)
# set(LOG_SOURCE
#     lognone.c lognone.h mArray.c mArray.h mList.c mList.h printFunction.c printFunction.h memorypool.c memorypool.h
# )
#aux_source_directory(./ LOG_SOURCE)
#target_include_directories(${PROJECT_NAME} PRIVATE ./)
file(GLOB LOG_SOURCE CONFIGURE_DEPENDS *.c *.h *.cpp printf/*.c printf/*.h)

add_library(log ${LOG_SOURCE})

#message(STATUS "log source: ${PROJECT_SOURCE_DIR}/Pre-built.2/include")
# pthread库
if(MSVC)
    set(USE_WINDOWS_THREAD 1)
    if(${USE_WINDOWS_THREAD})
        message(STATUS "MSVC use windows api thread")
        target_compile_definitions(${PROJECT_NAME} PRIVATE USE_WINDOWS_THREAD=1)
    else()
        message(STATUS "MSVC use pthread")
        set(PTHREADPATH ${PROJECT_SOURCE_DIR}/Pre-built.2)
        set(PTHREADLIBPATH ${PTHREADPATH}/lib/x64/pthreadVC2.lib)
        # PTHREAD_USE_LIB : 定义一个预处理器宏，值为 pthreadVC2.lib 的路径。这允许在代码中使用 PTHREAD_USE_LIB 来引用这个库路径。
        target_compile_definitions(${PROJECT_NAME} PRIVATE PTHREAD_USE_LIB="${PTHREADLIBPATH}")
        # PROJECT_SOURCE_DIR : 当前CMakeLists.txt所在目录的绝对路径
        # CMAKE_SOURCE_DIR : 顶层CMakeLists.txt所在目录的绝对路径
        target_link_libraries(${PROJECT_NAME} ${PTHREADLIBPATH})
        target_include_directories(${PROJECT_NAME} PRIVATE ${PTHREADPATH}/include)
        #target_link_options(${PROJECT_NAME} PRIVATE /NODEFAULTLIB:MSVCRT)

        # CMAKE_BINARY_DIR : 顶层CMakeLists.txt所在目录的二进制输出路径
        # CMAKE_CURRENT_BINARY_DIR : 当前CMakeLists.txt所在目录的二进制输出路径
        add_custom_command(TARGET ${PROJECT_NAME}
           POST_BUILD
           COMMAND ${CMAKE_COMMAND} -E
                copy_if_different
                ${PTHREADPATH}/dll/x64/pthreadVC2.dll
                "${CMAKE_BINARY_DIR}"
        )
    endif()
else()
    # 强制为编译和链接增加选项-pthread，这样会确保在编译和链接阶段都使用正确的线程库选项。
    # set_target_properties(${PROJECT_NAME} PROPERTIES
    #     COMPILE_FLAGS "-pthread"
    #     LINK_FLAGS "-pthread"
    # )
    # 会扩展为-lpthread，并且仅对链接阶段生效。这样的方法不适合跨平台的使用
    #target_link_libraries(${PROJECT_NAME} pthread)
    # pthread库在不同平台上的链接方式不同，CMake提供了一个跨平台的方式来链接线程库，
    # 即使用find_package(Threads)。
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} Threads::Threads)
endif()

# INTERFACE：表示该库是一个接口库，不会生成实际的库文件，但可以被其他目标链接，并且会传播其包含目录和编译选项。
target_include_directories(${PROJECT_NAME}
    INTERFACE
        ${PROJECT_SOURCE_DIR}
)

include(GNUInstallDirs)
install(TARGETS log
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

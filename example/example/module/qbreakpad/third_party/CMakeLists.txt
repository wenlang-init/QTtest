cmake_minimum_required(VERSION 3.16)
project(breakpads LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

set(BREAKPAD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/breakpad/src)

if (UNIX)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
    file(GLOB BREAKPADS_SRC_LIST
        ${BREAKPAD_PATH}/client/minidump_file_writer.cc
        ${BREAKPAD_PATH}/common/convert_UTF.cc
        ${BREAKPAD_PATH}/common/md5.cc
        ${BREAKPAD_PATH}/common/string_conversion.cc
    )
    if(APPLE)
        #MACOS
        # 当启用 OBJC 或 OBJCXX 时，扩展名为 .m 或 .mm 的源文件将分别被编译为 Objective C 或 C++。
        # 否则，它们将像以前一样被视为普通 C++ 源文件
        enable_language(OBJC OBJCXX)
        file(GLOB BREAKPADS_SRC_LIST
            ${BREAKPAD_PATH}/client/mac/crash_generation/crash_generation_client.cc
            ${BREAKPAD_PATH}/client/mac/handler/breakpad_nlist_64.cc
            ${BREAKPAD_PATH}/client/mac/handler/dynamic_images.cc
            ${BREAKPAD_PATH}/client/mac/handler/exception_handler.cc
            ${BREAKPAD_PATH}/client/mac/handler/minidump_generator.cc
            ${BREAKPAD_PATH}/common/mac/MachIPC.mm
            ${BREAKPAD_PATH}/common/mac/bootstrap_compat.cc
            ${BREAKPAD_PATH}/common/mac/file_id.cc
            ${BREAKPAD_PATH}/common/mac/macho_id.cc
            ${BREAKPAD_PATH}/common/mac/macho_utilities.cc
            ${BREAKPAD_PATH}/common/mac/macho_walker.cc
            ${BREAKPAD_PATH}/common/mac/string_utilities.cc
        )
    else()
        file(GLOB BREAKPADS_SRC_LIST
            ${BREAKPAD_PATH}/client/linux/crash_generation/crash_generation_client.cc
            ${BREAKPAD_PATH}/client/linux/dump_writer_common/thread_info.cc
            ${BREAKPAD_PATH}/client/linux/dump_writer_common/ucontext_reader.cc
            ${BREAKPAD_PATH}/client/linux/handler/exception_handler.cc
            ${BREAKPAD_PATH}/client/linux/handler/minidump_descriptor.cc
            ${BREAKPAD_PATH}/client/linux/log/log.cc
            ${BREAKPAD_PATH}/client/linux/microdump_writer/microdump_writer.cc
            ${BREAKPAD_PATH}/client/linux/minidump_writer/linux_core_dumper.cc
            ${BREAKPAD_PATH}/client/linux/minidump_writer/linux_dumper.cc
            ${BREAKPAD_PATH}/client/linux/minidump_writer/linux_ptrace_dumper.cc
            ${BREAKPAD_PATH}/client/linux/minidump_writer/minidump_writer.cc
            ${BREAKPAD_PATH}/common/linux/breakpad_getcontext.S
            ${BREAKPAD_PATH}/common/linux/elfutils.cc
            ${BREAKPAD_PATH}/common/linux/file_id.cc
            ${BREAKPAD_PATH}/common/linux/guid_creator.cc
            ${BREAKPAD_PATH}/common/linux/linux_libc_support.cc
            ${BREAKPAD_PATH}/common/linux/memory_mapped_file.cc
            ${BREAKPAD_PATH}/common/linux/safe_readlink.cc
            ${BREAKPAD_PATH}/common/linux/elf_core_dump.cc
        )
    endif()
elseif(WIN32)
    file(GLOB BREAKPADS_SRC_LIST
        ${BREAKPAD_PATH}/client/windows/crash_generation/crash_generation_client.cc
        ${BREAKPAD_PATH}/client/windows/handler/exception_handler.cc
        ${BREAKPAD_PATH}/common/windows/guid_string.cc)
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()
# https://github.com/buzzySmile/qBreakpad.git

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
include_directories(breakpad/src)
add_library(breakpads STATIC ${BREAKPADS_SRC_LIST})
target_link_libraries(breakpads PRIVATE)
target_compile_definitions(breakpads PRIVATE QKDNETS_LIBRARY)
#install(TARGETS breakpads)

target_include_directories(${PROJECT_NAME}
    INTERFACE
        ${PROJECT_SOURCE_DIR}
)
#include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    # ARCHIVE :静态库文件；LIBRARY：共享库文件；RUNTIME：可执行文件
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/static
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

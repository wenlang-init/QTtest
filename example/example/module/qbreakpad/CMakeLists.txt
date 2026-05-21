cmake_minimum_required(VERSION 3.16)
#Very importantVery important
add_definitions(-DUNICODE -D_UNICODE)
project(qbreakpads LANGUAGES CXX)

file(GLOB HANDLER_SRC_LIST handler/*.cpp handler/*.h handler/singletone/*.h)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)
find_package(Threads REQUIRED)
include_directories(./third_party/)
include_directories(./third_party/breakpad/src)
add_subdirectory(./third_party)

add_library(${PROJECT_NAME} STATIC ${HANDLER_SRC_LIST})
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads breakpads)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network breakpads)
target_compile_definitions(${PROJECT_NAME} PRIVATE QKDNETS_LIBRARY)

# INTERFACE：表示该库是一个接口库，不会生成实际的库文件，但可以被其他目标链接，并且会传播其包含目录和编译选项。
target_include_directories(${PROJECT_NAME}
    INTERFACE
        ${PROJECT_SOURCE_DIR}/handler
        ${PROJECT_SOURCE_DIR}/handler/singletone
)
#include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    # ARCHIVE :静态库文件；LIBRARY：共享库文件；RUNTIME：可执行文件
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/static
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

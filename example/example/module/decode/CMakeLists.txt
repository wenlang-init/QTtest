cmake_minimum_required(VERSION 3.16)

project(decodepublic LANGUAGES C)

# 把路径名和后缀名的排列组合全部写出来
# file(GLOB sources CONFIGURE_DEPENDS Xxw/*.cpp Xxw/*.h)
# 在指定路径自动搜集需要的所有源文件
# aux_source_directory(. sources)
# 通过 GLOB_RECURSE 自动包含所有子文件夹下的文件
#file(GLOB_RECURSE sources CONFIGURE_DEPENDS *.cpp *.h)

file(GLOB decodesources CONFIGURE_DEPENDS *.c *.h)

add_library(${PROJECT_NAME} ${decodesources})

#find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS core PrintSupport)
#target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::PrintSupport)

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
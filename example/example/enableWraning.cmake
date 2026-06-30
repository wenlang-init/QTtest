
# 功能 设置编译警告选项
# 函数名 target_set_warnings
# TARGET 需要设置的目标对象，一般为编译目标(工程)名
# 是否开启警告
# 是否将所有警告视为错误
function(target_set_warnings TARGET ENABLED ENABLED_AS_ERRORS)
    if (NOT ${ENABLED})
        message(STATUS "Warnings Disabled for: ${TARGET}")
        return()
    endif()

    set(MSVC_WARNINGS
        /W4
        /permissive-)

    set(CLANG_WARNINGS
        #-Wall
        #-w
        -W
        -Wextra
        -Wpedantic)

    set(GCC_WARNINGS
        ${CLANG_WARNINGS})

    if(${ENABLED_AS_ERRORS})
        set(MSVC_WARNINGS ${MSVC_WARNINGS} /WX)
        set(CLANG_WARNINGS ${CLANG_WARNINGS} -Werror)
        set(GCC_WARNINGS ${GCC_WARNINGS} -Werror)
    endif()

    if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(WARNINGS ${MSVC_WARNINGS})
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(WARNINGS ${CLANG_WARNINGS})
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(WARNINGS ${GCC_WARNINGS})
    endif()

    target_compile_options(${TARGET} PRIVATE ${WARNINGS})
    message(STATUS ${WARNINGS})
endfunction(target_set_warnings)

# 去掉字符串的前部分，然后输出到数组中。用于将文件夹下获取的文件去掉路径，只保留文件名
function(target_get_filenames TARGETNAMES INPUTNAMES PATHNAME)
    set(TEMP "")
    foreach(path ${INPUTNAMES})
        string(REPLACE
            "${PATHNAME}" "" rel_path ${path}
        )
        set(TEMP "${TEMP} ${rel_path}")
    endforeach()
    set(${TARGETNAMES} ${TEMP} PARENT_SCOPE)
endfunction(target_get_filenames)


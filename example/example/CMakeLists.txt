cmake_minimum_required(VERSION 3.16)

project(example VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Widgets LinguistTools Core Sql Quick
    Multimedia
    MultimediaWidgets
    QuickWidgets PrintSupport
    #Mqtt
    WebView
    Network
    Charts
    #OpenGL
    OpenGLWidgets
    #Qml
    QuickControls2
    #CorePrivate
    SerialPort
)

# 设置项目范围默认为标准安排。该命令在Qt6包的Core组件中定义，可以像这样加载：
# find_package(Qt6 REQUIRED COMPONENTS Core)
qt_standard_project_setup(REQUIRES ${QT_VERSION})

#aux_source_directory(src/messageWidget mwsources)
file(GLOB mwsources CONFIGURE_DEPENDS src/messageWidget/*.cpp src/messageWidget/*.h)
#file(GLOB videosources CONFIGURE_DEPENDS src/video/*.cpp src/video/*.h)
set(videosources
    src/video/popwidget.h
    src/video/videowidget.cpp
    src/video/videowidget.h
    src/video/mediaplayerwidget.cpp
    src/video/mediaplayerwidget.h
    src/video/popwidget.cpp
)
file(GLOB graphicssources CONFIGURE_DEPENDS src/graphics/*.cpp src/graphics/*.h)
file(GLOB qmlListsources CONFIGURE_DEPENDS src/qmlList/*.cpp src/qmlList/*.h)
set(audiosources
    src/audio/fft/fft.cpp
    src/audio/fft/fft.h
    src/audio/wav/audio_wave.cpp
    src/audio/wav/audio_wave.h
    src/audio/audioproc.cpp
    src/audio/audioproc.h
    src/audio/widget.cpp
    src/audio/widget.h
    src/audio/widget.ui
)

set(layoutsources
    src/layout/wlayout.h
    src/layout/wlayout.cpp
)

set(fftsources
    src/fftw/fftw3object.h
    src/fftw/fftw3object.cpp
    src/fftw/udpobject.h
    src/fftw/udpobject.cpp
    src/fftw/audioobject.cpp
    src/fftw/audioobject.h
    src/fftw/widegtfft.h
    src/fftw/widegtfft.cpp
    src/fftw/widegtfft.ui
    src/fftw/ffmpegobject.h
    src/fftw/ffmpegobject.cpp
    src/fftw/fftworkthread.h
    src/fftw/fftworkthread.cpp
    src/fftw/linechartwidget.h
    src/fftw/linechartwidget.cpp
    src/fftw/widgetitem.h
    src/fftw/widgetitem.cpp
    src/fftw/waveformgenerator.h
    src/fftw/gensignalwidget.h
    src/fftw/gensignalwidget.cpp
    src/fftw/gensignalwidget.ui
)

set(souyinsources
    src/souyin/shouyinw.cpp
    src/souyin/shouyinw.h
    src/souyin/shouyinw.ui
    src/souyin/quickw.cpp
    src/souyin/quickw.h
    src/souyin/usecode.h
    src/souyin/usecode.cpp
)

set(desktopsources
    src/desktop/bottom_icon.cpp
    src/desktop/bottom_icon.h
    src/desktop/desktopwidget.cpp
    src/desktop/desktopwidget.h
    src/desktop/custombtn.h
    src/desktop/custombtn.cpp
    src/desktop/basewidget.cpp
    src/desktop/basewidget.h
)

set(sqosources
    src/sql/sql_engine.cpp
    src/sql/sql_engine.h
)

set(publicsources
    src/public/funchelper.cpp
    src/public/funchelper.h

    src/public/waitdialog.h
    src/public/waitdialog.cpp

    src/public/lserialport.cpp
    src/public/lserialport.h
)

set(MODULE_SOURCES
    ${mwsources}
    ${videosources}
    ${graphicssources}
    ${qmlListsources}
    ${audiosources}
    ${layoutsources}
    ${souyinsources}
    ${desktopsources}
    ${sqosources}
    ${publicsources}
)
if(WIN32 OR LINUX)
    set(MODULE_SOURCES
        ${MODULE_SOURCES}
        ${fftsources}
    )
endif()

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    homewidget.h
    homewidget.cpp

    ${TS_FILES}
    ${MODULE_SOURCES}
)

qt_add_resources(APP_RESOURCES
    resources.qrc
    ${CMAKE_SOURCE_DIR}/resources/QtTheme/QtTheme.qrc
    ${CMAKE_SOURCE_DIR}/resources/MacOS/MacOS.qrc
)

# QMessageLogContext 获取日志上下文信息，包含文件名、行号、函数名等信息。
# 定义 QT_MESSAGELOGCONTEXT 可以启用这些功能。
add_compile_definitions(DEFINES QT_MESSAGELOGCONTEXT)
set(TS_FILES example_zh_CN.ts)
qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
#target_sources(${PROJECT_NAME} PRIVATE ${APP_RESOURCES})
#target_include_directories(${PROJECT_NAME} module/)

# if(ANDROID)
#     add_library(${PROJECT_NAME} SHARED
#         ${PROJECT_SOURCES}
#     )

#if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
qt_add_executable(example
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${APP_RESOURCES}
)

target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/shellbulid/build.sh)
target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/shellbulid/pack.sh)
# 标记该文件为“仅头文件/非编译”
set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/shellbulid/build.sh
    PROPERTIES HEADER_FILE_ONLY TRUE
)
set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/shellbulid/pack.sh
    PROPERTIES HEADER_FILE_ONLY TRUE
)

# 动态库
# add_library(${PROJECT_NAME}_shared
#     SHARED
#     ${PROJECT_SOURCES}
#     ${APP_RESOURCES}
# )

# 静态库
# add_library(${PROJECT_NAME}_static
#     ${PROJECT_SOURCES}
#     ${APP_RESOURCES}
# )

# ==========================
# QML 模块
# ==========================
qt_add_qml_module(${PROJECT_NAME}
    URI QmlTest
    VERSION 1.0
    QML_FILES
        src/layout/Main.qml
        src/layout/components/EButton.qml
        src/layout/components/EInput.qml
        src/layout/components/EList.qml
        src/layout/components/EHoverCard.qml
        src/layout/components/ETheme.qml
        src/layout/components/ESwitchButton.qml
        src/layout/components/ECheckBox.qml
        src/layout/components/ERadioButton.qml
        src/layout/components/ESlider.qml
        src/layout/components/EBlurCard.qml
        src/layout/components/ENavBar.qml
        src/layout/components/ETimeDisplay.qml
        src/layout/components/EAvatar.qml
        src/layout/components/EAccordion.qml
        src/layout/components/ECard.qml
        src/layout/components/ECalendar.qml
        src/layout/components/ECarousel.qml
        src/layout/components/EClock.qml
        src/layout/components/ECardWithTextArea.qml
        src/layout/components/EDataTable.qml
        src/layout/components/EAnimatedWindow.qml
        src/layout/components/Aboutme.qml
        src/layout/components/EDrawer.qml
        src/layout/components/EDropdown.qml
        src/layout/components/WLayout.qml
        src/layout/components/WVideoPlayer.qml
        src/layout/components/WCamera.qml
        src/layout/components/WComboBox.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.example)
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# 设置cmake模块的路径,存放上面函数的脚本名称为 enableWraning.txt，存放在cmake目录下：
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/")
# 引入cmake的模块
include(enableWraning.cmake)
#target_set_warnings(${PROJECT_NAME} true false)

#set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    Qt${QT_VERSION_MAJOR}::QuickWidgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
    #Qt${QT_VERSION_MAJOR}::Mqtt
    Qt${QT_VERSION_MAJOR}::WebView
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Charts
    #Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    #Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::QuickControls2
    #Qt${QT_VERSION_MAJOR}::CorePrivate
    Qt${QT_VERSION_MAJOR}::SerialPort
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        log
        plot
        decodepublic
        easyInput
)
# 是否不显示控制台窗口 WIN32_EXECUTABLE属性指定了是否为Windows应用程序生成一个控制台窗口。
# 默认情况下，CMake会根据项目类型自动设置这个属性，但在某些情况下，你可能需要手动调整它以满足你的需求。
if(WIN32)
    # if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    #     set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE OFF)
    # else()
    #      set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON)
    # endif()
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE OFF)
endif()

# fftw 库
set(FFTWPATH ${PROJECT_SOURCE_DIR}/thirdparty/fftw)
if(WIN32)
    target_include_directories(${PROJECT_NAME} PRIVATE ${FFTWPATH}/W64-3.3.5)
    if(MSVC)
        # 只有def文件和dll文件，需要生成lib文件
        set(DEF_FILE ${FFTWPATH}/W64-3.3.5/libfftw3-3.def)
        set(LIB_OUTPUT ${FFTWPATH}/W64-3.3.5/libfftw3-3.lib)
        add_custom_command(
            OUTPUT ${LIB_OUTPUT}
            COMMAND lib.exe /DEF:${DEF_FILE} /OUT:${LIB_OUTPUT} /MACHINE:X64
            DEPENDS ${DEF_FILE}
            VERBATIM
        )
        add_custom_target(GenerateThirdPartyLib DEPENDS ${LIB_OUTPUT})
        add_library(ThirdParty::Lib STATIC IMPORTED)
        add_dependencies(ThirdParty::Lib GenerateThirdPartyLib)
        set_target_properties(ThirdParty::Lib PROPERTIES
            IMPORTED_LOCATION ${LIB_OUTPUT}
        )

        target_link_libraries(${PROJECT_NAME} PRIVATE
            ThirdParty::Lib
        )

        #set(CMAKE_LINK_DEF_FILE_FLAG ${CMAKE_LINK_DEF_FILE_FLAG}${FFTWPATH}/W64-3.3.5/libfftw3-3.def)
        #message(STATUS "Link Def File Flag: ${CMAKE_LINK_DEF_FILE_FLAG}")
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE
            ${FFTWPATH}/W64-3.3.5/libfftw3-3.dll
            #${FFTWPATH}/W64-3.3.5/libfftw3f-3.dll
            #${FFTWPATH}/W64-3.3.5/libfftw3l-3.dll
        )
    endif()
    # add_custom_command(TARGET ${PROJECT_NAME}
    #    POST_BUILD
    #    COMMAND ${CMAKE_COMMAND} -E
    #         copy_if_different
    #         ${FFTWPATH}/W64-3.3.5/libfftw3-3.dll
    #         "${CMAKE_BINARY_DIR}"
    # )
elseif(UNIX)
    if(LINUX)
        target_link_directories(${PROJECT_NAME} PRIVATE ${FFTWPATH}/linux-x64-3.3.11/lib)
        target_link_libraries(${PROJECT_NAME} PRIVATE
            libfftw3.a
            # libfftw3_mpi.a
            # libfftw3_omp.a
            # libfftw3_threads.a
        )
        target_include_directories(${PROJECT_NAME} PRIVATE
            ${FFTWPATH}/linux-x64-3.3.11/include
        )
    endif()
endif()

# ffmpeg 库
# set(FFTMPEGPATH ${PROJECT_SOURCE_DIR}/thirdparty/ffmpeg)
# if(WIN32)
#     target_include_directories(${PROJECT_NAME} PRIVATE ${FFTMPEGPATH}/windows/include)
#     target_link_directories(${PROJECT_NAME} PRIVATE ${FFTMPEGPATH}/windows/lib)

#     target_link_libraries(${PROJECT_NAME} PRIVATE
#         avcodec avformat avutil
#         #swresample swscale avdevice avfilter
#     )

#     # add_custom_command(TARGET ${PROJECT_NAME}
#     #    POST_BUILD
#     #    COMMAND ${CMAKE_COMMAND} -E
#     #         copy_if_different
#     #         ${FFTMPEGPATH}/windows/bin/*.dll
#     #         "${CMAKE_BINARY_DIR}"
#     # )
# elseif(UNIX)
#     if(LINUX)
#         target_include_directories(${PROJECT_NAME} PRIVATE ${FFTMPEGPATH}/linux/include)
#         target_link_directories(${PROJECT_NAME} PRIVATE ${FFTMPEGPATH}/linux/lib)

#         target_link_libraries(${PROJECT_NAME} PRIVATE
#             avcodec avformat avutil
#             #swresample swscale avdevice avfilter
#         )
#         #find_package(PkgConfig REQUIRED) # 如果找不到会直接报错，不需要手动判断
#         # find_package(PkgConfig QUIET) # 找不到不会终止
#         # if(PkgConfig_FOUND)
#         #     pkg_check_modules(AVFORMAT REQUIRED libavformat)
#         #     pkg_check_modules(AVCODEC REQUIRED libavcodec)
#         #     pkg_check_modules(AVUTIL REQUIRED libavutil)
#         #     target_include_directories(${PROJECT_NAME} PRIVATE ${AVFORMAT_INCLUDE_DIRS})
#         #     target_include_directories(${PROJECT_NAME} PRIVATE ${AVCODEC_INCLUDE_DIRS})
#         #     target_include_directories(${PROJECT_NAME} PRIVATE ${AVUTIL_INCLUDE_DIRS})
#         #     target_link_libraries(${PROJECT_NAME} PRIVATE
#         #         ${AVFORMAT_LIBRARIES} ${AVCODEC_LIBRARIES} ${AVUTIL_LIBRARIES}
#         #     )
#         # else()
#         #     message(FATAL_ERROR "PkgConfig not found, cannot continue")
#         #     # 设置 PKG_CONFIG_PATH 以支持非标准路径 (可选)
#         #     set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${CUSTOM_FFMPEG_PATH}/lib/pkgconfig")
#         #     pkg_check_modules(PC_FFMPEG QUIET libavcodec libavformat libavutil libswresample)
#         #     if(NOT PC_FFMPEG_FOUND)
#         #         # 引入已经存在的动态库avcodec
#         #         #add_library(avcodec SHARED IMPORTED)
#         #         #指定所添加依赖库的导入路径
#         #         #set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION ./libavcodec.so )

#         #         target_include_directories(${PROJECT_NAME} PRIVATE ${FFTMPEGPATH}/linux/include)
#         #         target_link_directories(${PROJECT_NAME} PRIVATE ${FFTMPEGPATH}/linux/lib)

#         #         target_link_libraries(${PROJECT_NAME} PRIVATE
#         #             avcodec avformat avutil
#         #             #swresample swscale avdevice avfilter
#         #         )
#         #     endif()
#         # endif()
#     endif()
# endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME}
        PUBLIC
            qbreakpads
            #breakpads
    )
endif()

# set_target_properties(${PROJECT_NAME} PROPERTIES
#     # 检测栈内存溢出 -fsanitize=address
#     # 关闭栈保护 -fno-stack-protector
#     #COMPILE_FLAGS "-fsanitize=address"
#     LINK_FLAGS "-fsanitize=address"
# )

if(LINUX)
    # 设置可执行文件的 rpath，使其在运行时能够找到当前目录下的动态库
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "-Wl,-rpath=./"
    )
endif()

#include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/static
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# CMake 中安装路径通常基于「前缀路径」（CMAKE_INSTALL_PREFIX），默认值为：
# Linux/macOS：/usr/local
# Windows：C:/Program Files/<项目名>
# 可通过以下方式修改前缀路径：
# 命令行配置时指定：cmake -DCMAKE_INSTALL_PREFIX=/opt/myapp ..
# CMakeLists 中设置：set(CMAKE_INSTALL_PREFIX "/opt/myapp")

# CMAKE_INSTALL_BINDIR	可执行文件路径	bin
# CMAKE_INSTALL_LIBDIR	库文件路径	lib（64 位可能为 lib64）
# CMAKE_INSTALL_INCLUDEDIR	头文件路径	include
# CMAKE_INSTALL_DATADIR	数据文件路径（如文档、配置）	share
# CMAKE_INSTALL_MANDIR	手册页路径	share/man
# message(STATUS "CMAKE_INSTALL_PREFIX:" ${CMAKE_INSTALL_PREFIX})
# message(STATUS "CONFIGURATIONS:" ${CONFIGURATIONS})
# message(STATUS "DESTINATION:" ${DESTINATION})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

add_subdirectory(module/log)
add_subdirectory(module/plot)
add_subdirectory(module/decode)
add_subdirectory(module/easyInput)
# add_subdirectory(module/Mqtt)
if(WIN32)
    add_subdirectory(module/qbreakpad)
endif()

cmake_minimum_required(VERSION 3.16)

project(example VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Widgets LinguistTools core sql quick
    Multimedia
    multimediawidgets
    QuickWidgets PrintSupport
    #Mqtt
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

set(MODULE_SOURCES
    src/sql_engine.cpp
    src/sql_engine.h
    src/cpu/funchelper.cpp
    src/cpu/funchelper.h
    ${mwsources}
    ${videosources}
    ${graphicssources}
    ${qmlListsources}
    ${audiosources}
    ${layoutsources}
)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ${TS_FILES}
        ${MODULE_SOURCES}
)

qt_add_resources(APP_RESOURCES
    resources.qrc
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
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        log
        plot
        decodepublic
        qbreakpads
        #breakpads
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

add_subdirectory(module/log)
add_subdirectory(module/plot)
add_subdirectory(module/decode)
#add_subdirectory(module/Mqtt)
add_subdirectory(module/qbreakpad)

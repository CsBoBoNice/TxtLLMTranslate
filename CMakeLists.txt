cmake_minimum_required(VERSION 3.16)

project(TxtLLMTranslate VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        FileManager.cpp
        FileManager.h
        FileInfo.h
        FilePrompt.cpp
        FilePrompt.h
        FileTranslator.cpp
        FileTranslator.h
        llamaServerManager.cpp
        llamaServerManager.h
        OpenaiManager.cpp
        OpenaiManager.h
        PromptEditor.cpp
        PromptEditor.h
        PromptInfo.h
        SrtParser.cpp
        SrtParser.h
        SrtTranslator.cpp
        SrtTranslator.h
        SrtPrompt.cpp
        SrtPrompt.h
        TabTranslation.cpp
        TabTranslation.h
        TabSettings.cpp
        TabSettings.h
        TxtParser.cpp
        TxtParser.h
        TxtTranslator.cpp
        TxtTranslator.h
        TxtPrompt.cpp
        TxtPrompt.h
        MdParser.cpp
        MdParser.h
        MdTranslator.cpp
        MdTranslator.h
        MdPrompt.cpp
        MdPrompt.h
        TranslationProgress.cpp
        TranslationProgress.h
        RstParser.cpp
        RstParser.h
        RstTranslator.cpp
        RstTranslator.h
        RstPrompt.cpp
        RstPrompt.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TxtLLMTranslate
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET TxtLLMTranslate APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(TxtLLMTranslate SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(TxtLLMTranslate
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(TxtLLMTranslate PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.TxtLLMTranslate)
endif()
set_target_properties(TxtLLMTranslate PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS TxtLLMTranslate
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TxtLLMTranslate)
endif()

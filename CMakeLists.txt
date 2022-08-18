cmake_minimum_required(VERSION 3.15)
project(MagicBoxReduxCmake VERSION 0.1 LANGUAGES CXX)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
set(CMAKE_PREFIX_PATH "C:/Qt5/5.15.2/msvc2019_64/lib/cmake")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CONAN_DISABLE_CHECK_COMPILER TRUE)
include(libraries/conan_paths.cmake)
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets)
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Network)
find_package(xxHash)
find_package(libzippp)
find_package(magic_enum)
find_package(LibCDS)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)
find_package(OpenSSL)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "C:\\MagicBoxReduxRelease")
include_directories(includes)
set(PROJECT_SOURCES
                src/main.cpp
                includes/MainWindow.h src/MainWindow.cpp src/MainWindow.ui
                includes/Mod.h src/Mod.cpp
                includes/ModExceptions.h
                includes/ModServer.h src/ModServer.cpp
                includes/MyTab.h src/MyTab.cpp
                includes/MyTabExceptions.h
                includes/SettingsWindow.h src/SettingsWindow.cpp src/SettingsWindow.ui
                includes/xml.h src/xml.cpp
                includes/xmlitem.h src/xmlitem.cpp
                includes/xmlnode.h src/xmlnode.cpp
                includes/ModController.h src/ModController.cpp
                includes/Downloader.h src/Downloader.cpp
                includes/ModDownloader.h src/ModDownloader.cpp
                includes/Support.h src/Support.cpp
                includes/AppSettings.h src/AppSettings.cpp
        includes/DownloaderWindow.h src/DownloaderWindow.cpp src/DownloaderWindow.ui
        resources/resources.qrc
        includes/MBReduxApplication.cpp includes/MBReduxApplication.h)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MagicBoxReduxCmake
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET MagicBoxReduxCmake APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MagicBoxReduxCmake SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MagicBoxReduxCmake
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(MagicBoxReduxCmake PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(MagicBoxReduxCmake PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(MagicBoxReduxCmake PRIVATE xxHash::xxhash)
target_link_libraries(MagicBoxReduxCmake PRIVATE libzippp::libzippp)
target_link_libraries(MagicBoxReduxCmake PRIVATE magic_enum::magic_enum)
target_link_libraries(MagicBoxReduxCmake PRIVATE LibCDS::cds-s)
target_link_libraries(MagicBoxReduxCmake PRIVATE OpenSSL::Crypto)
target_link_libraries(MagicBoxReduxCmake PRIVATE OpenSSL::SSL)

set_target_properties(MagicBoxReduxCmake PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MagicBoxReduxCmake)
endif()

cmake_minimum_required(VERSION 3.26)
project(Glucocalc VERSION 2.0 LANGUAGES CXX)

# Locate Qt – per AGENTS.md static kits
if(NOT CMAKE_PREFIX_PATH)
  if(WIN32)
    set(CMAKE_PREFIX_PATH "C:/Qt/Qt.6.11.1-static")
  elseif(APPLE)
    set(CMAKE_PREFIX_PATH "/Users/jeff/Qt/Qt.6.11.1-static")
  else()
    set(CMAKE_PREFIX_PATH "/home/jeff/Qt/Qt.6.11.1-static")
  endif()
endif()
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Widgets Gui REQUIRED)

if(APPLE)
  qt_add_executable(glucocalc
      src/main.cpp
      src/calculator.cpp
      src/mainwindow.cpp
      src/helpdialog.cpp
      src/theme.cpp
      resources/AppIcon.icns
      resources/resources.qrc
  )
elseif(WIN32)
  qt_add_executable(glucocalc
      src/main.cpp
      src/calculator.cpp
      src/mainwindow.cpp
      src/helpdialog.cpp
      src/theme.cpp
      resources/glucocalc.rc
      resources/resources.qrc
  )
else()
  qt_add_executable(glucocalc
      src/main.cpp
      src/calculator.cpp
      src/mainwindow.cpp
      src/helpdialog.cpp
      src/theme.cpp
      resources/resources.qrc
  )
endif()

target_include_directories(glucocalc PRIVATE src)

# Link Qt (static libraries)
if(APPLE)
    target_link_libraries(glucocalc PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
    )
else()
    target_link_libraries(glucocalc PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
endif()

# Build as a macOS bundle
if(APPLE)
  set_target_properties(glucocalc PROPERTIES
      MACOSX_BUNDLE YES
      MACOSX_BUNDLE_GUI_IDENTIFIER "com.jocala.glucocalc"
      MACOSX_BUNDLE_BUNDLE_NAME "Glucocalc"
      MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
      MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
      MACOSX_BUNDLE_ICON_FILE AppIcon
      MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/packaging/Info.plist.in"
      RESOURCE "resources/AppIcon.icns"
  )
endif()
if(WIN32)
  set_target_properties(glucocalc PROPERTIES
      WIN32_EXECUTABLE TRUE
  )
endif()

# Install location
install(TARGETS glucocalc
    BUNDLE DESTINATION .)

# Packaging (macOS/Windows/Linux - CPack DragNDrop on Apple)
set(CPACK_PACKAGE_VENDOR "jocala")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Glucose/HbA1c calculator")
set(CPACK_PACKAGE_NAME "glucocalc")
if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_BINARY_TZ OFF)
endif()
include(CPack)

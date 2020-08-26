cmake_minimum_required(VERSION 3.5)

project(PerfectParkILTool LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Network Widgets REQUIRED)

add_executable(PerfectParkILTool WIN32
  levelwindow.cpp
  levelwindow.h
  levelwindow.ui
  logparser.cpp
  logparser.h
  main.cpp
  maps.h
  pbeditor.cpp
  pbeditor.h
  records.cpp
  records.h
)

target_compile_definitions(PerfectParkILTool
  PRIVATE PPT_VERSION="${PerfectParkILTool_Version}"
)

target_link_libraries(PerfectParkILTool PRIVATE Qt5::Core Qt5::Network Qt5::Widgets)

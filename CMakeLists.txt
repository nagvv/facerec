cmake_minimum_required(VERSION 3.14)
project(facerec)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(facerec
        main.cpp mainwindow.cpp mainwindow.h mainwindow.ui base.cpp base.h
        viewer.cpp viewer.h)

qt5_use_modules(facerec Widgets Network Core)

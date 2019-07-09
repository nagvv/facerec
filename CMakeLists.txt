cmake_minimum_required(VERSION 3.14)
project(facerec)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_library( libfacerec SHARED base.cpp base.h )

target_compile_definitions( libfacerec PRIVATE SHARED_LIBRARY )

qt5_use_modules( libfacerec Network Core Gui )

add_executable( facerec
        main.cpp
	mainwindow.cpp mainwindow.h mainwindow.ui
	viewer.cpp viewer.h
	logindialog.cpp logindialog.h logindialog.ui )

target_link_libraries(facerec libfacerec)

qt5_use_modules( facerec Widgets Network Core )

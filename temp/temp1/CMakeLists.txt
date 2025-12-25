cmake_minimum_required(VERSION 3.16)
project(temp1)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH "/home/ub24/software/Qt/6.10.1/gcc_64" CACHE PATH "Qt6.10 安装路径")

find_package(Qt6 REQUIRED COMPONENTS Qml Quick Core Svg  Network Test)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

qt_policy(SET QTP0001 OLD)
qt_policy(SET QTP0004 NEW)

qt_add_executable(temp1
    CounterModel.cpp
    CounterModel.h
    main.cpp
    CounterView.qml
    CounterViewModel.cpp
    CounterViewModel.h
)

target_link_libraries(temp1 PRIVATE Qt6::Quick Qt6::Core Qt6::Svg Qt6::Qml Qt6::Network Qt6::Gui)

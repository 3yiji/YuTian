cmake_minimum_required(VERSION 3.16)
project(YuTian)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH "/home/ub24/software/Qt/6.10.1/gcc_64" CACHE PATH "Qt6.10 安装路径")

find_package(Qt6 REQUIRED COMPONENTS Qml Quick Core Svg  Network Test)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

qt_policy(SET QTP0001 OLD)
qt_policy(SET QTP0004 NEW)

file(GLOB_RECURSE ViewModel_FILES       # 收集 ViewModel 目录下所有 .cpp（自动检测新增/删除）
    "src/ViewModel/*.cpp"
)
file(GLOB_RECURSE Model_FILES 
    "src/Model/*.cpp"
)

file(GLOB_RECURSE Header_FILES 
    "src/include/*.h"
)

# 1) 抽出一个“核心库”，放业务代码（Model/ViewModel等）
add_library(YuTianCore STATIC
    ${ViewModel_FILES}
    ${Model_FILES}
    ${Header_FILES}
)

# 2) 设置核心库的头文件路径（PUBLIC：表示使用该库的项目也能使用该头文件路径）
target_include_directories(YuTianCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ViewModel
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/Model
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/Model/search_source
)

target_link_libraries(YuTianCore PUBLIC
    Qt6::Core
    Qt6::Network
)

# ---------------------------主程序-----------------------------
qt_add_executable(YuTian
    main.cpp
    src/app.cpp
    temp/temp.cpp
)


# 收集 View 目录下所有 .qml（自动检测新增/删除）
file(GLOB_RECURSE QML_FILES 
    RELATIVE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/View/*.qml"
)

set(qml_singletons
    "${CMAKE_CURRENT_SOURCE_DIR}/src/View/common/Theme.qml"
)
set_source_files_properties(${qml_singletons}       # 设置为单例
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(YuTian
    URI View
    VERSION 1.0
    QML_FILES
        ${QML_FILES}
)


qt_add_resources(YuTian "qml_resources"
    FILES
        Resources/Resources.qrc
)

target_link_libraries(YuTian PRIVATE YuTianCore Qt6::Quick Qt6::Core Qt6::Svg Qt6::Qml Qt6::Network Qt6::Gui)




# ----------------------------测试部分-------------------------
enable_testing()
qt_add_executable(test1
    tests/test.cpp
)

target_link_libraries(test1 PRIVATE
    YuTianCore
    Qt6::Test
)

add_test(NAME test1 COMMAND test1)
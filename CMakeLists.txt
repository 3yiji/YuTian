cmake_minimum_required(VERSION 3.16)
project(MyMVVMApp)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH "/home/ub24/software/Qt/6.10.1/gcc_64" CACHE PATH "Qt6.10 安装路径")

find_package(Qt6 REQUIRED COMPONENTS Quick Core Svg)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

qt_policy(SET QTP0001 OLD)
qt_policy(SET QTP0004 NEW)

# file(GLOB_RECURSE View_FILES 
#     "View/*.qml"
# )

# # 2. 输出变量内容（关键！）
# message(STATUS "===== View_FILES 包含的文件 =====")
# message(STATUS "变量值：${View_FILES}")  # 输出所有文件路径（空格分隔）
# message(STATUS "文件总数：${CMAKE_ARGC}")  # 可选：输出文件个数
# message(STATUS "===============================")

file(GLOB_RECURSE ViewModel_FILES 
    "src/ViewModel/*.cpp"
)
file(GLOB_RECURSE Model_FILES 
    "src/Model/*.cpp"
)
qt_add_executable(MyMVVMApp
    main.cpp
    temp/temp.cpp
    ${ViewModel_FILES}
    ${Model_FILES}
)


target_include_directories(MyMVVMApp
    PRIVATE  # 仅当前项目（MyMVVMApp）能使用该头文件路径
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ViewModel  # ViewModel/ 目录的绝对路径
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
)

# 收集 View 目录下所有 .qml（自动检测新增/删除）
file(GLOB_RECURSE QML_FILES 
    RELATIVE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/View/*.qml"
)

message(STATUS "变量值：${QML_FILES}")  # 输出所有文件路径（空格分隔）
message(STATUS "变量值end")

qt_add_qml_module(MyMVVMApp
    URI View
    VERSION 1.0
    QML_FILES
        ${QML_FILES}
)


qt_add_resources(MyMVVMApp "qml_resources"
    FILES
        Resources/Resources.qrc
        # View/QmlResources.qrc
)

target_link_libraries(MyMVVMApp PRIVATE Qt6::Quick Qt6::Core Qt6::Svg)

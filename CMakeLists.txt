cmake_minimum_required(VERSION 3.16)
project(LibraryManagementQtApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(${PROJECT_NAME}
    LibraryManagementQtApp.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

# If you later integrate existing backend sources, add them here:
# target_sources(${PROJECT_NAME} PRIVATE
#    User.cpp
#    Admin.cpp
#    Member.cpp
#    Resource.cpp
#    BorrowRecord.cpp
#)

# If using Qt5 instead of Qt6, replace find_package with:
# find_package(Qt5 COMPONENTS Widgets REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)

cmake_minimum_required(VERSION 3.16)
project(StokEnvanterQt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql PrintSupport)


set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    depot_page.h
    depot_page.cpp
    depot_page.ui
    product_page.h
    product_page.cpp
    product_page.ui
    login_page.h
    login_page.cpp
    login_page.ui
    reports_page.h
    reports_page.cpp
    reports_page.ui
    transfer_page.h
    transfer_page.cpp
    transfer_page.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(StokEnvanterQt
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        database_manager.h database_manager.cpp
        product_page.h product_page.cpp
        depot_page.h depot_page.cpp depot_page.ui
        product_page.h product_page.cpp product_page.ui
        login_page.h login_page.cpp login_page.ui
        reports_page.h reports_page.cpp reports_page.ui
        transfer_page.h transfer_page.cpp transfer_page.ui
    )
else()
    if(ANDROID)
        add_library(StokEnvanterQt SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(StokEnvanterQt
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(StokEnvanterQt PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::PrintSupport

)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.StokEnvanterQt)
endif()

set_target_properties(StokEnvanterQt PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS StokEnvanterQt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(StokEnvanterQt)
endif()

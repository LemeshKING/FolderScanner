cmake_minimum_required(VERSION 3.19)
project(FolderScanner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Widgets
    Sql
)

qt_standard_project_setup()

qt_add_executable(FolderScanner
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainWindow.cpp
    mainWindow.h
    src/database/databaseManager.h src/database/databaseManager.cpp
    src/core/scnaInfo.h

)

qt_add_resources(FolderScanner sql
    PREFIX "/sql"
    FILES
        sql/00_init.sql
        sql/01_create_scans.sql
        sql/02_create_scan_entries.sql
        sql/03_create_scan_errors.sql

)

target_include_directories(FolderScanner
    PRIVATE
        src
)

target_link_libraries(FolderScanner
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Sql
)

install(TARGETS FolderScanner
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET FolderScanner
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

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
    Svg
)

qt_standard_project_setup()

qt_add_executable(FolderScanner
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainWindow.cpp
    mainWindow.h
    src/database/databaseManager.h src/database/databaseManager.cpp
    src/core/scnaInfo.h
    src/database/scanRepository.h src/database/scanRepository.cpp
    src/database/repositoryBase.h src/database/repositoryBase.cpp
    src/core/FileEntry.h
    src/database/entryRepository.h src/database/entryRepository.cpp
    src/core/ScanError.h
    src/database/errorRepository.h src/database/errorRepository.cpp
    src/scanner/fileSystemScanner.h src/scanner/fileSystemScanner.cpp
    src/services/scanService.h src/services/scanService.cpp
    src/models/fileTreeModel.h src/models/fileTreeModel.cpp
    src/models/scanHistoryModel.h src/models/scanHistoryModel.cpp
    src/ui/widgets/mainToolBar.h src/ui/widgets/mainToolBar.cpp
    src/ui/styleManager.h src/ui/styleManager.cpp
    src/ui/widgets/scanHistoryPanel.h src/ui/widgets/scanHistoryPanel.cpp
    src/ui/widgets/fileTreePanel.h src/ui/widgets/fileTreePanel.cpp
    src/ui/widgets/fileTreeView.h src/ui/widgets/fileTreeView.cpp
    src/ui/widgets/scanHistoryView.h src/ui/widgets/scanHistoryView.cpp
    src/ui/widgets/fileEntryDetailsPanel.h src/ui/widgets/fileEntryDetailsPanel.cpp
    src/services/comparisonService.h src/services/comparisonService.cpp
    src/core/ComparisonResult.h
    src/models/comparisonModel.h src/models/comparisonModel.cpp
    src/ui/widgets/comparisonView.h src/ui/widgets/comparisonView.cpp
    src/ui/widgets/comparisonPanel.h src/ui/widgets/comparisonPanel.cpp
    src/workers/fileOperationWorker.h src/workers/fileOperationWorker.cpp
    src/models/preparedFileTreeData.h src/models/preparedFileTreeData.cpp
    src/core/ScanLoadResult.h

)

qt_add_resources(FolderScanner sql
    PREFIX "/sql"
    FILES
        sql/00_init.sql
        sql/01_create_scans.sql
        sql/02_create_scan_entries.sql
        sql/03_create_scan_errors.sql

)

qt_add_resources(FolderScanner icons
    PREFIX "/icons"
    FILES
        icons/folder-open.svg
        icons/play.svg
        icons/refresh-cw.svg
        icons/file.svg
        icons/folder.svg
        icons/file-app.png
)

qt_add_resources(FolderScanner styles
    PREFIX "/styles"
    FILES
        styles/main-toolbar.qss
        styles/scan-history-panel.qss
        styles/file-tree-panel.qss
        styles/file-tree-view.qss
        styles/scan-history-view.qss
        styles/file-entry-details-panel.qss
        styles/main-window.qss
        styles/comparison-view.qss
        styles/comparison-panel.qss
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
        Qt6::Svg
)

include(GNUInstallDirs)

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

find_program(WINDEPLOYQT_EXECUTABLE windeployqt)

   if(WINDEPLOYQT_EXECUTABLE)

       add_custom_command(TARGET FolderScanner POST_BUILD
           COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                   --debug
                   --no-translations
                   --no-compiler-runtime
                   \"$<TARGET_FILE:FolderScanner>\"
           COMMENT "Running windeployqt..."
       )
   endif()

   add_custom_command(TARGET FolderScanner POST_BUILD
       COMMAND ${CMAKE_COMMAND} -E copy_if_different
           $<TARGET_RUNTIME_DLLS:FolderScanner>
           $<TARGET_FILE_DIR:FolderScanner>
           COMMAND_EXPAND_LISTS)


       set(CPACK_GENERATOR "NSIS")
       set(CPACK_PACKAGE_NAME "FolderScanner")
       set(CPACK_PACKAGE_VERSION "$PROJECT_VERSION}")
       set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "File system scanning and comparison application")
       set(CPACK_PACKAGE_INSTALL_DIRECTORY "FolderScanner")

       set(CPACK_PACKAGE_EXECUTABLES "FolderScanner" "FolderScanner")

       set(CPACK_NSIS_PACKAGE_NAME "FolderScanner")
       set(CPACK_NSIS_DISPLAY_NAME "FolderScanner")
       set(CPACK_NSIS_EXECUTABLES_DIRECTORY "bin")
       set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icons/file-app.png")
       set(CPACK_NSIS_MUI_UNIICON "{CMAKE_CURRENT_SOURCE_DIR}/icons/file-app.png")

       include(CPack)

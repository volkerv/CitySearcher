cmake_minimum_required(VERSION 3.16)

project(CitySearcher VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Network LinguistTools)

qt_standard_project_setup(REQUIRES 6.8)

# Add tests subdirectory (optional)
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

qt_add_executable(appCitySearcher
    main.cpp
    models/city_model.h
    models/city_model.cpp
    models/city_list_model.h
    models/city_list_model.cpp
    viewmodels/city_search_viewmodel.h
    viewmodels/city_search_viewmodel.cpp
    models/nominatim_search_request.h
    models/nominatim_search_request.cpp
    services/nominatim_client.h
    services/nominatim_client.cpp
    services/nominatim_service.h
    services/nominatim_service.cpp
    services/mock_city_search_service.h
    services/mock_city_search_service.cpp
    interfaces/icity_search_service.h
    concepts/service_concepts.h
    factories/city_search_service_factory.h
    factories/city_search_service_factory.cpp
    utils/debug_logger.h
)

qt_add_qml_module(appCitySearcher
    URI CitySearcher
    VERSION 1.0
    QML_FILES
        Main.qml
        RESOURCES README.md
        RESOURCES .gitignore
)

# Add translations
qt_add_translations(appCitySearcher
    TS_FILES
        translations/citysearcher_de.ts
    SOURCES
        main.cpp
        models/city_model.h
        models/city_model.cpp
        models/city_list_model.h
        models/city_list_model.cpp
        viewmodels/city_search_viewmodel.h
        viewmodels/city_search_viewmodel.cpp
        services/nominatim_service.h
        services/nominatim_service.cpp
        services/mock_city_search_service.h
        services/mock_city_search_service.cpp
        factories/city_search_service_factory.h
        factories/city_search_service_factory.cpp
        Main.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appCitySearcher PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appCitySearcher
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(appCitySearcher PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/models
    ${CMAKE_CURRENT_SOURCE_DIR}/viewmodels
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
file(GLOB README_FILES README.md)
source_group("Docs" FILES ${README_FILES})

target_link_libraries(appCitySearcher
    PRIVATE Qt6::Quick Qt6::Network
)

include(GNUInstallDirs)
install(TARGETS appCitySearcher
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

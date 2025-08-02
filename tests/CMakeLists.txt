# Unit Tests for CitySearcher
cmake_minimum_required(VERSION 3.16)

find_package(Qt6 REQUIRED COMPONENTS Test Core Quick Network)

# Enable Qt's automatic MOC, UIC and RCC processing
set(CMAKE_AUTOMOC ON)

# Common test sources (shared by all tests)
set(COMMON_TEST_SOURCES
    ../models/city_model.cpp
    ../models/city_list_model.cpp
    ../viewmodels/city_search_viewmodel.cpp
    ../services/nominatim_service.cpp
    ../services/nominatim_client.cpp
    ../services/mock_city_search_service.cpp
    ../models/nominatim_search_request.cpp
    ../factories/city_search_service_factory.cpp
)

# Common test headers
set(COMMON_TEST_HEADERS
    ../models/city_model.h
    ../models/city_list_model.h
    ../viewmodels/city_search_viewmodel.h
    ../services/nominatim_service.h
    ../services/nominatim_client.h
    ../services/mock_city_search_service.h
    ../models/nominatim_search_request.h
    ../factories/city_search_service_factory.h
    ../interfaces/icity_search_service.h
)

# Test for CityListModel (deduplication logic)
add_executable(test_city_list_model
    test_city_list_model.cpp
    ${COMMON_TEST_SOURCES}
    ${COMMON_TEST_HEADERS}
)

target_link_libraries(test_city_list_model
    Qt6::Test
    Qt6::Core
    Qt6::Quick
    Qt6::Network
)

target_include_directories(test_city_list_model PRIVATE ..)

# Test for CitySearchViewModel (search coordination)
add_executable(test_city_search_viewmodel
    test_city_search_viewmodel.cpp
    ${COMMON_TEST_SOURCES}
    ${COMMON_TEST_HEADERS}
)

target_link_libraries(test_city_search_viewmodel
    Qt6::Test
    Qt6::Core
    Qt6::Quick
    Qt6::Network
)

target_include_directories(test_city_search_viewmodel PRIVATE ..)

# Test for MockCitySearchService (service behavior)
add_executable(test_mock_city_search_service
    test_mock_city_search_service.cpp
    ${COMMON_TEST_SOURCES}
    ${COMMON_TEST_HEADERS}
)

target_link_libraries(test_mock_city_search_service
    Qt6::Test
    Qt6::Core
    Qt6::Quick
    Qt6::Network
)

target_include_directories(test_mock_city_search_service PRIVATE ..)

# Test for Service Factory
add_executable(test_service_factory
    test_service_factory.cpp
    ${COMMON_TEST_SOURCES}
    ${COMMON_TEST_HEADERS}
)

target_link_libraries(test_service_factory
    Qt6::Test
    Qt6::Core
    Qt6::Quick
    Qt6::Network
)

target_include_directories(test_service_factory PRIVATE ..)

# Add tests to CTest
enable_testing()

add_test(NAME CityListModelTest COMMAND test_city_list_model)
add_test(NAME CitySearchViewModelTest COMMAND test_city_search_viewmodel)
add_test(NAME MockCitySearchServiceTest COMMAND test_mock_city_search_service)
add_test(NAME ServiceFactoryTest COMMAND test_service_factory)

# Test runner target
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS test_city_list_model test_city_search_viewmodel test_mock_city_search_service test_service_factory
    COMMENT "Running all unit tests"
) 
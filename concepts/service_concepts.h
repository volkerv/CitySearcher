#pragma once
#include <concepts>
#include <QString>

// Forward declaration
class ICitySearchService;

/**
 * @brief Concept for city search service implementations
 * 
 * Ensures that types used as city search services provide the required interface
 */
template<typename T>
concept CitySearchServiceConcept = requires(T t, const QString& query) {
    { t.searchCities(query) } -> std::same_as<void>;
    { t.serviceName() } -> std::convertible_to<QString>;
    { t.isSearching() } -> std::same_as<bool>;
    { t.cancelSearch() } -> std::same_as<void>;
    requires std::derived_from<T, ICitySearchService>;
};

/**
 * @brief Concept for service configuration types
 * 
 * Ensures configuration objects have the required fields
 */
template<typename T>
concept ServiceConfigurationConcept = requires(T config) {
    { config.apiKey } -> std::convertible_to<QString>;
    { config.baseUrl } -> std::convertible_to<QString>;
    { config.rateLimitPerMinute } -> std::convertible_to<int>;
    { config.enableLogging } -> std::convertible_to<bool>;
};

/**
 * @brief Concept for city-like objects
 * 
 * Ensures objects can be used in city-related operations
 */
template<typename T>
concept CityLike = requires(const T* t) {
    { t->displayName() } -> std::convertible_to<QString>;
    { t->name() } -> std::convertible_to<QString>;
    { t->country() } -> std::convertible_to<QString>;
    { t->latitude() } -> std::convertible_to<double>;
    { t->longitude() } -> std::convertible_to<double>;
};


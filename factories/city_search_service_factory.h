#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>
#include <utility>
#include "../interfaces/icity_search_service.h"
#include "../concepts/service_concepts.h"

/**
 * @brief Factory for creating city search service instances
 * 
 * This factory implements the Factory Pattern to create different
 * city search service implementations without the client needing
 * to know about specific implementation classes.
 */
class CitySearchServiceFactory
{
public:
    /**
     * @brief Enumeration of available service types
     */
    enum class ServiceType {
        Nominatim,      // OpenStreetMap Nominatim service
        GooglePlaces,   // Google Places API (future implementation)
        Mock            // Mock service for testing
    };

    /**
     * @brief Service configuration for API keys, endpoints, etc.
     */
    struct ServiceConfiguration {
        QString apiKey{};
        QString baseUrl{};
        int rateLimitPerMinute{60};
        QStringList supportedCountries{};
        bool enableLogging{true};
        int timeoutMs{10000};
        
        ServiceConfiguration() = default;
        explicit ServiceConfiguration(QString  key) : apiKey(std::move(key)) {}
    };

    // Factory methods
    static std::unique_ptr<ICitySearchService> createService(
        ServiceType type, 
        QObject* parent = nullptr);
        
    static std::unique_ptr<ICitySearchService> createService(
        ServiceType type, 
        const ServiceConfiguration& config,
        QObject* parent = nullptr);

    // Service introspection
    static QStringList availableServices();
    static ServiceType defaultService();
    static QString serviceTypeToString(ServiceType type);
    static ServiceType serviceTypeFromString(const QString& name);
    
    // Service validation
    static bool isServiceAvailable(ServiceType type);
    static bool requiresApiKey(ServiceType type);
    static QString serviceDescription(ServiceType type);
    
    template<CitySearchServiceConcept ServiceType>
    static std::unique_ptr<ServiceType> createTypedService(QObject* parent = nullptr) {
        return std::make_unique<ServiceType>(parent);
    }
    
    template<ServiceConfigurationConcept ConfigType>
    static std::unique_ptr<ICitySearchService> createConfiguredService(
        ServiceType type, 
        const ConfigType& config,
        QObject* parent = nullptr) {
        return createService(type, config, parent);
    }

private:
    // Private constructor - this is a utility class
    CitySearchServiceFactory() = default;
    
    // Helper methods for creating specific services
    static std::unique_ptr<ICitySearchService> createNominatimService(
        const ServiceConfiguration& config, QObject* parent);
    static std::unique_ptr<ICitySearchService> createMockService(
        const ServiceConfiguration& config, QObject* parent);
    
    // Future service creators
    static std::unique_ptr<ICitySearchService> createGooglePlacesService(
        const ServiceConfiguration& config, const QObject* parent);
};

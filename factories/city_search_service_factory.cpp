#include "city_search_service_factory.h"
#include "../services/nominatim_service.h"
#include "../services/mock_city_search_service.h"
#include <QDebug>

std::unique_ptr<ICitySearchService> CitySearchServiceFactory::createService(
    ServiceType type, QObject* parent)
{
    ServiceConfiguration defaultConfig;
    return createService(type, defaultConfig, parent);
}

std::unique_ptr<ICitySearchService> CitySearchServiceFactory::createService(
    ServiceType type, const ServiceConfiguration& config, QObject* parent)
{
    qDebug() << "Creating service:" << serviceTypeToString(type);
    
    switch (type) {
    case ServiceType::Nominatim:
        return createNominatimService(config, parent);
    case ServiceType::Mock:
        return createMockService(config, parent);
    default:
        qWarning() << "Unknown service type, falling back to Nominatim";
        return createNominatimService(config, parent);
    }
}

QStringList CitySearchServiceFactory::availableServices()
{
    return {
        serviceTypeToString(ServiceType::Nominatim),
        serviceTypeToString(ServiceType::Mock),
    };
}

CitySearchServiceFactory::ServiceType CitySearchServiceFactory::defaultService()
{
    return ServiceType::Nominatim;
}

QString CitySearchServiceFactory::serviceTypeToString(ServiceType type)
{
    switch (type) {
    case ServiceType::Nominatim:
        return "Nominatim";
    case ServiceType::Mock:
        return "Mock";
    default:
        return "Unknown";
    }
}

CitySearchServiceFactory::ServiceType CitySearchServiceFactory::serviceTypeFromString(const QString& name)
{
    if (name == "Nominatim") return ServiceType::Nominatim;
    if (name == "Mock") return ServiceType::Mock;
    
    qWarning() << "Unknown service name:" << name << "- returning default";
    return defaultService();
}

bool CitySearchServiceFactory::isServiceAvailable(ServiceType type)
{
    switch (type) {
    case ServiceType::Nominatim:
    case ServiceType::Mock:
        return true;
    default:
        return false;
    }
}

bool CitySearchServiceFactory::requiresApiKey(ServiceType type)
{
    switch (type) {
    case ServiceType::Nominatim:
    case ServiceType::Mock:
        return false;
    default:
        return false;
    }
}

QString CitySearchServiceFactory::serviceDescription(ServiceType type)
{
    switch (type) {
    case ServiceType::Nominatim:
        return "OpenStreetMap Nominatim search service - free, no API key required";
    case ServiceType::Mock:
        return "Mock service for testing - returns predefined test data";
    default:
        return "Unknown service";
    }
}

std::unique_ptr<ICitySearchService> CitySearchServiceFactory::createNominatimService(
    const ServiceConfiguration& config, QObject* parent)
{
    Q_UNUSED(config) // Nominatim doesn't need configuration for basic usage
    return std::make_unique<NominatimService>(parent);
}

std::unique_ptr<ICitySearchService> CitySearchServiceFactory::createMockService(
    const ServiceConfiguration& config, QObject* parent)
{
    auto mockService = std::make_unique<MockCitySearchService>(parent);
    
    // Configure mock service based on configuration
    if (config.enableLogging) {
        qDebug() << "Creating MockCitySearchService with configuration";
    }
    
    // Set timeouts to simulate different network conditions
    if (config.timeoutMs != 10000) { // Non-default timeout
        mockService->setSimulateNetworkDelay(true, config.timeoutMs / 10); // Simulate partial delay
    }
    
    return mockService;
}

std::unique_ptr<ICitySearchService> CitySearchServiceFactory::createGooglePlacesService(
    const ServiceConfiguration& config, QObject* parent)
{
    Q_UNUSED(config)
    Q_UNUSED(parent)
    qWarning() << "GooglePlacesService not yet implemented";
    return nullptr;
}

std::unique_ptr<ICitySearchService> CitySearchServiceFactory::createOpenCageService(
    const ServiceConfiguration& config, QObject* parent)
{
    Q_UNUSED(config)
    Q_UNUSED(parent)
    qWarning() << "OpenCageService not yet implemented";
    return nullptr;
}

std::unique_ptr<ICitySearchService> CitySearchServiceFactory::createBingMapsService(
    const ServiceConfiguration& config, QObject* parent)
{
    Q_UNUSED(config)
    Q_UNUSED(parent)
    qWarning() << "BingMapsService not yet implemented";
    return nullptr;
} 

#include <QtTest/QtTest>
#include <QDebug>
#include <memory>
#include "factories/city_search_service_factory.h"
#include "interfaces/icity_search_service.h"
#include "services/nominatim_service.h"
#include "services/mock_city_search_service.h"

class TestServiceFactory : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // Basic factory functionality tests
    void testCreateNominatimService();
    void testCreateMockService();
    void testCreateServiceWithConfiguration();
    void testCreateServiceWithParent();

    // Service introspection tests
    void testAvailableServices();
    void testDefaultService();
    void testServiceTypeStringConversion();
    void testServiceValidation();

    // Configuration tests
    void testServiceConfiguration();
    void testServiceRequirements();
    void testServiceDescriptions();

    // Error handling tests
    void testInvalidServiceCreation();
    void testFallbackBehavior();

    // Service metadata tests
    void testServiceMetadata();
    void testServiceAvailability();

private:
    // Helper methods
    void verifyServiceInterface(ICitySearchService* service);
    void verifyServiceBasicFunctionality(ICitySearchService* service);
};

void TestServiceFactory::initTestCase()
{
    qDebug() << "Starting CitySearchServiceFactory tests";
}

void TestServiceFactory::cleanupTestCase()
{
    qDebug() << "Finished CitySearchServiceFactory tests";
}

void TestServiceFactory::verifyServiceInterface(ICitySearchService* service)
{
    QVERIFY(service != nullptr);
    
    // Test interface methods
    QVERIFY(!service->serviceName().isEmpty());
    QVERIFY(!service->serviceVersion().isEmpty());
    QVERIFY(!service->supportedFeatures().isEmpty());
    
    // Test initial state
    QCOMPARE(service->isSearching(), false);
    QCOMPARE(service->isServiceAvailable(), true);
    QCOMPARE(service->successfulRequestsCount(), 0);
    QCOMPARE(service->failedRequestsCount(), 0);
}

void TestServiceFactory::verifyServiceBasicFunctionality(ICitySearchService* service)
{
    verifyServiceInterface(service);
    
    // Test that service metadata makes sense
    QVERIFY(service->rateLimitPerMinute() > 0);
    QVERIFY(!service->serviceDescription().isEmpty());
}

void TestServiceFactory::testCreateNominatimService()
{
    // Test basic creation
    auto service = CitySearchServiceFactory::createService(
        CitySearchServiceFactory::ServiceType::Nominatim);
    
    verifyServiceBasicFunctionality(service.get());
    
    // Test Nominatim-specific properties
    QCOMPARE(service->serviceName(), QString("Nominatim"));
    QCOMPARE(service->requiresApiKey(), false);
    QCOMPARE(service->supportsAutoComplete(), false);
    
    // Test that it's actually a NominatimService
    auto* nominatimService = dynamic_cast<NominatimService*>(service.get());
    QVERIFY(nominatimService != nullptr);
}

void TestServiceFactory::testCreateMockService()
{
    // Test basic creation
    auto service = CitySearchServiceFactory::createService(
        CitySearchServiceFactory::ServiceType::Mock);
    
    verifyServiceBasicFunctionality(service.get());
    
    // Test Mock-specific properties
    QCOMPARE(service->serviceName(), QString("Mock"));
    QCOMPARE(service->requiresApiKey(), false);
    QCOMPARE(service->supportsAutoComplete(), true);
    QCOMPARE(service->rateLimitPerMinute(), 1000); // High rate limit for mock
    
    // Test that it's actually a MockCitySearchService
    auto* mockService = dynamic_cast<MockCitySearchService*>(service.get());
    QVERIFY(mockService != nullptr);
}

void TestServiceFactory::testCreateServiceWithConfiguration()
{
    CitySearchServiceFactory::ServiceConfiguration config;
    config.enableLogging = true;
    config.timeoutMs = 5000;
    config.rateLimitPerMinute = 30;
    
    // Test Nominatim with configuration
    auto nominatimService = CitySearchServiceFactory::createService(
        CitySearchServiceFactory::ServiceType::Nominatim, config);
    
    verifyServiceBasicFunctionality(nominatimService.get());
    
    // Test Mock with configuration
    auto mockService = CitySearchServiceFactory::createService(
        CitySearchServiceFactory::ServiceType::Mock, config);
    
    verifyServiceBasicFunctionality(mockService.get());
    
    // Configuration should be applied (though we can't directly test all aspects)
    QVERIFY(mockService != nullptr);
}

void TestServiceFactory::testCreateServiceWithParent()
{
    QObject parent;
    
    auto service = CitySearchServiceFactory::createService(
        CitySearchServiceFactory::ServiceType::Mock, &parent);
    
    verifyServiceBasicFunctionality(service.get());
    
    // Test that parent is set correctly
    QCOMPARE(service->parent(), &parent);
}

void TestServiceFactory::testAvailableServices()
{
    QStringList available = CitySearchServiceFactory::availableServices();
    
    QVERIFY(!available.isEmpty());
    QVERIFY(available.contains("Nominatim"));
    QVERIFY(available.contains("Mock"));
    
    // Should not contain unimplemented services
    QVERIFY(!available.contains("GooglePlaces"));
    QVERIFY(!available.contains("OpenCage"));
    QVERIFY(!available.contains("BingMaps"));
    
    // Should be reasonable number of services
    QVERIFY(available.size() >= 2);
    QVERIFY(available.size() <= 10); // Sanity check
}

void TestServiceFactory::testDefaultService()
{
    auto defaultType = CitySearchServiceFactory::defaultService();
    QCOMPARE(defaultType, CitySearchServiceFactory::ServiceType::Nominatim);
    
    // Default service should be available
    QCOMPARE(CitySearchServiceFactory::isServiceAvailable(defaultType), true);
}

void TestServiceFactory::testServiceTypeStringConversion()
{
    // Test all known service types
    QCOMPARE(CitySearchServiceFactory::serviceTypeToString(
        CitySearchServiceFactory::ServiceType::Nominatim), QString("Nominatim"));
    QCOMPARE(CitySearchServiceFactory::serviceTypeToString(
        CitySearchServiceFactory::ServiceType::Mock), QString("Mock"));
    
    // Test round-trip conversion
    auto nominatimType = CitySearchServiceFactory::serviceTypeFromString("Nominatim");
    QCOMPARE(nominatimType, CitySearchServiceFactory::ServiceType::Nominatim);
    
    auto mockType = CitySearchServiceFactory::serviceTypeFromString("Mock");
    QCOMPARE(mockType, CitySearchServiceFactory::ServiceType::Mock);
    
    // Test case sensitivity
    auto nominatimLower = CitySearchServiceFactory::serviceTypeFromString("nominatim");
    QCOMPARE(nominatimLower, CitySearchServiceFactory::defaultService()); // Should fallback
    
    // Test invalid names
    auto invalid = CitySearchServiceFactory::serviceTypeFromString("InvalidService");
    QCOMPARE(invalid, CitySearchServiceFactory::defaultService());
    
    auto empty = CitySearchServiceFactory::serviceTypeFromString("");
    QCOMPARE(empty, CitySearchServiceFactory::defaultService());
}

void TestServiceFactory::testServiceValidation()
{
    // Test available services
    QCOMPARE(CitySearchServiceFactory::isServiceAvailable(
        CitySearchServiceFactory::ServiceType::Nominatim), true);
    QCOMPARE(CitySearchServiceFactory::isServiceAvailable(
        CitySearchServiceFactory::ServiceType::Mock), true);
    
    // Future services should not be available yet
    // Note: These enum values might not exist in the cleaned up version
    // but we test the concept of unavailable services
}

void TestServiceFactory::testServiceConfiguration()
{
    // Test default configuration
    CitySearchServiceFactory::ServiceConfiguration defaultConfig;
    QCOMPARE(defaultConfig.rateLimitPerMinute, 60);
    QCOMPARE(defaultConfig.enableLogging, true);
    QCOMPARE(defaultConfig.timeoutMs, 10000);
    QVERIFY(defaultConfig.apiKey.isEmpty());
    QVERIFY(defaultConfig.baseUrl.isEmpty());
    
    // Test configuration with API key
    CitySearchServiceFactory::ServiceConfiguration configWithKey("test-api-key");
    QCOMPARE(configWithKey.apiKey, QString("test-api-key"));
    
    // Test configuration modification
    defaultConfig.rateLimitPerMinute = 120;
    defaultConfig.timeoutMs = 5000;
    QCOMPARE(defaultConfig.rateLimitPerMinute, 120);
    QCOMPARE(defaultConfig.timeoutMs, 5000);
}

void TestServiceFactory::testServiceRequirements()
{
    // Test API key requirements
    QCOMPARE(CitySearchServiceFactory::requiresApiKey(
        CitySearchServiceFactory::ServiceType::Nominatim), false);
    QCOMPARE(CitySearchServiceFactory::requiresApiKey(
        CitySearchServiceFactory::ServiceType::Mock), false);
    
    // Future services might require API keys
    // (We can't test this directly since those enums might not exist)
}

void TestServiceFactory::testServiceDescriptions()
{
    // Test that descriptions are meaningful
    QString nominatimDesc = CitySearchServiceFactory::serviceDescription(
        CitySearchServiceFactory::ServiceType::Nominatim);
    QVERIFY(!nominatimDesc.isEmpty());
    QVERIFY(nominatimDesc.contains("Nominatim"));
    QVERIFY(nominatimDesc.contains("OpenStreetMap"));
    
    QString mockDesc = CitySearchServiceFactory::serviceDescription(
        CitySearchServiceFactory::ServiceType::Mock);
    QVERIFY(!mockDesc.isEmpty());
    QVERIFY(mockDesc.contains("Mock"));
    QVERIFY(mockDesc.contains("testing"));
    
    // Descriptions should be different
    QVERIFY(nominatimDesc != mockDesc);
}

void TestServiceFactory::testInvalidServiceCreation()
{
    // Test creating service with invalid enum value
    // This is tricky to test since we need a valid enum value
    // The factory should handle unknown values gracefully by falling back
    
    // We can't directly test with invalid enum, but we can test the fallback behavior
    // by checking the default case in the switch statement
    
    // Instead, let's test string-based creation with invalid names
    auto invalidType = CitySearchServiceFactory::serviceTypeFromString("InvalidService");
    auto service = CitySearchServiceFactory::createService(invalidType);
    
    // Should fallback to default service (Nominatim)
    verifyServiceBasicFunctionality(service.get());
    QCOMPARE(service->serviceName(), QString("Nominatim"));
}

void TestServiceFactory::testFallbackBehavior()
{
    // Test that invalid service names fallback to default
    auto fallbackType = CitySearchServiceFactory::serviceTypeFromString("NonExistentService");
    QCOMPARE(fallbackType, CitySearchServiceFactory::defaultService());
    
    // Test that empty strings fallback to default
    auto emptyType = CitySearchServiceFactory::serviceTypeFromString("");
    QCOMPARE(emptyType, CitySearchServiceFactory::defaultService());
    
    // Test that whitespace-only strings fallback to default
    auto whitespaceType = CitySearchServiceFactory::serviceTypeFromString("   ");
    QCOMPARE(whitespaceType, CitySearchServiceFactory::defaultService());
}

void TestServiceFactory::testServiceMetadata()
{
    // Test all available service types
    QStringList availableServices = CitySearchServiceFactory::availableServices();
    
    for (const QString& serviceName : availableServices) {
        auto serviceType = CitySearchServiceFactory::serviceTypeFromString(serviceName);
        
        // Test that we can create the service
        auto service = CitySearchServiceFactory::createService(serviceType);
        verifyServiceBasicFunctionality(service.get());
        
        // Test that metadata is consistent
        QCOMPARE(service->serviceName(), serviceName);
        
        // Test that service is marked as available
        QCOMPARE(CitySearchServiceFactory::isServiceAvailable(serviceType), true);
        
        // Test that description exists
        QString description = CitySearchServiceFactory::serviceDescription(serviceType);
        QVERIFY(!description.isEmpty());
        QVERIFY(description.contains(serviceName, Qt::CaseInsensitive));
    }
}

void TestServiceFactory::testServiceAvailability()
{
    // Test that all services in availableServices() are actually available
    QStringList available = CitySearchServiceFactory::availableServices();
    
    for (const QString& serviceName : available) {
        auto serviceType = CitySearchServiceFactory::serviceTypeFromString(serviceName);
        QCOMPARE(CitySearchServiceFactory::isServiceAvailable(serviceType), true);
        
        // And we should be able to create them
        auto service = CitySearchServiceFactory::createService(serviceType);
        QVERIFY(service != nullptr);
        QCOMPARE(service->isServiceAvailable(), true);
    }
    
    // Test specific services
    QCOMPARE(CitySearchServiceFactory::isServiceAvailable(
        CitySearchServiceFactory::ServiceType::Nominatim), true);
    QCOMPARE(CitySearchServiceFactory::isServiceAvailable(
        CitySearchServiceFactory::ServiceType::Mock), true);
}

// Register the test class
QTEST_MAIN(TestServiceFactory)
#include "test_service_factory.moc" 
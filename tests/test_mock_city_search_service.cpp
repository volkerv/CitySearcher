#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QDebug>
#include "services/mock_city_search_service.h"
#include "../models/city_model.h"

class TestMockCitySearchService : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic service interface tests
    void testServiceMetadata();
    void testInitialState();
    void testServiceInterface();

    // Search functionality tests
    void testSuccessfulSearch();
    void testEmptyQuerySearch();
    void testSearchCancellation();
    void testSearchStateManagement();

    // Mock-specific configuration tests
    void testNetworkDelaySimulation();
    void testErrorSimulation();
    void testCustomResults();
    void testDuplicateGeneration();

    // Signal emission tests
    void testSearchSignals();
    void testSearchCompletedSignal();
    void testSearchErrorSignal();
    void testStateSignals();

    // Data generation tests
    void testMockDataGeneration();
    void testQueryFiltering();
    void testCoordinateGeneration();

    // Performance and statistics tests
    void testServiceStatistics();
    void testLargeQueryHandling();

private:
    MockCitySearchService* service;
    
    // Helper methods
    void waitForSearchCompletion(int timeoutMs = 5000);
    void verifySearchState(bool expectedSearching);
    QList<CityModel*> createTestCities();
};

void TestMockCitySearchService::initTestCase()
{
    qDebug() << "Starting MockCitySearchService tests";
}

void TestMockCitySearchService::cleanupTestCase()
{
    qDebug() << "Finished MockCitySearchService tests";
}

void TestMockCitySearchService::init()
{
    service = new MockCitySearchService(this);
    // Configure for fast testing by default
    service->setSimulateNetworkDelay(false);
    service->setSimulateErrors(false);
}

void TestMockCitySearchService::cleanup()
{
    delete service;
    service = nullptr;
}

void TestMockCitySearchService::waitForSearchCompletion(int timeoutMs)
{
    QElapsedTimer timer;
    timer.start();
    
    while (service->isSearching() && timer.elapsed() < timeoutMs) {
        QCoreApplication::processEvents();
        QThread::msleep(10);
    }
}

void TestMockCitySearchService::verifySearchState(bool expectedSearching)
{
    QCOMPARE(service->isSearching(), expectedSearching);
}

QList<CityModel*> TestMockCitySearchService::createTestCities()
{
    QList<CityModel*> cities;
    cities.append(new CityModel("Test City 1", "Test City 1, Test Country", "Test Country", 50.0, 10.0));
    cities.append(new CityModel("Test City 2", "Test City 2, Test Country", "Test Country", 51.0, 11.0));
    cities.append(new CityModel("Test City 3", "Test City 3, Test Country", "Test Country", 52.0, 12.0));
    return cities;
}

void TestMockCitySearchService::testServiceMetadata()
{
    // Test basic metadata
    QCOMPARE(service->serviceName(), QString("Mock"));
    QCOMPARE(service->serviceVersion(), QString("1.0-test"));
    
    // Test description
    QString description = service->serviceDescription();
    QVERIFY(description.contains("Mock"));
    QVERIFY(description.contains("testing"));
    
    // Test features
    QStringList features = service->supportedFeatures();
    QVERIFY(features.contains("basic_search"));
    QVERIFY(features.contains("autocomplete"));
    QVERIFY(features.contains("custom_results"));
    QVERIFY(features.contains("error_simulation"));
    QVERIFY(features.contains("delay_simulation"));
    
    // Test optional features
    QCOMPARE(service->supportsAutoComplete(), true);
    QCOMPARE(service->requiresApiKey(), false);
    QCOMPARE(service->rateLimitPerMinute(), 1000);
    
    // Test supported countries
    QStringList countries = service->supportedCountries();
    QVERIFY(countries.contains("US"));
    QVERIFY(countries.contains("DE"));
    QVERIFY(countries.contains("FR"));
    QVERIFY(countries.contains("UK"));
}

void TestMockCitySearchService::testInitialState()
{
    // Test initial state
    QCOMPARE(service->isSearching(), false);
    QCOMPARE(service->isServiceAvailable(), true);
    QCOMPARE(service->lastErrorMessage(), QString());
    QCOMPARE(service->successfulRequestsCount(), 0);
    QCOMPARE(service->failedRequestsCount(), 0);
}

void TestMockCitySearchService::testServiceInterface()
{
    // Test that all interface methods are implemented
    QVERIFY(service->serviceName() != QString());
    QVERIFY(service->serviceVersion() != QString());
    QVERIFY(!service->supportedFeatures().isEmpty());
    QVERIFY(service->serviceDescription() != QString());
    
    // Test that the service is available
    QCOMPARE(service->isServiceAvailable(), true);
}

void TestMockCitySearchService::testSuccessfulSearch()
{
    QSignalSpy startedSpy(service, &MockCitySearchService::searchStarted);
    QSignalSpy finishedSpy(service, &MockCitySearchService::searchFinished);
    QSignalSpy citiesFoundSpy(service, &MockCitySearchService::citiesFound);
    QSignalSpy errorSpy(service, &MockCitySearchService::searchError);
    
    // Start search
    service->searchCities("Berlin");
    
    waitForSearchCompletion();
    
    // Verify final state
    verifySearchState(false);
    
    // Verify signals
    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(citiesFoundSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);
    
    // Verify results
    QList<QVariant> arguments = citiesFoundSpy.first();
    QVariant citiesVariant = arguments.first();
    // We can't easily cast this back to QList<CityModel*> in the test
    // but we can verify the signal was emitted
    
    // Verify statistics
    QCOMPARE(service->successfulRequestsCount(), 1);
    QCOMPARE(service->failedRequestsCount(), 0);
    QCOMPARE(service->lastErrorMessage(), QString());
}

void TestMockCitySearchService::testEmptyQuerySearch()
{
    QSignalSpy errorSpy(service, &MockCitySearchService::searchError);
    QSignalSpy citiesFoundSpy(service, &MockCitySearchService::citiesFound);
    
    // Search with empty query
    service->searchCities("");
    
    waitForSearchCompletion();
    
    // Should have error and no results
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(citiesFoundSpy.count(), 0);
    
    // Verify error message
    QList<QVariant> arguments = errorSpy.first();
    QString errorMessage = arguments.first().toString();
    QVERIFY(errorMessage.contains("search query"));
    
    // Verify statistics
    QCOMPARE(service->successfulRequestsCount(), 0);
    QCOMPARE(service->failedRequestsCount(), 1);
    QVERIFY(!service->lastErrorMessage().isEmpty());
}

void TestMockCitySearchService::testSearchCancellation()
{
    // Enable delay for cancellation testing
    service->setSimulateNetworkDelay(true, 1000);
    
    QSignalSpy startedSpy(service, &MockCitySearchService::searchStarted);
    QSignalSpy finishedSpy(service, &MockCitySearchService::searchFinished);
    QSignalSpy citiesFoundSpy(service, &MockCitySearchService::citiesFound);
    
    // Start search
    service->searchCities("Berlin");
    
    // Verify search started
    QCOMPARE(startedSpy.count(), 1);
    verifySearchState(true);
    
    // Cancel search
    service->cancelSearch();
    
    waitForSearchCompletion();
    
    // Verify search stopped
    verifySearchState(false);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(citiesFoundSpy.count(), 0); // No results due to cancellation
}

void TestMockCitySearchService::testSearchStateManagement()
{
    service->setSimulateNetworkDelay(true, 100);
    
    // Initial state
    verifySearchState(false);
    
    // Start search
    service->searchCities("Berlin");
    
    // Should be searching immediately
    verifySearchState(true);
    
    waitForSearchCompletion();
    
    // Should be finished
    verifySearchState(false);
}

void TestMockCitySearchService::testNetworkDelaySimulation()
{
    QElapsedTimer timer;
    
    // Test with delay
    service->setSimulateNetworkDelay(true, 200);
    
    timer.start();
    service->searchCities("Berlin");
    waitForSearchCompletion();
    qint64 delayElapsed = timer.elapsed();
    
    // Should take at least the simulated delay
    QVERIFY(delayElapsed >= 180); // Allow some tolerance
    
    // Test without delay
    service->setSimulateNetworkDelay(false);
    
    timer.restart();
    service->searchCities("Munich");
    waitForSearchCompletion();
    qint64 noDelayElapsed = timer.elapsed();
    
    // Should be much faster
    QVERIFY(noDelayElapsed < 50);
}

void TestMockCitySearchService::testErrorSimulation()
{
    QSignalSpy errorSpy(service, &MockCitySearchService::searchError);
    QSignalSpy citiesFoundSpy(service, &MockCitySearchService::citiesFound);
    
    // Configure 100% error rate
    service->setSimulateErrors(true, 1.0);
    
    // Multiple searches should all fail
    for (int i = 0; i < 5; ++i) {
        service->searchCities(QString("Test%1").arg(i));
        waitForSearchCompletion();
    }
    
    // All should have failed
    QCOMPARE(errorSpy.count(), 5);
    QCOMPARE(citiesFoundSpy.count(), 0);
    QCOMPARE(service->failedRequestsCount(), 5);
    QCOMPARE(service->successfulRequestsCount(), 0);
    
    // Configure 0% error rate
    service->setSimulateErrors(false);
    
    service->searchCities("Berlin");
    waitForSearchCompletion();
    
    // Should succeed now
    QCOMPARE(service->successfulRequestsCount(), 1);
}

void TestMockCitySearchService::testCustomResults()
{
    QSignalSpy citiesFoundSpy(service, &MockCitySearchService::citiesFound);
    
    // Set custom results
    QList<CityModel*> customCities = createTestCities();
    service->setCustomResults(customCities);
    
    // Search should return custom results
    service->searchCities("anything");
    waitForSearchCompletion();
    
    // Should have received results
    QCOMPARE(citiesFoundSpy.count(), 1);
    
    // Clear custom results
    service->clearCustomResults();
    
    // Search again - should use normal mock data
    citiesFoundSpy.clear();
    service->searchCities("Berlin");
    waitForSearchCompletion();
    
    QCOMPARE(citiesFoundSpy.count(), 1);
    // Results should be different (normal mock data, not custom)
}

void TestMockCitySearchService::testDuplicateGeneration()
{
    QSignalSpy citiesFoundSpy(service, &MockCitySearchService::citiesFound);
    
    // Enable duplicate generation
    service->setIncludeDuplicatesInResults(true);
    
    // Search for "test" which triggers duplicate generation
    service->searchCities("test");
    waitForSearchCompletion();
    
    // Should have received results with duplicates
    QCOMPARE(citiesFoundSpy.count(), 1);
    
    // Disable duplicate generation
    service->setIncludeDuplicatesInResults(false);
    
    citiesFoundSpy.clear();
    service->searchCities("test");
    waitForSearchCompletion();
    
    // Should still have results but potentially different count
    QCOMPARE(citiesFoundSpy.count(), 1);
}

void TestMockCitySearchService::testSearchSignals()
{
    QSignalSpy startedSpy(service, &MockCitySearchService::searchStarted);
    QSignalSpy finishedSpy(service, &MockCitySearchService::searchFinished);
    QSignalSpy citiesFoundSpy(service, &MockCitySearchService::citiesFound);
    QSignalSpy errorSpy(service, &MockCitySearchService::searchError);
    
    // Successful search
    service->searchCities("Berlin");
    waitForSearchCompletion();
    
    // Verify signal order and counts
    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(citiesFoundSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);
    
    // Test error case
    service->setSimulateErrors(true, 1.0);
    
    startedSpy.clear();
    finishedSpy.clear();
    citiesFoundSpy.clear();
    errorSpy.clear();
    
    service->searchCities("Test");
    waitForSearchCompletion();
    
    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(citiesFoundSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
}

void TestMockCitySearchService::testSearchCompletedSignal()
{
    QSignalSpy citiesFoundSpy(service, &MockCitySearchService::citiesFound);
    
    service->searchCities("Berlin");
    waitForSearchCompletion();
    
    QCOMPARE(citiesFoundSpy.count(), 1);
    
    // The signal should contain a QList<CityModel*>
    // We can't easily verify the exact content in the test, 
    // but we can verify the signal was emitted with the right signature
    QList<QVariant> arguments = citiesFoundSpy.first();
    QCOMPARE(arguments.size(), 1);
}

void TestMockCitySearchService::testSearchErrorSignal()
{
    QSignalSpy errorSpy(service, &MockCitySearchService::searchError);
    
    // Test empty query error
    service->searchCities("");
    waitForSearchCompletion();
    
    QCOMPARE(errorSpy.count(), 1);
    
    QList<QVariant> arguments = errorSpy.first();
    QString errorMessage = arguments.first().toString();
    QVERIFY(!errorMessage.isEmpty());
    QVERIFY(errorMessage.contains("search query"));
}

void TestMockCitySearchService::testStateSignals()
{
    QSignalSpy startedSpy(service, &MockCitySearchService::searchStarted);
    QSignalSpy finishedSpy(service, &MockCitySearchService::searchFinished);
    
    service->setSimulateNetworkDelay(true, 100);
    
    service->searchCities("Berlin");
    
    // Should get started signal immediately
    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 0);
    
    waitForSearchCompletion();
    
    // Should get finished signal after completion
    QCOMPARE(finishedSpy.count(), 1);
}

void TestMockCitySearchService::testMockDataGeneration()
{
    QSignalSpy citiesFoundSpy(service, &MockCitySearchService::citiesFound);
    
    // Test different queries to verify data generation
    QStringList testQueries = {"Berlin", "London", "Paris", "New York", "unknowncity"};
    
    for (const QString& query : testQueries) {
        citiesFoundSpy.clear();
        service->searchCities(query);
        waitForSearchCompletion();
        
        if (query == "unknowncity") {
            // Unknown cities should generate generic results
            QCOMPARE(citiesFoundSpy.count(), 1);
        } else {
            // Known cities should return results
            QCOMPARE(citiesFoundSpy.count(), 1);
        }
    }
}

void TestMockCitySearchService::testQueryFiltering()
{
    QSignalSpy citiesFoundSpy(service, &MockCitySearchService::citiesFound);
    
    // Test case-insensitive matching
    QStringList variations = {"Berlin", "berlin", "BERLIN", "BeRlIn"};
    
    for (const QString& query : variations) {
        citiesFoundSpy.clear();
        service->searchCities(query);
        waitForSearchCompletion();
        
        // All variations should return results
        QCOMPARE(citiesFoundSpy.count(), 1);
    }
}

void TestMockCitySearchService::testCoordinateGeneration()
{
    // This test verifies that generated cities have reasonable coordinates
    // We can't easily inspect the actual results, but we can verify
    // that the service generates them consistently
    
    QSignalSpy citiesFoundSpy(service, &MockCitySearchService::citiesFound);
    
    // Test multiple searches of the same query
    for (int i = 0; i < 3; ++i) {
        service->searchCities("Berlin");
        waitForSearchCompletion();
        QCOMPARE(citiesFoundSpy.count(), i + 1);
    }
    
    // All searches for the same query should succeed
    QCOMPARE(service->successfulRequestsCount(), 3);
}

void TestMockCitySearchService::testServiceStatistics()
{
    // Initial state
    QCOMPARE(service->successfulRequestsCount(), 0);
    QCOMPARE(service->failedRequestsCount(), 0);
    
    // Successful searches
    service->searchCities("Berlin");
    waitForSearchCompletion();
    
    service->searchCities("Paris");
    waitForSearchCompletion();
    
    QCOMPARE(service->successfulRequestsCount(), 2);
    QCOMPARE(service->failedRequestsCount(), 0);
    
    // Failed search
    service->searchCities("");
    waitForSearchCompletion();
    
    QCOMPARE(service->successfulRequestsCount(), 2);
    QCOMPARE(service->failedRequestsCount(), 1);
    
    // Error simulation
    service->setSimulateErrors(true, 1.0);
    service->searchCities("Munich");
    waitForSearchCompletion();
    
    QCOMPARE(service->successfulRequestsCount(), 2);
    QCOMPARE(service->failedRequestsCount(), 2);
    
    // Verify last error message
    QVERIFY(!service->lastErrorMessage().isEmpty());
}

void TestMockCitySearchService::testLargeQueryHandling()
{
    QSignalSpy citiesFoundSpy(service, &MockCitySearchService::citiesFound);
    
    // Test with very long query
    QString longQuery = QString("a").repeated(1000);
    service->searchCities(longQuery);
    waitForSearchCompletion();
    
    // Should handle gracefully (either return results or reasonable error)
    QVERIFY(citiesFoundSpy.count() == 1 || service->failedRequestsCount() == 1);
    
    // Test with special characters
    service->searchCities("München@#$%^&*()");
    waitForSearchCompletion();
    
    // Should handle gracefully
    QVERIFY(service->successfulRequestsCount() > 0 || service->failedRequestsCount() > 0);
}

// Register the test class
QTEST_MAIN(TestMockCitySearchService)
#include "test_mock_city_search_service.moc" 
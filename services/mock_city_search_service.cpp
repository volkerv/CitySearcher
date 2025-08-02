#include "mock_city_search_service.h"
#include <QDebug>
#include <QRandomGenerator>

MockCitySearchService::MockCitySearchService(QObject* parent)
    : ICitySearchService(parent)
    , delayTimer_(new QTimer(this))
{
    delayTimer_->setSingleShot(true);
    connect(delayTimer_, &QTimer::timeout,
            this, &MockCitySearchService::simulateSearchCompleted);
}

void MockCitySearchService::searchCities(const QString& query)
{
    Log::debug("Starting mock search");
    Log::info(QString("Mock search query: %1").arg(query));
    
    if (query.trimmed().isEmpty()) {
        QString error = "Please enter a search query";
        Log::error(error);
        updateStats(false, error);
        emit searchError(error);
        return;
    }
    
    if (isSearching_) {
        Log::warning("Search already in progress, cancelling previous search");
        cancelSearch();
    }
    
    currentQuery_ = query;
    isSearching_ = true;
    emit searchStarted();
    
    Log::debug(QString("Mock search configuration - delay: %1ms, error simulation: %2")
                      .arg(simulateDelay_ ? delayMs_ : 0)
                      .arg(simulateErrors_ ? "enabled" : "disabled"));
    
    if (simulateDelay_) {
        Log::debug(QString("Simulating network delay of %1ms").arg(delayMs_));
        delayTimer_->start(delayMs_);
    } else {
        simulateSearchCompleted();
    }
}

void MockCitySearchService::cancelSearch()
{
    if (isSearching_) {
        Log::info("Cancelling mock search");
        delayTimer_->stop();
        isSearching_ = false;
        emit searchFinished();
    } else {
        Log::debug("Cancel requested but no search in progress");
    }
}

bool MockCitySearchService::isSearching() const
{
    return isSearching_;
}

QString MockCitySearchService::serviceName() const
{
    return "Mock";
}

QString MockCitySearchService::serviceVersion() const
{
    return "1.0-test";
}

QStringList MockCitySearchService::supportedFeatures() const
{
    return {"basic_search", "autocomplete", "custom_results", "error_simulation", "delay_simulation"};
}

QString MockCitySearchService::serviceDescription() const
{
    return "Mock service for testing - returns predefined test data with configurable delays and errors";
}

void MockCitySearchService::setSimulateNetworkDelay(bool enable, int delayMs)
{
    simulateDelay_ = enable;
    delayMs_ = delayMs;
    Log::info(QString("Network delay simulation %1 with %2ms delay")
                     .arg(enable ? "enabled" : "disabled")
                     .arg(delayMs));
}

void MockCitySearchService::setSimulateErrors(bool enable, double errorRate)
{
    simulateErrors_ = enable;
    errorRate_ = qBound(0.0, errorRate, 1.0); // Clamp between 0 and 1
    Log::info(QString("Error simulation %1 with %2% error rate")
                     .arg(enable ? "enabled" : "disabled")
                     .arg(errorRate_ * 100, 0, 'f', 1));
}

void MockCitySearchService::setCustomResults(const QList<CityModel*>& cities)
{
    customResults_.clear();
    for (auto* city : cities) {
        // Create copies with this service as parent
        auto* copy = new CityModel(city->name(), city->displayName(), 
                                  city->country(), city->latitude(), 
                                  city->longitude(), this);
        customResults_.append(copy);
    }
    Log::info(QString("Set %1 custom mock results").arg(customResults_.size()));
}

void MockCitySearchService::clearCustomResults()
{
    qDeleteAll(customResults_);
    customResults_.clear();
    Log::info("Cleared custom mock results");
}

void MockCitySearchService::simulateSearchCompleted()
{
    if (!isSearching_) {
        return; // Search was cancelled
    }
    
    isSearching_ = false;
    
    // Check if we should simulate an error
    if (shouldSimulateError()) {
        QString error = QString("Simulated network error for query: %1").arg(currentQuery_);
        Log::warning("Simulating network error for testing");
        Log::error(error);
        updateStats(false, error);
        emit searchError(error);
        emit searchFinished();
        return;
    }
    
    // Return results
    QList<CityModel*> results;
    if (!customResults_.isEmpty()) {
        // Use custom results if available
        for (auto* city : std::as_const(customResults_)) {
            // Create copies with this service as parent for proper cleanup
            auto* copy = new CityModel(city->name(), city->displayName(),
                                      city->country(), city->latitude(),
                                      city->longitude(), this);
            results.append(copy);
        }
    } else {
        // Generate mock results based on query
        results = createMockCities(currentQuery_);
    }
    
    if (results.isEmpty()) {
        QString error = QString("No mock cities found for query: %1").arg(currentQuery_);
        Log::warning(error);
        updateStats(false, error);
        emit searchError(error);
    } else {
        Log::info(QString("Returning %1 mock cities").arg(results.size()));
        updateStats(true);
        emit citiesFound(results);
    }
    
    emit searchFinished();
}

void MockCitySearchService::updateStats(bool success, const QString& errorMessage)
{
    if (success) {
        successCount_++;
        lastError_.clear();
    } else {
        failureCount_++;
        lastError_ = errorMessage;
    }
}

QList<CityModel*> MockCitySearchService::createMockCities(const QString& query)
{
    QList<CityModel*> cities;
    QString lowerQuery = query.toLower();
    
    // Predefined mock data
    struct MockCityData {
        QString name;
        QString country;
        double lat;
        double lon;
    };
    
    QList<MockCityData> mockData = {
        {"Berlin", "Germany", 52.5200, 13.4050},
        {"Munich", "Germany", 48.1351, 11.5820},
        {"Hamburg", "Germany", 53.5511, 9.9937},
        {"Cologne", "Germany", 50.9375, 6.9603},
        {"Frankfurt", "Germany", 50.1109, 8.6821},
        {"New York", "United States", 40.7128, -74.0060},
        {"Los Angeles", "United States", 34.0522, -118.2437},
        {"Chicago", "United States", 41.8781, -87.6298},
        {"San Francisco", "United States", 37.7749, -122.4194},
        {"London", "United Kingdom", 51.5074, -0.1278},
        {"Manchester", "United Kingdom", 53.4808, -2.2426},
        {"Birmingham", "United Kingdom", 52.4862, -1.8904},
        {"Paris", "France", 48.8566, 2.3522},
        {"Lyon", "France", 45.7640, 4.8357},
        {"Marseille", "France", 43.2965, 5.3698},
        // Test duplicates (for testing deduplication logic)
        {"Berlin", "Germany", 52.5201, 13.4051}, // Very close coordinates
        {"London", "United Kingdom", 51.5074, -0.1278}, // Exact duplicate
        {"Paris", "France", 48.8566, 2.3522} // Another exact duplicate
    };
    
    // Filter mock data based on query
    for (const auto& data : std::as_const(mockData)) {
        if (data.name.toLower().contains(lowerQuery) || 
            data.country.toLower().contains(lowerQuery) ||
            lowerQuery.contains(data.name.toLower())) {
            
            QString displayName = QString("%1, %2").arg(data.name, data.country);
            auto* city = new CityModel(data.name, displayName, data.country,
                                      data.lat, data.lon, this);
            cities.append(city);
        }
    }
    
    // Add some extra results for duplicate testing if enabled
    if (includeDuplicates_ && lowerQuery.contains("test")) {
        // Add intentional duplicates when searching for "test"
        QString testName = "Test City";
        QString testCountry = "Test Country";
        QString testDisplayName = QString("%1, %2").arg(testName, testCountry);
        
        // Add the same city multiple times with slight variations
        cities.append(new CityModel(testName, testDisplayName, testCountry, 50.0, 10.0, this));
        cities.append(new CityModel(testName, testDisplayName, testCountry, 50.0001, 10.0001, this)); // Close coords
        cities.append(new CityModel(testName, testDisplayName, testCountry, 50.0, 10.0, this)); // Exact duplicate
        cities.append(new CityModel("Test City", "Test City, Test Country", "Test Country", 50.1, 10.1, this)); // Different formatting
    }
    
    // If no specific matches, add some generic results based on query length
    if (cities.isEmpty() && !lowerQuery.isEmpty()) {
        // Generate some generic results
        for (int i = 0; i < qMin(3, lowerQuery.length()); ++i) {
            QString mockName = QString("Mock City %1 (%2)").arg(i + 1).arg(query);
            QString mockCountry = "Mock Country";
            QString displayName = QString("%1, %2").arg(mockName).arg(mockCountry);
            double lat = 50.0 + (i * 0.1);
            double lon = 10.0 + (i * 0.1);
            
            auto* city = new CityModel(mockName, displayName, mockCountry,
                                      lat, lon, this);
            cities.append(city);
        }
    }
    
    Log::debug(QString("Generated %1 mock cities for query: %2")
                      .arg(cities.size())
                      .arg(query));
    
    return cities;
}

bool MockCitySearchService::shouldSimulateError() const
{
    if (!simulateErrors_) {
        return false;
    }
    
    double random = QRandomGenerator::global()->generateDouble();
    return random < errorRate_;
} 
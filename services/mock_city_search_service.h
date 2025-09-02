#pragma once

#include <QObject>
#include <QTimer>
#include <QStringList>
#include "../interfaces/icity_search_service.h"
#include "../models/city_model.h"
#include "../utils/debug_logger.h"

/**
 * @brief Mock implementation of ICitySearchService for testing
 * 
 * This service returns predefined test data and simulates network delays.
 * Useful for unit testing and development when network access is not available.
 */
class MockCitySearchService : public ICitySearchService
{
    Q_OBJECT

public:
    explicit MockCitySearchService(QObject* parent = nullptr);
    
    // ICitySearchService interface implementation
    void searchCities(const QString& query) override;
    void cancelSearch() override;
    [[nodiscard]] bool isSearching() const override;
    
    // Service metadata implementation
    [[nodiscard]] QString serviceName() const override;
    [[nodiscard]] QString serviceVersion() const override;
    [[nodiscard]] QStringList supportedFeatures() const override;
    
    // Optional features implementation
    [[nodiscard]] bool supportsAutoComplete() const override { return true; }
    [[nodiscard]] bool requiresApiKey() const override { return false; }
    [[nodiscard]] int rateLimitPerMinute() const override { return 1000; } // Very high for mock
    [[nodiscard]] QStringList supportedCountries() const override { return {"US", "DE", "FR", "UK"}; }
    [[nodiscard]] QString serviceDescription() const override;
    
    // Service health implementation
    [[nodiscard]] bool isServiceAvailable() const override { return true; }
    [[nodiscard]] QString lastErrorMessage() const override { return lastError_; }
    [[nodiscard]] int successfulRequestsCount() const override { return successCount_; }
    [[nodiscard]] int failedRequestsCount() const override { return failureCount_; }
    
    // Mock-specific configuration
    void setSimulateNetworkDelay(bool enable, int delayMs = 500);
    void setSimulateErrors(bool enable, double errorRate = 0.1); // 10% error rate
    void setCustomResults(const QList<CityModel*>& cities);
    void clearCustomResults();
    
    // Testing utilities
    void setIncludeDuplicatesInResults(bool enable) { includeDuplicates_ = enable; }

private slots:
    void simulateSearchCompleted();

private:
    void updateStats(bool success, const QString& errorMessage = QString());
    QList<CityModel*> createMockCities(const QString& query);
    [[nodiscard]] bool shouldSimulateError() const;
    
    // Mock configuration
    bool simulateDelay_ = true;
    int delayMs_ = 500;
    bool simulateErrors_ = false;
    double errorRate_ = 0.1;
    bool includeDuplicates_ = true; // Include test duplicates by default
    QList<CityModel*> customResults_;
    
    // Current state
    QTimer* delayTimer_;
    bool isSearching_ = false;
    QString currentQuery_;
    
    // Service statistics
    QString lastError_;
    int successCount_ = 0;
    int failureCount_ = 0;
}; 
#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <source_location>
#include "../models/city_model.h"

/**
 * @brief Abstract interface for city search services
 * 
 * This interface allows the CitySearchViewModel to work with different
 * search service implementations (Nominatim, Google Places, etc.) without
 * being coupled to any specific implementation.
 */
class ICitySearchService : public QObject
{
    Q_OBJECT

public:
    explicit ICitySearchService(QObject* parent = nullptr) : QObject(parent) {}
    ~ICitySearchService() override = default;

    // Core search functionality (pure virtual - must be implemented)
    virtual void searchCities(const QString& query) = 0;
    virtual void cancelSearch() = 0;
    [[nodiscard]] virtual bool isSearching() const = 0;

    // Service metadata (pure virtual - must be implemented)
    [[nodiscard]] virtual QString serviceName() const = 0;
    [[nodiscard]] virtual QString serviceVersion() const = 0;
    [[nodiscard]] virtual QStringList supportedFeatures() const = 0;

    // Optional advanced features (with default implementations)
    [[nodiscard]] virtual bool supportsAutoComplete() const { return false; }
    [[nodiscard]] virtual bool requiresApiKey() const { return false; }
    [[nodiscard]] virtual int rateLimitPerMinute() const { return 60; }
    [[nodiscard]] virtual QStringList supportedCountries() const { return {}; } // Empty = all countries
    [[nodiscard]] virtual QString serviceDescription() const { return {}; }

    // Service health and diagnostics
    [[nodiscard]] virtual bool isServiceAvailable() const { return true; }
    [[nodiscard]] virtual QString lastErrorMessage() const { return {}; }
    [[nodiscard]] virtual int successfulRequestsCount() const { return 0; }
    [[nodiscard]] virtual int failedRequestsCount() const { return 0; }

signals:
    // Core signals that all implementations must emit
    void citiesFound(const QList<CityModel*>& cities);
    void searchError(const QString& errorMessage);
    void searchStarted();
    void searchFinished();

    // Optional signals for advanced features
    void serviceStatusChanged(bool available);
    void rateLimitExceeded();
    void apiKeyInvalid();
}; 

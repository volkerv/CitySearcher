#pragma once

#include <QObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <string_view>

#include "nominatim_client.h"
#include "../models/nominatim_search_request.h"
#include "../models/city_model.h"
#include "../interfaces/icity_search_service.h"
#include "../utils/debug_logger.h"

class NominatimService : public ICitySearchService
{
    Q_OBJECT

public:
    explicit NominatimService(QObject* parent = nullptr);
    
    // Inject client dependency (for testing)
    explicit NominatimService(NominatimClient* client, QObject* parent = nullptr);
    
    // ICitySearchService interface implementation
    void searchCities(const QString& query) override;
    void cancelSearch() override;
    bool isSearching() const override;
    
    // Service metadata implementation
    QString serviceName() const override;
    QString serviceVersion() const override;
    QStringList supportedFeatures() const override;
    
    // Optional features implementation
    bool supportsAutoComplete() const override { return false; }
    bool requiresApiKey() const override { return false; }
    int rateLimitPerMinute() const override { return 60; }
    QStringList supportedCountries() const override { return QStringList(); } // All countries
    QString serviceDescription() const override;
    
    // Service health implementation
    bool isServiceAvailable() const override;
    QString lastErrorMessage() const override { return lastError_; }
    int successfulRequestsCount() const override { return successCount_; }
    int failedRequestsCount() const override { return failureCount_; }

    // Note: signals are inherited from ICitySearchService

private slots:
    void onClientSearchCompleted(const QByteArray& jsonData);
    void onClientSearchError(const QString& errorMessage);
    void onClientRequestStarted();
    void onClientRequestFinished();

private:
    void parseJsonResponse(const QByteArray& jsonData);
    CityModel* createCityFromJson(const QJsonObject& cityJson);
    void updateStats(bool success, const QString& errorMessage = QString());
    
    // JSON field constants
    static constexpr std::string_view JSON_DISPLAY_NAME = "display_name";
    static constexpr std::string_view JSON_LATITUDE = "lat";
    static constexpr std::string_view JSON_LONGITUDE = "lon";
    static constexpr std::string_view JSON_ADDRESS = "address";
    static constexpr std::string_view JSON_COUNTRY = "country";
    static constexpr std::string_view JSON_CITY = "city";
    static constexpr std::string_view JSON_TOWN = "town";
    static constexpr std::string_view JSON_VILLAGE = "village";
    static constexpr std::string_view JSON_MUNICIPALITY = "municipality";
    
    // String constants
    static constexpr std::string_view DISPLAY_NAME_SEPARATOR = ", ";
    
    NominatimClient* client_;
    bool ownsClient_;
    
    // Service statistics and health
    QString lastError_;
    int successCount_ = 0;
    int failureCount_ = 0;
}; 
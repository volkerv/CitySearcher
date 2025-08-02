#include "nominatim_service.h"
#include <QDebug>

NominatimService::NominatimService(QObject* parent)
    : ICitySearchService(parent)
    , client_(new NominatimClient(this))
    , ownsClient_(true)
{
    // Connect client signals
    connect(client_, &NominatimClient::searchCompleted,
            this, &NominatimService::onClientSearchCompleted);
    connect(client_, &NominatimClient::searchError,
            this, &NominatimService::onClientSearchError);
    connect(client_, &NominatimClient::requestStarted,
            this, &NominatimService::onClientRequestStarted);
    connect(client_, &NominatimClient::requestFinished,
            this, &NominatimService::onClientRequestFinished);
}

NominatimService::NominatimService(NominatimClient* client, QObject* parent)
    : ICitySearchService(parent)
    , client_(client)
    , ownsClient_(false)
{
    // Connect client signals
    connect(client_, &NominatimClient::searchCompleted,
            this, &NominatimService::onClientSearchCompleted);
    connect(client_, &NominatimClient::searchError,
            this, &NominatimService::onClientSearchError);
    connect(client_, &NominatimClient::requestStarted,
            this, &NominatimService::onClientRequestStarted);
    connect(client_, &NominatimClient::requestFinished,
            this, &NominatimService::onClientRequestFinished);
}

void NominatimService::searchCities(const QString& query)
{
    Log::debug("Starting Nominatim search");
    Log::info(QString("Search query: %1").arg(query));
    
    if (query.trimmed().isEmpty()) {
        QString error = "Please enter a search query";
        Log::error(error);
        updateStats(false, error);
        emit searchError(error);
        return;
    }
    
    NominatimSearchRequest request(query);
    Log::debug("Sending request to Nominatim API");
    client_->searchAsync(request);
}

void NominatimService::cancelSearch()
{
    Log::info("Cancelling Nominatim search");
    client_->cancelCurrentRequest();
}

bool NominatimService::isSearching() const
{
    return client_->isRequestInProgress();
}

void NominatimService::onClientSearchCompleted(const QByteArray& jsonData)
{
    parseJsonResponse(jsonData);
}

void NominatimService::onClientSearchError(const QString& errorMessage)
{
    updateStats(false, errorMessage);
    emit searchError(errorMessage);
}

void NominatimService::onClientRequestStarted()
{
    emit searchStarted();
}

void NominatimService::onClientRequestFinished()
{
    emit searchFinished();
}

void NominatimService::parseJsonResponse(const QByteArray& jsonData)
{
    Log::debug("Parsing Nominatim API response");
    Log::info(QString("Response size: %1 bytes").arg(jsonData.size()));
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        QString error = QString("JSON parse error: %1").arg(parseError.errorString());
        Log::error(error);
        updateStats(false, error);
        emit searchError(error);
        return;
    }

    if (!doc.isArray()) {
        QString error = "Invalid response format";
        Log::error(error);
        updateStats(false, error);
        emit searchError(error);
        return;
    }

    QJsonArray results = doc.array();
    Log::debug(QString("Processing %1 results from API").arg(results.size()));
    QList<CityModel*> cities;

    for (const auto &value : std::as_const(results)) {
        if (!value.isObject())
            continue;

        QJsonObject cityJson = value.toObject();
        CityModel *city = createCityFromJson(cityJson);
        if (city) {
            cities.append(city);
        }
    }

    if (!cities.isEmpty()) {
        // Set parent to this service so cities get cleaned up properly
        for (auto* city : cities) {
            city->setParent(this);
        }
        Log::info(QString("Successfully found %1 cities").arg(cities.size()));
        updateStats(true);
        emit citiesFound(cities);
    } else {
        QString error = "No cities found for your search query";
        Log::warning(error);
        updateStats(false, error);
        emit searchError(error);
    }
}

CityModel* NominatimService::createCityFromJson(const QJsonObject &cityJson)
{
    // Extract basic information
    QString displayName = cityJson[JSON_DISPLAY_NAME.data()].toString();
    double latitude = cityJson[JSON_LATITUDE.data()].toString().toDouble();
    double longitude = cityJson[JSON_LONGITUDE.data()].toString().toDouble();
    
    // Extract address details
    QJsonObject address = cityJson[JSON_ADDRESS.data()].toObject();
    QString cityName;
    QString country = address[JSON_COUNTRY.data()].toString();
    
    // Try to get city name from various fields
    if (address.contains(JSON_CITY.data())) {
        cityName = address[JSON_CITY.data()].toString();
    } else if (address.contains(JSON_TOWN.data())) {
        cityName = address[JSON_TOWN.data()].toString();
    } else if (address.contains(JSON_VILLAGE.data())) {
        cityName = address[JSON_VILLAGE.data()].toString();
    } else if (address.contains(JSON_MUNICIPALITY.data())) {
        cityName = address[JSON_MUNICIPALITY.data()].toString();
    } else {
        // Use the first part of display_name as fallback
        QStringList parts = displayName.split(DISPLAY_NAME_SEPARATOR.data());
        if (!parts.isEmpty()) {
            cityName = parts.first();
        }
    }

    // Only create city if we have essential information
    if (cityName.isEmpty() || displayName.isEmpty()) {
        return nullptr;
    }

    return new CityModel(cityName, displayName, country, latitude, longitude);
}

QString NominatimService::serviceName() const
{
    return "Nominatim";
}

QString NominatimService::serviceVersion() const
{
    return "1.0";
}

QStringList NominatimService::supportedFeatures() const
{
    return {"basic_search", "address_details", "coordinates", "country_filter"};
}

QString NominatimService::serviceDescription() const
{
    return "OpenStreetMap Nominatim geocoding service - free worldwide city search";
}

bool NominatimService::isServiceAvailable() const
{
    // Could implement actual health check here
    // For now, just check if we have a client
    return client_ != nullptr;
}

void NominatimService::updateStats(bool success, const QString& errorMessage)
{
    if (success) {
        successCount_++;
        lastError_.clear();
    } else {
        failureCount_++;
        lastError_ = errorMessage;
    }
} 
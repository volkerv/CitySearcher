#include "city_search_viewmodel.h"
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>

#include "factories/city_search_service_factory.h"

CitySearchViewModel::CitySearchViewModel(QObject *parent)
    : QObject(parent)
    , cityListModel_(new CityListModel(this))
    , isSearching_(false)
{
    // Create default service using factory
    auto service = CitySearchServiceFactory::createService(
        CitySearchServiceFactory::ServiceType::Nominatim, this);
    setSearchService(std::move(service));
}

CitySearchViewModel::CitySearchViewModel(std::unique_ptr<ICitySearchService> service, QObject *parent)
    : QObject(parent)
    , cityListModel_(new CityListModel(this))
    , isSearching_(false)
{
    setSearchService(std::move(service));
}

CityListModel* CitySearchViewModel::cityListModel() const
{
    return cityListModel_;
}

bool CitySearchViewModel::isSearching() const
{
    return isSearching_;
}

QString CitySearchViewModel::errorMessage() const
{
    return errorMessage_;
}

void CitySearchViewModel::setServiceType(const QString& serviceTypeName)
{
    const auto serviceType = CitySearchServiceFactory::serviceTypeFromString(serviceTypeName);
    
    if (!CitySearchServiceFactory::isServiceAvailable(serviceType)) {
        setErrorMessage(QString("Service '%1' is not available").arg(serviceTypeName));
        return;
    }

    // ReSharper disable once CppTooWideScope
    auto newService = CitySearchServiceFactory::createService(serviceType, this);
    if (newService) {
        setSearchService(std::move(newService));
        setErrorMessage(""); // Clear any previous errors
        qDebug() << "Switched to service:" << serviceTypeName;
    } else {
        setErrorMessage(QString("Failed to create service '%1'").arg(serviceTypeName));
    }
}

QString CitySearchViewModel::currentServiceName() const
{
    return searchService_ ? searchService_->serviceName() : "None";
}

QStringList CitySearchViewModel::availableServices()
{
    return CitySearchServiceFactory::availableServices();
}

void CitySearchViewModel::searchCities(const QString &query)
{
    if (!searchService_) {
        setErrorMessage("No search service available");
        return;
    }
    
    // Clear previous results and error message
    cityListModel_->clear();
    setErrorMessage("");
    
    // Delegate to service
    searchService_->searchCities(query);
}

void CitySearchViewModel::clearResults()
{
    cityListModel_->clear();
    setErrorMessage("");
    if (searchService_) {
        searchService_->cancelSearch();
    }
}

void CitySearchViewModel::openCityInBrowser(double latitude, double longitude, const QString& cityName)
{
    // Construct OpenStreetMap URL with coordinates
    // Using zoom level 15 for city-level view
    QString url = QString(OPENSTREETMAP_URL_TEMPLATE)
                  .arg(latitude, 0, 'f', 6)
                  .arg(longitude, 0, 'f', 6);
    
    qDebug() << "Opening city in browser:" << cityName << "at coordinates" << latitude << longitude;
    qDebug() << "URL:" << url;
    
    bool success = QDesktopServices::openUrl(QUrl(url));
    if (!success) {
        qWarning() << "Failed to open URL in browser:" << url;
        setErrorMessage("Failed to open location in browser");
    }
}

QString CitySearchViewModel::serviceDescription() const
{
    return searchService_ ? searchService_->serviceDescription() : "No service";
}

int CitySearchViewModel::successfulRequests() const
{
    return searchService_ ? searchService_->successfulRequestsCount() : 0;
}

int CitySearchViewModel::failedRequests() const
{
    return searchService_ ? searchService_->failedRequestsCount() : 0;
}

void CitySearchViewModel::onServiceCitiesFound(const QList<CityModel*>& cities)
{
    // Transfer ownership of cities to the list model
    QList<CityModel*> citiesCopy;
    for (auto* city : cities) {
        citiesCopy.append(city);
        city->setParent(cityListModel_); // Transfer ownership
    }
    
    cityListModel_->addCities(citiesCopy);
    emit searchCompleted(static_cast<int>(cities.size()));
}

void CitySearchViewModel::onServiceSearchError(const QString& errorMessage)
{
    setErrorMessage(errorMessage);
}

void CitySearchViewModel::onServiceSearchStarted()
{
    setIsSearching(true);
}

void CitySearchViewModel::onServiceSearchFinished()
{
    setIsSearching(false);
}

void CitySearchViewModel::setIsSearching(bool searching)
{
    if (isSearching_ != searching) {
        isSearching_ = searching;
        emit isSearchingChanged();
    }
}

void CitySearchViewModel::setErrorMessage(const QString &message)
{
    if (errorMessage_ != message) {
        errorMessage_ = message;
        emit errorMessageChanged();
    }
}

void CitySearchViewModel::connectServiceSignals()
{
    if (!searchService_) {
        return;
    }
    
    connect(searchService_.get(), &ICitySearchService::citiesFound,
            this, &CitySearchViewModel::onServiceCitiesFound);
    connect(searchService_.get(), &ICitySearchService::searchError,
            this, &CitySearchViewModel::onServiceSearchError);
    connect(searchService_.get(), &ICitySearchService::searchStarted,
            this, &CitySearchViewModel::onServiceSearchStarted);
    connect(searchService_.get(), &ICitySearchService::searchFinished,
            this, &CitySearchViewModel::onServiceSearchFinished);
}

void CitySearchViewModel::setSearchService(std::unique_ptr<ICitySearchService> service)
{
    // Disconnect old service if any
    if (searchService_) {
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto res = searchService_->disconnect(this);
        if (!res)
        {
            qDebug() << "Error disconnecting from search service";
        }
    }
    
    // Set new service
    searchService_ = std::move(service);
    
    // Connect new service signals
    connectServiceSignals();
} 

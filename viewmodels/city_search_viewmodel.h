#pragma once

#include <QObject>
#include <QQmlEngine>
#include <memory>
#include "../models/city_list_model.h"
#include "../models/city_model.h"
#include "../interfaces/icity_search_service.h"

class CitySearchViewModel : public QObject
{
    Q_OBJECT
    
private:
    static constexpr auto OPENSTREETMAP_URL_TEMPLATE = "https://www.openstreetmap.org/#map=15/%1/%2";
    QML_ELEMENT
    
    Q_PROPERTY(CityListModel* cityListModel READ cityListModel CONSTANT)
    Q_PROPERTY(bool isSearching READ isSearching NOTIFY isSearchingChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    explicit CitySearchViewModel(QObject *parent = nullptr);
    
    // Dependency injection constructor (for testing)
    explicit CitySearchViewModel(std::unique_ptr<ICitySearchService> service, QObject *parent = nullptr);
    
    // Service management
    Q_INVOKABLE void setServiceType(const QString& serviceTypeName);
    Q_INVOKABLE [[nodiscard]] QString currentServiceName() const;
    Q_INVOKABLE [[nodiscard]] static QStringList availableServices();

    [[nodiscard]] CityListModel* cityListModel() const;
    [[nodiscard]] bool isSearching() const;
    [[nodiscard]] QString errorMessage() const;

    Q_INVOKABLE void searchCities(const QString &query);
    Q_INVOKABLE void clearResults();
    
    // Open city location in browser
    Q_INVOKABLE void openCityInBrowser(double latitude, double longitude, const QString& cityName = QString());
    
    // Service information (for debugging/monitoring)
    Q_INVOKABLE [[nodiscard]] QString serviceDescription() const;
    Q_INVOKABLE [[nodiscard]] int successfulRequests() const;
    Q_INVOKABLE [[nodiscard]] int failedRequests() const;

signals:
    void isSearchingChanged();
    void errorMessageChanged();
    void searchCompleted(int resultCount);

private slots:
    void onServiceCitiesFound(const QList<CityModel*>& cities);
    void onServiceSearchError(const QString& errorMessage);
    void onServiceSearchStarted();
    void onServiceSearchFinished();

private:
    void setIsSearching(bool searching);
    void setErrorMessage(const QString &message);
    void connectServiceSignals();
    void setSearchService(std::unique_ptr<ICitySearchService> service);

    std::unique_ptr<ICitySearchService> searchService_;
    CityListModel* cityListModel_;
    bool isSearching_;
    QString errorMessage_;
}; 

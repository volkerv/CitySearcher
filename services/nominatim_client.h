#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <string_view>

#include "../models/nominatim_search_request.h"

class NominatimClient : public QObject
{
    Q_OBJECT

public:
    explicit NominatimClient(QObject* parent = nullptr);
    
    // Async search operation
    void searchAsync(const NominatimSearchRequest& request);
    
    // Cancel ongoing request
    void cancelCurrentRequest();
    
    // Check if request is in progress
    bool isRequestInProgress() const;

signals:
    void searchCompleted(const QByteArray& jsonData);
    void searchError(const QString& errorMessage);
    void requestStarted();
    void requestFinished();

private slots:
    void onNetworkReplyFinished();
    void onNetworkError(QNetworkReply::NetworkError error);

private:
    static QUrl buildSearchUrl(const NominatimSearchRequest& request) ;
    void cleanupCurrentRequest();
    
    // API Constants
    static constexpr std::string_view BASE_URL = "https://nominatim.openstreetmap.org/search";
    static constexpr std::string_view USER_AGENT = "CitySearcher Qt App";
    
    // Query parameter names
    static constexpr std::string_view PARAM_QUERY = "q";
    static constexpr std::string_view PARAM_FORMAT = "format";
    static constexpr std::string_view PARAM_ADDRESS_DETAILS = "addressdetails";
    static constexpr std::string_view PARAM_LIMIT = "limit";
    static constexpr std::string_view PARAM_FEATURE_TYPE = "featuretype";
    
    QNetworkAccessManager* networkManager_;
    QNetworkReply* currentReply_;
}; 
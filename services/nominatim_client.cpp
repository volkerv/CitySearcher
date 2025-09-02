#include "nominatim_client.h"
#include <QNetworkRequest>
#include <QDebug>

NominatimClient::NominatimClient(QObject* parent)
    : QObject(parent)
    , networkManager_(new QNetworkAccessManager(this))
    , currentReply_(nullptr)
{
}

void NominatimClient::searchAsync(const NominatimSearchRequest& request)
{
    // Validate request
    if (!request.isValid()) {
        emit searchError(QString("Invalid request: %1").arg(request.validationError()));
        return;
    }
    
    // Cancel any ongoing request
    cancelCurrentRequest();
    
    // Build URL
    const QUrl url = buildSearchUrl(request);
    qDebug() << "Nominatim request URL:" << url.toString();
    
    // Create network request
    QNetworkRequest networkRequest(url);
    networkRequest.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT.data());
    
    // Send request
    currentReply_ = networkManager_->get(networkRequest);
    
    // Connect signals
    connect(currentReply_, &QNetworkReply::finished, 
            this, &NominatimClient::onNetworkReplyFinished);
    connect(currentReply_, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &NominatimClient::onNetworkError);
    
    emit requestStarted();
}

void NominatimClient::cancelCurrentRequest()
{
    if (currentReply_) {
        currentReply_->abort();
        cleanupCurrentRequest();
    }
}

bool NominatimClient::isRequestInProgress() const
{
    return currentReply_ != nullptr;
}

void NominatimClient::onNetworkReplyFinished()
{
    if (!currentReply_) {
        return;
    }
    
    if (currentReply_->error() == QNetworkReply::NoError) {
        const QByteArray data = currentReply_->readAll();
        qDebug() << "Nominatim response size:" << data.size() << "bytes";
        emit searchCompleted(data);
    }
    
    cleanupCurrentRequest();
    emit requestFinished();
}

void NominatimClient::onNetworkError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error)
    
    QString errorMessage = "Unknown network error";
    if (currentReply_) {
        errorMessage = currentReply_->errorString();
    }
    
    qDebug() << "Nominatim network error:" << errorMessage;
    emit searchError(QString("Network error: %1").arg(errorMessage));
    
    cleanupCurrentRequest();
    emit requestFinished();
}

QUrl NominatimClient::buildSearchUrl(const NominatimSearchRequest& request)
{
    QUrl url(BASE_URL.data());
    QUrlQuery query;
    
    query.addQueryItem(PARAM_QUERY.data(), request.query());
    query.addQueryItem(PARAM_FORMAT.data(), request.format());
    query.addQueryItem(PARAM_ADDRESS_DETAILS.data(), request.addressDetailsAsString());
    query.addQueryItem(PARAM_LIMIT.data(), request.limitAsString());
    query.addQueryItem(PARAM_FEATURE_TYPE.data(), request.featureType());
    
    url.setQuery(query);
    return url;
}

void NominatimClient::cleanupCurrentRequest()
{
    if (currentReply_) {
        currentReply_->deleteLater();
        currentReply_ = nullptr;
    }
} 
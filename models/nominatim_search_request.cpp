#include "nominatim_search_request.h"

NominatimSearchRequest::NominatimSearchRequest(const QString& searchQuery)
    : query_(searchQuery)
{
}

void NominatimSearchRequest::setQuery(const QString& query)
{
    query_ = query;
}

void NominatimSearchRequest::setLimit(int limit)
{
    if (limit >= MIN_LIMIT && limit <= MAX_LIMIT) {
        limit_ = limit;
    }
}

void NominatimSearchRequest::setAddressDetails(bool enabled)
{
    addressDetails_ = enabled;
}

void NominatimSearchRequest::setFeatureType(const QString& type)
{
    if (!type.isEmpty()) {
        featureType_ = type;
    }
}

void NominatimSearchRequest::setFormat(const QString& format)
{
    if (!format.isEmpty()) {
        format_ = format;
    }
}

bool NominatimSearchRequest::isValid() const
{
    return !query_.trimmed().isEmpty() 
           && limit_ >= MIN_LIMIT 
           && limit_ <= MAX_LIMIT
           && !format_.isEmpty()
           && !featureType_.isEmpty();
}

QString NominatimSearchRequest::validationError() const
{
    if (query_.trimmed().isEmpty()) {
        return "Query cannot be empty";
    }
    if (limit_ < MIN_LIMIT || limit_ > MAX_LIMIT) {
        return QString("Limit must be between %1 and %2").arg(MIN_LIMIT).arg(MAX_LIMIT);
    }
    if (format_.isEmpty()) {
        return "Format cannot be empty";
    }
    if (featureType_.isEmpty()) {
        return "Feature type cannot be empty";
    }
    return QString(); // Valid
}

QString NominatimSearchRequest::limitAsString() const
{
    return QString::number(limit_);
}

QString NominatimSearchRequest::addressDetailsAsString() const
{
    return addressDetails_ ? "1" : "0";
} 
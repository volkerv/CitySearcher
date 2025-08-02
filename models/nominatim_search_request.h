#pragma once

#include <QString>
#include <string_view>

class NominatimSearchRequest
{
public:
    // Default constructor with sensible defaults
    NominatimSearchRequest() = default;
    
    // Constructor with query
    explicit NominatimSearchRequest(const QString& searchQuery);
    
    // Getters
    QString query() const { return query_; }
    int limit() const { return limit_; }
    bool addressDetails() const { return addressDetails_; }
    QString featureType() const { return featureType_; }
    QString format() const { return format_; }
    
    // Setters with validation
    void setQuery(const QString& query);
    void setLimit(int limit);
    void setAddressDetails(bool enabled);
    void setFeatureType(const QString& type);
    void setFormat(const QString& format);
    
    // Validation
    bool isValid() const;
    QString validationError() const;
    
    // URL building helpers
    QString limitAsString() const;
    QString addressDetailsAsString() const;
    
private:
    // API Configuration Constants
    static constexpr std::string_view DEFAULT_FORMAT = "json";
    static constexpr std::string_view DEFAULT_FEATURE_TYPE = "city";
    static constexpr int DEFAULT_LIMIT = 50;
    static constexpr int MIN_LIMIT = 1;
    static constexpr int MAX_LIMIT = 100;
    
    QString query_;
    int limit_ = DEFAULT_LIMIT;
    bool addressDetails_ = true;
    QString featureType_ = DEFAULT_FEATURE_TYPE.data();
    QString format_ = DEFAULT_FORMAT.data();
}; 
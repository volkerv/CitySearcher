#pragma once

#include <QString>
#include <string_view>

class NominatimSearchRequest
{
public:
    // Default constructor with sensible defaults
    NominatimSearchRequest() = default;
    
    // Constructor with query
    explicit NominatimSearchRequest(QString  searchQuery);
    
    // Getters
    [[nodiscard]] QString query() const { return query_; }
    [[nodiscard]] int limit() const { return limit_; }
    [[nodiscard]] bool addressDetails() const { return addressDetailsEnabled_; }
    [[nodiscard]] QString featureType() const { return featureType_; }
    [[nodiscard]] QString format() const { return format_; }
    
    // Setters with validation
    void setQuery(const QString& query);
    void setLimit(int limit);
    void setAddressDetailsEnabled(bool enabled);
    void setFeatureType(const QString& type);
    void setFormat(const QString& format);
    
    // Validation
    [[nodiscard]] bool isValid() const;
    [[nodiscard]] QString validationError() const;
    
    // URL building helpers
    [[nodiscard]] QString limitAsString() const;
    [[nodiscard]] QString addressDetailsAsString() const;
    
private:
    // API Configuration Constants
    static constexpr std::string_view DEFAULT_FORMAT = "json";
    static constexpr std::string_view DEFAULT_FEATURE_TYPE = "city";
    static constexpr int DEFAULT_LIMIT = 50;
    static constexpr int MIN_LIMIT = 1;
    static constexpr int MAX_LIMIT = 100;
    
    QString query_;
    int limit_ = DEFAULT_LIMIT;
    bool addressDetailsEnabled_ = true;
    QString featureType_ = DEFAULT_FEATURE_TYPE.data();
    QString format_ = DEFAULT_FORMAT.data();
}; 
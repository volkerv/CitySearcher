#include "city_model.h"

#include <utility>

CityModel::CityModel(QObject *parent)
    : QObject(parent)
    , latitude_(0.0)
    , longitude_(0.0)
{
}

CityModel::CityModel(QString name, QString displayName, QString country,
                     const double latitude, const double longitude, QObject *parent)
    : QObject(parent)
    , name_(std::move(name))
    , display_name_(std::move(displayName))
    , country_(std::move(country))
    , latitude_(latitude)
    , longitude_(longitude)
{
}

QString CityModel::name() const
{
    return name_;
}

void CityModel::setName(const QString &name)
{
    if (name_ != name) {
        name_ = name;
        emit nameChanged();
    }
}

QString CityModel::displayName() const
{
    return display_name_;
}

void CityModel::setDisplayName(const QString &displayName)
{
    if (display_name_ != displayName) {
        display_name_ = displayName;
        emit displayNameChanged();
    }
}

QString CityModel::country() const
{
    return country_;
}

void CityModel::setCountry(const QString &country)
{
    if (country_ != country) {
        country_ = country;
        emit countryChanged();
    }
}

double CityModel::latitude() const
{
    return latitude_;
}

void CityModel::setLatitude(double latitude)
{
    if (qAbs(latitude_ - latitude) > 0.000001) {
        latitude_ = latitude;
        emit latitudeChanged();
    }
}

double CityModel::longitude() const
{
    return longitude_;
}

void CityModel::setLongitude(double longitude)
{
    if (qAbs(longitude_ - longitude) > 0.000001) {
        longitude_ = longitude;
        emit longitudeChanged();
    }
}

std::partial_ordering CityModel::operator<=>(const CityModel& other) const
{
    // First compare by display name (case-insensitive)
    if (const auto cmp = display_name_.compare(other.display_name_, Qt::CaseInsensitive); cmp != 0) {
        return cmp < 0 ? std::partial_ordering::less : std::partial_ordering::greater;
    }

    // Then by country
    if (const auto cmp = country_.compare(other.country_); cmp != 0) {
        return cmp < 0 ? std::partial_ordering::less : std::partial_ordering::greater;
    }
    
    // Then by latitude
    if (const auto cmp = latitude_ <=> other.latitude_; cmp != 0) {
        return cmp;
    }
    
    // Finally by longitude
    return longitude_ <=> other.longitude_;
}

bool CityModel::operator==(const CityModel& other) const
{
    // Compare all fields for equality (QObject comparison is not meaningful for data equality)
    return display_name_ == other.display_name_ &&
           name_ == other.name_ &&
           country_ == other.country_ &&
           qAbs(latitude_ - other.latitude_) < 0.000001 &&
           qAbs(longitude_ - other.longitude_) < 0.000001;
} 
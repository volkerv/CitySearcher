#include "city_model.h"

CityModel::CityModel(QObject *parent)
    : QObject(parent)
    , m_latitude(0.0)
    , m_longitude(0.0)
{
}

CityModel::CityModel(const QString &name, const QString &displayName, const QString &country, 
                     double latitude, double longitude, QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_displayName(displayName)
    , m_country(country)
    , m_latitude(latitude)
    , m_longitude(longitude)
{
}

QString CityModel::name() const
{
    return m_name;
}

void CityModel::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

QString CityModel::displayName() const
{
    return m_displayName;
}

void CityModel::setDisplayName(const QString &displayName)
{
    if (m_displayName != displayName) {
        m_displayName = displayName;
        emit displayNameChanged();
    }
}

QString CityModel::country() const
{
    return m_country;
}

void CityModel::setCountry(const QString &country)
{
    if (m_country != country) {
        m_country = country;
        emit countryChanged();
    }
}

double CityModel::latitude() const
{
    return m_latitude;
}

void CityModel::setLatitude(double latitude)
{
    if (qAbs(m_latitude - latitude) > 0.000001) {
        m_latitude = latitude;
        emit latitudeChanged();
    }
}

double CityModel::longitude() const
{
    return m_longitude;
}

void CityModel::setLongitude(double longitude)
{
    if (qAbs(m_longitude - longitude) > 0.000001) {
        m_longitude = longitude;
        emit longitudeChanged();
    }
}

std::partial_ordering CityModel::operator<=>(const CityModel& other) const
{
    // First compare by display name (case-insensitive)
    if (auto cmp = m_displayName.toLower().compare(other.m_displayName.toLower()); cmp != 0) {
        return cmp < 0 ? std::partial_ordering::less : std::partial_ordering::greater;
    }
    
    // Then by country
    if (auto cmp = m_country.compare(other.m_country); cmp != 0) {
        return cmp < 0 ? std::partial_ordering::less : std::partial_ordering::greater;
    }
    
    // Then by latitude
    if (auto cmp = m_latitude <=> other.m_latitude; cmp != 0) {
        return cmp;
    }
    
    // Finally by longitude
    return m_longitude <=> other.m_longitude;
}

bool CityModel::operator==(const CityModel& other) const
{
    // Compare all fields for equality (QObject comparison is not meaningful for data equality)
    return m_displayName == other.m_displayName &&
           m_name == other.m_name &&
           m_country == other.m_country &&
           qAbs(m_latitude - other.m_latitude) < 0.000001 &&
           qAbs(m_longitude - other.m_longitude) < 0.000001;
} 
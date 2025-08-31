#include "city_list_model.h"
#include <algorithm>
#include <ranges>
#include <QDebug>
#include "../concepts/service_concepts.h"

CityListModel::CityListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int CityListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(m_cities.size());
}

QVariant CityListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_cities.size())
        return {};

    const CityModel *city = m_cities.at(index.row());

    switch (role) {
    case NameRole:
        return city->name();
    case DisplayNameRole:
        return city->displayName();
    case CountryRole:
        return city->country();
    case LatitudeRole:
        return city->latitude();
    case LongitudeRole:
        return city->longitude();
    default:
        return {};
    }
}

QHash<int, QByteArray> CityListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[DisplayNameRole] = "displayName";
    roles[CountryRole] = "country";
    roles[LatitudeRole] = "latitude";
    roles[LongitudeRole] = "longitude";
    return roles;
}

void CityListModel::addCity(CityModel *city)
{
    if (!city)
        return;

    // Check for duplicates before adding
    for (const CityModel* existingCity : std::as_const(m_cities)) {
        if (isDuplicate(city, existingCity)) {
            qDebug() << "CityListModel: Skipping duplicate city:" << city->displayName();
            city->deleteLater(); // Clean up the duplicate
            return;
        }
    }

    beginInsertRows(QModelIndex(), static_cast<int>(m_cities.size()), static_cast<int>(m_cities.size()));
    m_cities.append(city);
    city->setParent(this);
    endInsertRows();
    
    sortCities();
}

void CityListModel::clear()
{
    if (m_cities.isEmpty())
        return;

    beginResetModel();
    qDeleteAll(m_cities);
    m_cities.clear();
    endResetModel();
}

int CityListModel::count() const
{
    return static_cast<int>(m_cities.size());
}

void CityListModel::addCities(std::span<CityModel* const> cities)
{
    if (cities.empty())
        return;

    // Filter out duplicates before adding using ranges
    QList<CityModel*> uniqueCities = filterDuplicates(cities);
    
    if (uniqueCities.isEmpty())
        return;

    beginInsertRows(QModelIndex(), static_cast<int>(m_cities.size()), static_cast<int>(m_cities.size() + uniqueCities.size() - 1));
    for (CityModel *city : uniqueCities) {
        m_cities.append(city);
        city->setParent(this);
    }
    endInsertRows();
    
    sortCities();
}

void CityListModel::sortCities()
{
    beginResetModel();
    
    auto compareCities = []<CityLike T>(const T* a, const T* b) {
        return a->displayName().toLower() < b->displayName().toLower();
    };
    
    std::ranges::sort(m_cities, compareCities);
    endResetModel();
}

QList<CityModel*> CityListModel::filterDuplicates(std::span<CityModel* const> cities)
{
    QList<CityModel*> uniqueCities;
    int duplicatesRemoved = 0;
    
    // Use ranges to filter out null pointers first
    auto validCities = cities | std::ranges::views::filter([](const auto* city) {
        return city != nullptr; 
    });
    
    for (CityModel* newCity : validCities) {
        bool isDuplicateFound = false;
        
        // Check against existing cities using ranges
        if (std::ranges::any_of(m_cities, [newCity](const auto* existing) {
            return isDuplicate(newCity, existing);
        })) {
            isDuplicateFound = true;
        }
        
        // Check against cities we're about to add using ranges
        if (!isDuplicateFound && std::ranges::any_of(uniqueCities, [newCity](const auto* other) {
            return isDuplicate(newCity, other);
        })) {
            isDuplicateFound = true;
        }
        
        if (!isDuplicateFound) {
            uniqueCities.append(newCity);
        } else {
            duplicatesRemoved++;
            // Delete the duplicate city since we're not using it
            newCity->deleteLater();
        }
    }
    
    if (duplicatesRemoved > 0) {
        qDebug() << "CityListModel: Filtered out" << duplicatesRemoved << "duplicate cities";
    }
    
    return uniqueCities;
}

bool CityListModel::isDuplicate(const CityModel* newCity, const CityModel* existingCity)
{
    if (!newCity || !existingCity) {
        return false;
    }
    
    // Check for exact display name match (most common duplicate case)
    if (newCity->displayName().compare(existingCity->displayName(), Qt::CaseInsensitive) == 0) {
        return true;
    }
    
    // Check for same city name and country (handles different formatting)
    if (newCity->name().compare(existingCity->name(), Qt::CaseInsensitive) == 0 &&
        newCity->country().compare(existingCity->country(), Qt::CaseInsensitive) == 0) {
        return true;
    }
    
    // Check for very close coordinates (within ~100 meters)
    // This catches cases where the same location has slightly different names or formatting
    if (areCoordinatesClose(newCity->latitude(), newCity->longitude(),
                           existingCity->latitude(), existingCity->longitude())) {
        return true;
    }
    
    return false;
}

bool CityListModel::areCoordinatesClose(const double lat1, const double lon1, const double lat2, const double lon2)
{
    // Use a simple distance threshold for duplicate detection
    // Approximately 0.001 degrees = ~100 meters
    constexpr double COORDINATE_THRESHOLD = 0.001;
    
    double latDiff = qAbs(lat1 - lat2);
    double lonDiff = qAbs(lon1 - lon2);
    
    return (latDiff < COORDINATE_THRESHOLD && lonDiff < COORDINATE_THRESHOLD);
}

bool CityListModel::containsDuplicates(std::span<CityModel* const> cities) const
{
    // Use ranges to check for duplicates efficiently
    return std::ranges::any_of(cities, [this](const auto* newCity) {
        return newCity && std::ranges::any_of(m_cities, [newCity](const auto* existing) {
            return isDuplicate(newCity, existing);
        });
    });
} 

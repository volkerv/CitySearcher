#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>
#include <span>
#include <ranges>
#include "city_model.h"
#include "../concepts/service_concepts.h"

class CityListModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum CityRoles {
        NameRole = Qt::UserRole + 1,
        DisplayNameRole,
        CountryRole,
        LatitudeRole,
        LongitudeRole
    };

    explicit CityListModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Methods for managing cities
    Q_INVOKABLE void addCity(CityModel *city);
    Q_INVOKABLE void clear();
    Q_INVOKABLE int count() const;

    // Add cities from search results - C++20 std::span version
    void addCities(std::span<CityModel* const> cities);
    
    // Legacy overload for QList compatibility
    void addCities(const QList<CityModel*> &cities) {
        addCities(std::span<CityModel* const>{cities});
    }

private:
    void sortCities();
    QList<CityModel*> filterDuplicates(std::span<CityModel* const> cities);
    bool isDuplicate(const CityModel* newCity, const CityModel* existingCity) const;
    bool areCoordinatesClose(double lat1, double lon1, double lat2, double lon2) const;
    bool containsDuplicates(std::span<CityModel* const> cities) const;
    
    QList<CityModel*> m_cities;
}; 
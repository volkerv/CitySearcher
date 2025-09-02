#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>
#include <span>
#include <ranges>
#include "city_model.h"

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
    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    // Methods for managing cities
    Q_INVOKABLE void addCity(CityModel *city);
    Q_INVOKABLE void clear();
    Q_INVOKABLE [[nodiscard]] int count() const;

    void addCities(std::span<CityModel* const> cities);

    void addCities(const QList<CityModel*> &cities) {
        addCities(std::span{cities});
    }

private:
    void sortCities();
    QList<CityModel*> filterDuplicates(std::span<CityModel* const> cities);
    static bool isDuplicate(const CityModel* newCity, const CityModel* existingCity) ;
    static bool areCoordinatesClose(double lat1, double lon1, double lat2, double lon2) ;
    [[nodiscard]] bool containsDuplicates(std::span<CityModel* const> cities) const;
    
    QList<CityModel*> cities_;
}; 
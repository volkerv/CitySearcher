#pragma once

#include <QObject>
#include <QString>
#include <QDebug>
#include <compare>

class CityModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
    Q_PROPERTY(QString country READ country WRITE setCountry NOTIFY countryChanged)
    Q_PROPERTY(double latitude READ latitude WRITE setLatitude NOTIFY latitudeChanged)
    Q_PROPERTY(double longitude READ longitude WRITE setLongitude NOTIFY longitudeChanged)

public:
    explicit CityModel(QObject *parent = nullptr);
    CityModel(QString name, QString displayName, QString country,
              double latitude, double longitude, QObject *parent = nullptr);

    QString name() const;
    void setName(const QString &name);

    QString displayName() const;
    void setDisplayName(const QString &displayName);

    QString country() const;
    void setCountry(const QString &country);

    double latitude() const;
    void setLatitude(double latitude);

    double longitude() const;
    void setLongitude(double longitude);

    std::partial_ordering operator<=>(const CityModel& other) const;
    bool operator==(const CityModel& other) const;

signals:
    void nameChanged();
    void displayNameChanged();
    void countryChanged();
    void latitudeChanged();
    void longitudeChanged();

private:
    QString name_;
    QString display_name_;
    QString country_;
    double latitude_;
    double longitude_;
}; 
#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QDebug>
#include "../models/city_list_model.h"
#include "../models/city_model.h"

class TestCityListModel : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic functionality tests
    void testAddSingleCity();
    void testAddMultipleCities();
    void testClearCities();
    void testSorting();

    // Deduplication tests
    void testExactDuplicateFiltering();
    void testCoordinateProximityDuplicateFiltering();
    void testMixedDuplicateFiltering();
    void testNoDuplicatesWhenUnique();
    void testCoordinateThreshold();

    // Edge cases
    void testNullPointerHandling();
    void testEmptyLists();
    void testSingleItemList();
    void testSelfDuplication();

    // Performance and memory tests
    void testMemoryCleanupOnDuplicates();
    void testLargeDatasetDeduplication();

private:
    CityListModel* model;
    
    // Helper methods
    CityModel* createTestCity(const QString& name, const QString& country, 
                             double lat, double lon, const QString& displayName = QString());
    void verifyModelContains(const QString& expectedName, const QString& expectedCountry);
    void verifyModelDoesNotContain(const QString& name, const QString& country);
    int countCitiesWithName(const QString& name);
};

void TestCityListModel::initTestCase()
{
    qDebug() << "Starting CityListModel tests";
}

void TestCityListModel::cleanupTestCase()
{
    qDebug() << "Finished CityListModel tests";
}

void TestCityListModel::init()
{
    model = new CityListModel(this);
}

void TestCityListModel::cleanup()
{
    delete model;
    model = nullptr;
}

CityModel* TestCityListModel::createTestCity(const QString& name, const QString& country, 
                                           double lat, double lon, const QString& displayName)
{
    QString display = displayName.isEmpty() ? QString("%1, %2").arg(name, country) : displayName;
    return new CityModel(name, display, country, lat, lon);
}

void TestCityListModel::verifyModelContains(const QString& expectedName, const QString& expectedCountry)
{
    bool found = false;
    for (int i = 0; i < model->rowCount(); ++i) {
        QModelIndex index = model->index(i);
        QString name = model->data(index, CityListModel::NameRole).toString();
        QString country = model->data(index, CityListModel::CountryRole).toString();
        
        if (name == expectedName && country == expectedCountry) {
            found = true;
            break;
        }
    }
    QVERIFY2(found, QString("City %1, %2 not found in model").arg(expectedName, expectedCountry).toUtf8());
}

void TestCityListModel::verifyModelDoesNotContain(const QString& name, const QString& country)
{
    for (int i = 0; i < model->rowCount(); ++i) {
        QModelIndex index = model->index(i);
        QString modelName = model->data(index, CityListModel::NameRole).toString();
        QString modelCountry = model->data(index, CityListModel::CountryRole).toString();
        
        QVERIFY2(!(modelName == name && modelCountry == country), 
                QString("City %1, %2 unexpectedly found in model").arg(name, country).toUtf8());
    }
}

int TestCityListModel::countCitiesWithName(const QString& name)
{
    int count = 0;
    for (int i = 0; i < model->rowCount(); ++i) {
        QModelIndex index = model->index(i);
        QString modelName = model->data(index, CityListModel::NameRole).toString();
        if (modelName == name) {
            count++;
        }
    }
    return count;
}

void TestCityListModel::testAddSingleCity()
{
    QCOMPARE(model->rowCount(), 0);
    
    CityModel* city = createTestCity("Berlin", "Germany", 52.52, 13.405);
    model->addCity(city);
    
    QCOMPARE(model->rowCount(), 1);
    QCOMPARE(model->count(), 1);
    
    verifyModelContains("Berlin", "Germany");
}

void TestCityListModel::testAddMultipleCities()
{
    QList<CityModel*> cities;
    cities.append(createTestCity("Berlin", "Germany", 52.52, 13.405));
    cities.append(createTestCity("Paris", "France", 48.8566, 2.3522));
    cities.append(createTestCity("London", "United Kingdom", 51.5074, -0.1278));
    
    model->addCities(cities);
    
    QCOMPARE(model->rowCount(), 3);
    verifyModelContains("Berlin", "Germany");
    verifyModelContains("Paris", "France");
    verifyModelContains("London", "United Kingdom");
}

void TestCityListModel::testClearCities()
{
    // Add some cities first
    QList<CityModel*> cities;
    cities.append(createTestCity("Berlin", "Germany", 52.52, 13.405));
    cities.append(createTestCity("Paris", "France", 48.8566, 2.3522));
    model->addCities(cities);
    
    QCOMPARE(model->rowCount(), 2);
    
    model->clear();
    
    QCOMPARE(model->rowCount(), 0);
    QCOMPARE(model->count(), 0);
}

void TestCityListModel::testSorting()
{
    QList<CityModel*> cities;
    cities.append(createTestCity("Zurich", "Switzerland", 47.3769, 8.5417));
    cities.append(createTestCity("Amsterdam", "Netherlands", 52.3676, 4.9041));
    cities.append(createTestCity("Berlin", "Germany", 52.52, 13.405));
    
    model->addCities(cities);
    
    // Check alphabetical order
    QModelIndex first = model->index(0);
    QModelIndex second = model->index(1);
    QModelIndex third = model->index(2);
    
    QString firstName = model->data(first, CityListModel::DisplayNameRole).toString();
    QString secondName = model->data(second, CityListModel::DisplayNameRole).toString();
    QString thirdName = model->data(third, CityListModel::DisplayNameRole).toString();
    
    QVERIFY(firstName.toLower() < secondName.toLower());
    QVERIFY(secondName.toLower() < thirdName.toLower());
}

void TestCityListModel::testExactDuplicateFiltering()
{
    QList<CityModel*> cities;
    cities.append(createTestCity("Berlin", "Germany", 52.52, 13.405));
    cities.append(createTestCity("Berlin", "Germany", 52.52, 13.405)); // Exact duplicate
    cities.append(createTestCity("Paris", "France", 48.8566, 2.3522));
    
    model->addCities(cities);
    
    // Should only have 2 cities (duplicate filtered out)
    QCOMPARE(model->rowCount(), 2);
    QCOMPARE(countCitiesWithName("Berlin"), 1);
    
    verifyModelContains("Berlin", "Germany");
    verifyModelContains("Paris", "France");
}

void TestCityListModel::testCoordinateProximityDuplicateFiltering()
{
    QList<CityModel*> cities;
    cities.append(createTestCity("Berlin", "Germany", 52.5200, 13.4050));
    cities.append(createTestCity("Berlin Center", "Germany", 52.5201, 13.4051)); // Within 0.001° threshold
    cities.append(createTestCity("Hamburg", "Germany", 53.5511, 9.9937)); // Different city, should remain
    
    model->addCities(cities);
    
    // The coordinate threshold is 0.001°
    // Berlin Center (52.5201, 13.4051) is very close to Berlin (52.5200, 13.4050) - should be filtered
    // Hamburg is a different city entirely - should remain
    // Result: Berlin (1) + Hamburg (1) = 2 cities
    QCOMPARE(model->rowCount(), 2);
}

void TestCityListModel::testMixedDuplicateFiltering()
{
    QList<CityModel*> cities;
    
    // Various types of duplicates mixed with unique cities
    cities.append(createTestCity("Berlin", "Germany", 52.52, 13.405));
    cities.append(createTestCity("Berlin", "Germany", 52.52, 13.405)); // Exact duplicate
    cities.append(createTestCity("Paris", "France", 48.8566, 2.3522));
    cities.append(createTestCity("Berlin", "Germany", 52.5201, 13.4051)); // Close coordinates
    cities.append(createTestCity("London", "United Kingdom", 51.5074, -0.1278));
    cities.append(createTestCity("paris", "france", 48.857, 2.353, "Paris, France")); // Case difference, close coords
    
    model->addCities(cities);
    
    // Should have Berlin, Paris, London (3 unique cities)
    QCOMPARE(model->rowCount(), 3);
    QCOMPARE(countCitiesWithName("Berlin"), 1);
    QCOMPARE(countCitiesWithName("Paris"), 1);
    QCOMPARE(countCitiesWithName("London"), 1);
}

void TestCityListModel::testNoDuplicatesWhenUnique()
{
    QList<CityModel*> cities;
    cities.append(createTestCity("Berlin", "Germany", 52.52, 13.405));
    cities.append(createTestCity("Berlin", "United States", 44.4759, -71.1878)); // Different country
    cities.append(createTestCity("Paris", "France", 48.8566, 2.3522));
    cities.append(createTestCity("Paris", "United States", 36.3020, -88.3267)); // Different country
    
    model->addCities(cities);
    
    // All should be unique (different countries)
    QCOMPARE(model->rowCount(), 4);
}

void TestCityListModel::testCoordinateThreshold()
{
    QList<CityModel*> cities;
    cities.append(createTestCity("Test1", "Country", 50.0, 10.0));
    cities.append(createTestCity("Test2", "Country", 50.0009, 10.0009)); // Just under threshold (0.001)
    cities.append(createTestCity("Test3", "Country", 50.0011, 10.0011)); // Just over threshold
    
    model->addCities(cities);
    
    // Test2 should be filtered (under threshold), Test3 should remain (over threshold)
    QCOMPARE(model->rowCount(), 2);
}

void TestCityListModel::testNullPointerHandling()
{
    // Test adding null pointer
    model->addCity(nullptr);
    QCOMPARE(model->rowCount(), 0);
    
    // Test adding list with null pointers mixed with valid cities
    QList<CityModel*> cities;
    
    // Create cities normally (they will be parented to this test object)
    auto* city1 = createTestCity("Berlin", "Germany", 52.52, 13.405);
    auto* city2 = createTestCity("Paris", "France", 48.8566, 2.3522);
    
    cities.append(city1);
    cities.append(nullptr);  // This should be ignored
    cities.append(city2);
    
    // Note: addCities expects the cities to not have parents initially
    // so let's set them to nullptr temporarily
    city1->setParent(nullptr);
    city2->setParent(nullptr);
    
    model->addCities(cities);
    
    // Should have 2 valid cities (null ignored)
    QCOMPARE(model->rowCount(), 2);
    
    // Verify the cities are in the model
    verifyModelContains("Berlin", "Germany");
    verifyModelContains("Paris", "France");
}

void TestCityListModel::testEmptyLists()
{
    QList<CityModel*> emptyCities;
    model->addCities(emptyCities);
    
    QCOMPARE(model->rowCount(), 0);
}

void TestCityListModel::testSingleItemList()
{
    QList<CityModel*> cities;
    cities.append(createTestCity("Berlin", "Germany", 52.52, 13.405));
    
    model->addCities(cities);
    
    QCOMPARE(model->rowCount(), 1);
    verifyModelContains("Berlin", "Germany");
}

void TestCityListModel::testSelfDuplication()
{
    // Add a city first
    CityModel* city1 = createTestCity("Berlin", "Germany", 52.52, 13.405);
    model->addCity(city1);
    
    QCOMPARE(model->rowCount(), 1);
    
    // Try to add a duplicate
    CityModel* city2 = createTestCity("Berlin", "Germany", 52.52, 13.405);
    model->addCity(city2);
    
    // Should still have only one
    QCOMPARE(model->rowCount(), 1);
}

void TestCityListModel::testMemoryCleanupOnDuplicates()
{
    // This test verifies that duplicate cities are properly deleted
    // We can't directly test memory, but we can verify the behavior
    
    QList<CityModel*> cities;
    cities.append(createTestCity("Berlin", "Germany", 52.52, 13.405));
    cities.append(createTestCity("Berlin", "Germany", 52.52, 13.405)); // Duplicate
    cities.append(createTestCity("Berlin", "Germany", 52.52, 13.405)); // Another duplicate
    
    // All cities in the list have no parent initially
    for (auto* city : cities) {
        QVERIFY(city->parent() == nullptr);
    }
    
    model->addCities(cities);
    
    // Should only have one city in the model
    QCOMPARE(model->rowCount(), 1);
    
    // The remaining city should have the model as parent
    QModelIndex index = model->index(0);
    QString name = model->data(index, CityListModel::NameRole).toString();
    QCOMPARE(name, QString("Berlin"));
}

void TestCityListModel::testLargeDatasetDeduplication()
{
    QList<CityModel*> cities;
    
    // Create a large dataset with many duplicates
    for (int i = 0; i < 100; ++i) {
        cities.append(createTestCity("Berlin", "Germany", 52.52, 13.405)); // Many duplicates
        if (i % 10 == 0) {
            cities.append(createTestCity(QString("City%1").arg(i), "Country", 50.0 + i*0.01, 10.0 + i*0.01));
        }
    }
    
    // Add a few unique cities
    cities.append(createTestCity("Paris", "France", 48.8566, 2.3522));
    cities.append(createTestCity("London", "United Kingdom", 51.5074, -0.1278));
    
    model->addCities(cities);
    
    // Should have much fewer cities than added (due to deduplication)
    QVERIFY(model->rowCount() < 20); // Should be around 13 unique cities
    QCOMPARE(countCitiesWithName("Berlin"), 1); // Only one Berlin despite 100 duplicates
}

// Register the test class
QTEST_MAIN(TestCityListModel)
#include "test_city_list_model.moc" 

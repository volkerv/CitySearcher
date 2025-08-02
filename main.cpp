#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QDebug>
#include "viewmodels/city_search_viewmodel.h"
#include "models/city_list_model.h"
#include "models/city_model.h"
#include "utils/debug_logger.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Configure DebugLogger
    // Set to Info level for production, Debug for development
    Log::setLogLevel(Log::LogLevel::Debug);
    Log::setIncludeSourceLocation(true);
    Log::setEnabled(true);
    
    // Demonstrate the new logging system
    Log::info("CitySearcher application starting");
    Log::debug("Debug logging is enabled");

    // Load translations
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    qDebug() << "System UI languages:" << uiLanguages;
    
    bool translationLoaded = false;
    for (const QString &locale : uiLanguages) {
        QLocale qlocale(locale);
        
        // Try different variants: full locale, language+country, just language
        QStringList candidates;
        candidates << "citysearcher_" + qlocale.name();           // e.g., citysearcher_de_DE
        candidates << "citysearcher_" + qlocale.name().split('_').first(); // e.g., citysearcher_de
        
        for (const QString &baseName : candidates) {
            const QString translationPath = ":/i18n/" + baseName;
            qDebug() << "Trying to load translation:" << translationPath;
            
            if (translator.load(translationPath)) {
                qDebug() << "Successfully loaded translation:" << translationPath;
                app.installTranslator(&translator);
                translationLoaded = true;
                break;
            } else {
                qDebug() << "Failed to load translation:" << translationPath;
            }
        }
        
        if (translationLoaded) break;
    }
    
    if (!translationLoaded) {
        Log::info("No translations loaded, using default English");
    } else {
        Log::info("Translations loaded successfully");
    }

    // Register QML types
    Log::debug("Registering QML types");
    qmlRegisterType<CitySearchViewModel>("CitySearcher", 1, 0, "CitySearchViewModel");
    qmlRegisterType<CityListModel>("CitySearcher", 1, 0, "CityListModel");
    qmlRegisterType<CityModel>("CitySearcher", 1, 0, "CityModel");

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { 
            Log::error("QML object creation failed");
            QCoreApplication::exit(-1); 
        },
        Qt::QueuedConnection);
    
    Log::debug("Loading QML module");
    engine.loadFromModule("CitySearcher", "Main");

    Log::info("Application started successfully");
    return app.exec();
}

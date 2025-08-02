# City Searcher

A Qt6/QML desktop application for searching cities worldwide using the OpenStreetMap Nominatim API. 

&nbsp;
## Features

### **City Search**
- **OpenStreetMap Nominatim API**
- **Global coverage** - search cities worldwide
- **Detailed results** showing city name, country, and coordinates

### **Interactive Results**
- **Click-to-view** - Click any city to open its location in your default web browser
- **Precise coordinates** - Navigate to exact city locations with 15x zoom level
- **Visual feedback** - Hover and click states for better user experience


&nbsp;
## **User Interface**

### **Main Features:**
- **Search Input**: Type any city name to search globally
- **Results List**: Scrollable list of found cities with detailed information
- **Interactive Items**: Click any city result to open its location in OpenStreetMap

### **Search Results Display:**
```
Berlin
Berlin, Brandenburg, Deutschland
Coordinates: 52.5200, 13.4050 • Click to open in map
```

Each result shows:
- **City Name** (bold, primary text)
- **Full Display Name** (including region/country details)
- **Precise Coordinates** (latitude/longitude with 4 decimal precision)

### **Browser Integration:**
- **Automatic Opening**: Clicks open your system's default web browser
- **OpenStreetMap URLs**: Direct links to city locations on openstreetmap.org
- **Optimal Zoom**: 15x zoom level for perfect city-level viewing

&nbsp;
## **Usage**

### **Basic Search:**
1. Launch the application
2. Type a city name in the search box
3. Browse the filtered, unique results
4. **Click any city** to view its location on OpenStreetMap

&nbsp;
## **Architecture**

### Service Layer Architecture

```
┌─────────────────────┐    ┌──────────────────────┐
│  CitySearchViewModel│────│  ICitySearchService  │ (Abstract Interface)
│                     │    │  (Pure Virtual)      │
└─────────────────────┘    └──────────────────────┘
                                       ▲
                           ┌───────────┴───────────┐
                           │                       │
               ┌─────────────────────┐  ┌──────────────────────┐
               │  NominatimService   │  │ MockCitySearchService│
               │  (Implementation)   │  │ (Testing)            │
               └─────────────────────┘  └──────────────────────┘
                           │                       │
               ┌─────────────────────┐  ┌──────────────────────┐
               │  NominatimClient    │  │   Timer Simulation   │
               └─────────────────────┘  └──────────────────────┘
```

### Key Components

- **`ICitySearchService`**: Abstract interface defining the contract for all search services
- **`CitySearchServiceFactory`**: Factory pattern for creating service instances
- **`NominatimService`**: OpenStreetMap Nominatim implementation
- **`MockCitySearchService`**: Testing implementation with configurable behavior
- **`CitySearchViewModel`**: Presentation layer that works with any service implementation

&nbsp;
## **Project Structure**

```
CitySearcher/
├── interfaces/                      # Abstract interfaces
│   └── icity_search_service.h
├── factories/                       # Service creation
│   ├── city_search_service_factory.h/cpp
├── services/                        # Service implementations
│   ├── nominatim_service.h/cpp        # Nominatim implementation
│   ├── nominatim_client.h/cpp         # Network layer
│   └── mock_city_search_service.h/cpp # Testing implementation
├── models/                          # Data models
│   ├── city_model.h/cpp
│   ├── city_list_model.h/cpp
│   └── nominatim_search_request.h/cpp
├── viewmodels/                      # Viewmodels
│   ├── city_search_viewmodel.h/cpp  # Presentation logic
├── main.cpp                         # Application entry point
├── Main.qml                         # QML UI
└── translations/                    # Internationalization
    └── citysearcher_de.ts
```

&nbsp;
## **Building**

### Prerequisites

- **Qt 6.8+** with Quick and Network modules
- **CMake 3.16+**
- **C++20 compatible compiler**

&nbsp;
### macOS with Homebrew

```bash
# Install Qt6
brew install qt6

# Configure build
export OPENGL_INCLUDE_DIR="/System/Library/Frameworks/OpenGL.framework/Headers"
export OPENGL_gl_LIBRARY="/System/Library/Frameworks/OpenGL.framework/OpenGL"
export CMAKE_PREFIX_PATH="$(brew --prefix qt6)"
cmake -B build -S .

# Build
cmake --build build

# Run
./build/appCitySearcher.app/Contents/MacOS/appCitySearcher
```

&nbsp;
### Generic Linux/Windows

```bash
# Make sure Qt6 is in your PATH
cmake -B build -S .
cmake --build build
./build/appCitySearcher  # Linux
./build/appCitySearcher.exe  # Windows
```

## **Usage**

### Service Management (Programmatic)

The application supports switching between different search services:

```cpp
// Switch to Mock service for testing
viewModel->setServiceType("Mock");

// Get current service information
QString currentService = viewModel->currentServiceName();
QStringList availableServices = viewModel->availableServices();
QString description = viewModel->serviceDescription();

// Monitor service statistics
int successCount = viewModel->successfulRequests();
int failureCount = viewModel->failedRequests();
```

### Mock Service Configuration

For testing and development, the Mock service provides configurable behavior:

```cpp
auto mockService = CitySearchServiceFactory::createService(
    CitySearchServiceFactory::ServiceType::Mock);
    
auto* mock = static_cast<MockCitySearchService*>(mockService.get());

// Configure network simulation
mock->setSimulateNetworkDelay(true, 1000);  // 1 second delay
mock->setSimulateErrors(true, 0.1);         // 10% error rate

// Configure duplicate testing
mock->setIncludeDuplicatesInResults(true);  // Include test duplicates

// Set custom test results
QList<CityModel*> customCities = { /* ... */ };
mock->setCustomResults(customCities);
```

&nbsp;
## **Available Services**

### Nominatim (Default)
- **Provider**: OpenStreetMap
- **API Key**: Not required
- **Rate Limit**: 60 requests/minute
- **Coverage**: Worldwide
- **Features**: Basic search, coordinates, country information

### Mock (Testing)
- **Purpose**: Testing and development
- **Features**: Configurable delays, error simulation, custom results
- **Data**: Predefined city database with major cities
- **Behavior**: Simulates real network conditions

### Future Services
The architecture supports easy addition of new services:
- Google Places API
- OpenCage Geocoding
- Bing Maps API

&nbsp;
## **Internationalization**

The application supports German translations:

```bash
# Update translations
lupdate . -ts translations/citysearcher_de.ts

# Edit translations
linguist translations/citysearcher_de.ts

# Compile translations (automatic during build)
lrelease translations/citysearcher_de.ts
```

**Language Detection**: The application automatically detects system locale and loads appropriate translations.

&nbsp;
## **Development**

### Adding a New Search Service

1. **Implement Interface**: Create a new class inheriting from `ICitySearchService`
2. **Update Factory**: Add the new service type to `CitySearchServiceFactory`
3. **Register Service**: Update the factory's service creation methods
4. **Test Integration**: Use the Mock service as a template

Example:
```cpp
class GooglePlacesService : public ICitySearchService {
    // Implement pure virtual methods
    void searchCities(const QString& query) override;
    QString serviceName() const override { return "GooglePlaces"; }
    // ... other required methods
};
```

### Testing

The Mock service provides comprehensive testing capabilities:

```cpp
// Unit test example
auto mockService = std::make_unique<MockCitySearchService>();
mockService->setSimulateNetworkDelay(false);  // Instant results
mockService->setSimulateErrors(false);        // No errors

CitySearchViewModel viewModel(std::move(mockService));
// Test viewModel functionality...
```

### Code Style

- **Modern C++**: Uses C++20 features (constexpr, std::string_view, concepts)
- **Qt Conventions**: Follows Qt naming and design patterns
- **RAII**: Proper resource management with smart pointers
- **const-correctness**: Extensive use of const methods and immutable data

&nbsp;
## **License**
CitySearcher is licensed under the [MIT License](https://opensource.org/licenses/MIT).  
You are free to use, modify, and distribute this software for any purpose, including commercial applications, as long as the original license is included.


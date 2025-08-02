# Unit Tests for CitySearcher

This directory contains unit tests for the CitySearcher application, covering both search functionality and deduplication logic using Qt's testing framework.

## Test Overview

### **Test Structure**

The test suite is organized into 4 main test executables:

1. **`CityListModelTest`** - Tests deduplication logic and model functionality
2. **`CitySearchViewModelTest`** - Tests search coordination and service management  
3. **`MockCitySearchServiceTest`** - Tests service behavior and configuration
4. **`ServiceFactoryTest`** - Tests factory pattern implementation

### **Test Coverage**

#### **CityListModelTest (20 tests)**
- **Basic functionality**: Add cities, clear, sorting
- **Deduplication logic**: 
  - Exact duplicate filtering
  - Display name matching (case-insensitive)
  - Name + country combination matching
  - Coordinate proximity filtering (~100m threshold)
  - Mixed duplicate scenarios
- **Edge cases**: Null pointers, empty lists, large datasets
- **Memory management**: Proper cleanup of duplicate objects

#### **CitySearchViewModelTest (21 tests)**
- **Search functionality**: Successful searches, empty queries, error handling
- **State management**: Search state transitions, signal emissions
- **Service management**: Service switching, invalid service types, metadata
- **Integration testing**: Search with deduplication, multiple searches
- **Statistics tracking**: Success/failure counts

#### **MockCitySearchServiceTest (22 tests)**
- **Service interface**: Metadata, initial state, interface compliance
- **Search behavior**: Successful searches, cancellation, state management
- **Mock configuration**: Network delay simulation, error simulation
- **Custom results**: Setting custom test data, duplicate generation
- **Data generation**: Query filtering, coordinate generation, large queries

#### **ServiceFactoryTest (17 tests)**
- **Factory functionality**: Service creation for different types
- **Configuration**: Service configuration, parent object handling
- **Service introspection**: Available services, string conversion, validation
- **Error handling**: Invalid service creation, fallback behavior
- **Metadata**: Service descriptions, requirements, availability

## **Key Test Features**

### **Deduplication Testing**
```cpp
// Tests multiple deduplication strategies:
- Exact display name matches (case-insensitive)
- Same city name + country combinations  
- Coordinate proximity within 0.001° (~100m)
- Mixed scenarios with multiple duplicate types
- Large dataset performance (100+ cities with 99% duplicates)
```

### **Mock Service Configuration**
```cpp
// Configurable test behavior:
service->setSimulateNetworkDelay(true, 500);  // Network simulation
service->setSimulateErrors(true, 0.1);        // 10% error rate
service->setIncludeDuplicatesInResults(true); // Generate test duplicates
service->setCustomResults(testCities);        // Custom test data
```

### **Signal Testing**
```cpp
// Comprehensive signal verification:
QSignalSpy searchingChangedSpy(viewModel, &CitySearchViewModel::isSearchingChanged);
QSignalSpy citiesFoundSpy(service, &ICitySearchService::citiesFound);
QSignalSpy errorSpy(service, &ICitySearchService::searchError);
```

## **Running Tests**

### **Build and Run All Tests**
```bash
# Configure with tests enabled
cmake -B build -S . -DBUILD_TESTS=ON

# Build everything
cmake --build build

# Run all tests
cd build && ctest --verbose
```

### **Run Individual Test Suites**
```bash
# Run specific test
./build/tests/test_city_list_model
./build/tests/test_city_search_viewmodel
./build/tests/test_mock_city_search_service
./build/tests/test_service_factory
```

### **Run Tests with Detailed Output**
```bash
cd build
ctest --output-on-failure  # Show output only for failed tests
ctest --verbose            # Show all output
```

## **Test Architecture**

### **Dependency Injection**
Tests use dependency injection to provide mock services:
```cpp
// ViewModel tests with mock service
auto mockService = createMockService();
viewModel = new CitySearchViewModel(std::move(mockService), this);
```

### **Helper Methods**
Each test class provides helper methods for common operations:
```cpp
// CityListModel helpers
CityModel* createTestCity(name, country, lat, lon);
void verifyModelContains(name, country);
int countCitiesWithName(name);

// ViewModel helpers  
void waitForSearchCompletion(timeoutMs);
void verifySearchState(isSearching, errorMessage);
```

### **Test Data Management**
- **Deterministic results**: Mock service provides predictable test data
- **Memory safety**: Proper parent-child relationships and cleanup
- **Configurable behavior**: Network delays, error rates, custom results

## **Debugging Tests**

### **Debug Output**
Tests include extensive debug logging:
```
QDEBUG : TestCityListModel::testExactDuplicateFiltering() CityListModel: Filtered out 1 duplicate cities
QDEBUG : TestMockCitySearchService::testSuccessfulSearch() MockCitySearchService: Generated 2 mock cities for query: "Berlin"
```

### **Test Failures**
When tests fail, they provide detailed information:
- Expected vs actual values
- File and line number of failure
- Context about what was being tested

### **Signal Debugging**
Signal spies help debug Qt signal/slot interactions:
```cpp
QSignalSpy spy(object, &Class::signal);
// ... perform action ...
QCOMPARE(spy.count(), expectedCount);
```

## **Future Test Enhancements**

- **Integration tests** with real Nominatim service (slower, network-dependent)
- **UI tests** using Qt Quick Test framework
- **Performance benchmarks** for large datasets
- **Fuzz testing** with random input data
- **Coverage analysis** to identify untested code paths 

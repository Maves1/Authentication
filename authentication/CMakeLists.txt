CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

project(authentication)
include(GNUInstallDirs)

include(FindPkgConfig)
pkg_search_module(AURORA auroraapp REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Quick Qml Sensors REQUIRED)

set(CMAKE_AUTOMOC ON)

set(SOURCES collector/sensors/SensorReader.cpp
collector/sensors/GyroscopeReader.cpp
collector/sensors/AccelerometerReader.cpp
collector/sensors/SensorManager.cpp
collector/sensors/filters/GyroscopeRateFilter.cpp
collector/sensors/filters/AccelerometerRateFilter.cpp)

add_executable(authentication collector/main.cpp ${SOURCES})
target_link_libraries(authentication
                      PRIVATE Qt5Core Qt5Gui Qt5Quick Qt5Qml Qt5Sensors
                      ${AURORA_LDFLAGS})
include_directories(${Qt5Core_INCLUDE_DIRS}
                    ${Qt5Gui_INCLUDE_DIRS}
                    ${Qt5Quick_INCLUDE_DIRS}
                    ${Qt5Qml_INCLUDE_DIRS}
                    ${Qt5Sensors_INCLUDE_DIRS}
                    ${AURORA_INCLUDE_DIRS})

INSTALL(TARGETS authentication
        DESTINATION ${CMAKE_INSTALL_BINDIR})

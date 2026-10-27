cmake_minimum_required(VERSION 3.16)
project(TpThemeHypr LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Core Gui Qml Quick Svg)
qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

qt_add_executable(tp-theme-hypr
    src/main.cpp
    src/WidgetBackend.cpp
    src/WidgetBackend.h
    src/SvgClockItem.cpp
    src/SvgClockItem.h
)

qt_add_qml_module(tp-theme-hypr
    URI TpTheme.Hypr
    VERSION 1.0
    QML_FILES
        qml/WidgetWindow.qml
        qml/Clock.qml
        qml/System.qml
        qml/Quote.qml
        qml/Journal.qml
        qml/Animation.qml
)

set_property(SOURCE ../TpTheme-KDE/desklets/src/clock/clock.svgz PROPERTY QT_RESOURCE_ALIAS "clock.svgz")

qt_add_resources(tp-theme-hypr "theme_assets"
    PREFIX "/TpTheme/Hypr/assets"
    FILES
        ../TpTheme-KDE/desklets/src/clock/clock.svgz
)

target_link_libraries(tp-theme-hypr PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::Svg)

install(TARGETS tp-theme-hypr RUNTIME DESTINATION bin)

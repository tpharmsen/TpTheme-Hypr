cmake_minimum_required(VERSION 3.16)
project(TpThemeHypr LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Core Gui Qml Quick Svg)
find_package(LayerShellQt 6 REQUIRED)
qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

set(TP_THEME_SOURCES
    src/main.cpp
    src/WidgetBackend.cpp
    src/WidgetBackend.h
    src/SvgClockItem.cpp
    src/SvgClockItem.h
)

set(TP_THEME_QML
    qml/WidgetWindow.qml
    qml/Panel.qml
    qml/Clock.qml
    qml/System.qml
    qml/Quote.qml
    qml/Journal.qml
    qml/Animation.qml
)

set(TP_THEME_ASSETS
    assets/clock.svgz
    assets/archlinux-logo.svg
    assets/signatureTP.svg
)

qt_add_executable(tp-theme-hypr
    ${TP_THEME_SOURCES}
)

qt_add_qml_module(tp-theme-hypr
    URI TpTheme.Hypr
    VERSION 1.0
    QML_FILES ${TP_THEME_QML}
)

set_property(SOURCE assets/clock.svgz PROPERTY QT_RESOURCE_ALIAS "clock.svgz")
set_property(SOURCE assets/archlinux-logo.svg PROPERTY QT_RESOURCE_ALIAS "archlinux-logo.svg")

qt_add_resources(tp-theme-hypr "theme_assets"
    PREFIX "/TpTheme/Hypr/assets"
    FILES ${TP_THEME_ASSETS}
)

target_link_libraries(tp-theme-hypr
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::Svg
        LayerShellQt::Interface
)

include(GNUInstallDirs)
install(TARGETS tp-theme-hypr RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

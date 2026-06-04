cmake_minimum_required(VERSION 3.16)
project(BlackRelay)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── SFML 3 via MSYS2 ──────────────────────────────────────────────────────────
# pacman -S mingw-w64-ucrt-x86_64-sfml
find_package(SFML 3 COMPONENTS Graphics Audio Window System REQUIRED)

set(SOURCES
    main.cpp

    # Affichage Menu
    Affichage/Affichage_Menu/Sources/affichage_menu.cpp

    # Menu
    Menu/Sources/Menu.cpp
    Menu/Sources/Options.cpp
    Menu/Sources/Scores.cpp
)

# ── Icône Windows ──────────────────────────────────────────────────────────────
if(WIN32)
    list(APPEND SOURCES "Assets/Icones/Ico/app.rc")
endif()

add_executable(BlackRelay ${SOURCES})
target_include_directories(BlackRelay PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(BlackRelay
    SFML::Graphics
    SFML::Audio
    SFML::Window
    SFML::System
)

# Copier Assets/ à côté de l'exécutable
add_custom_command(TARGET BlackRelay POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Assets $<TARGET_FILE_DIR:BlackRelay>/Assets)
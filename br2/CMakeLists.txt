cmake_minimum_required(VERSION 3.16)
project(BlackRelay)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics Audio Window System REQUIRED)
find_package(nlohmann_json REQUIRED)

set(SOURCES
    main.cpp
    Affichage/Affichage_Menu/Sources/affichage_menu.cpp
    Affichage/Game/Sources/Game.cpp
    Game/Model/Sources/Game.cpp
    Game/Model/Enemy/Sources/Ennemy.cpp
    Game/Model/Player_Tower/Sources/Player_Tower.cpp
    Game/Controller/Sources/Game.cpp
    Menu/Sources/Menu.cpp
    Menu/Sources/Options.cpp
    Menu/Sources/Scores.cpp
)

add_executable(BlackRelay ${SOURCES})
target_include_directories(BlackRelay PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(BlackRelay
    SFML::Graphics SFML::Audio SFML::Window SFML::System
    nlohmann_json::nlohmann_json
)

add_custom_command(TARGET BlackRelay POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Assets $<TARGET_FILE_DIR:BlackRelay>/Assets)

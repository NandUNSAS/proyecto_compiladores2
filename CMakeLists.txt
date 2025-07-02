cmake_minimum_required(VERSION 3.10)

project(proyecto_compiladores)

set(CMAKE_CXX_STANDARD 17)

# Busca SFML instalado en el sistema
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

set(SOURCES
    #src/main_sfml.cpp
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
)

include_directories(include)


# Añade el ejecutable con tus archivos fuente
add_executable(proyecto_compiladores ${SOURCES})

# Enlaza SFML con tu ejecutable
target_link_libraries(proyecto_compiladores sfml-system sfml-window sfml-graphics sfml-network sfml-audio)

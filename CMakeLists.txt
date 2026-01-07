cmake_minimum_required(VERSION 3.16)

# NOTE: update executable name in .github/workflows/cmake.yml:25 when changing executable name in this file
set(MAIN_PROJECT_NAME "oop")
set(MAIN_EXECUTABLE_NAME "${MAIN_PROJECT_NAME}")

# Această linie îi spune lui CMake unde ai instalat SFML prin pacman
set(SFML_DIR "C:/msys64/mingw64/lib/cmake/SFML")

project(${MAIN_PROJECT_NAME})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/Options.cmake)
include(cmake/CompilerFlags.cmake)
include(cmake/CopyHelper.cmake)

###############################################################################
# DEPENDENȚE EXTERNE
###############################################################################

# Folosim find_package în loc de FetchContent pentru a evita eroarea de compatibilitate Freetype/CMake < 3.5
find_package(SFML 3.0 REQUIRED COMPONENTS Graphics Window System)
find_package(Threads REQUIRED)

if(APPLE)
elseif(UNIX)
    find_package(X11)
endif()

###############################################################################
# EXECUTABILUL ȘI LISTA COMPLETĂ DE FIȘIERE
###############################################################################

add_executable(${MAIN_EXECUTABLE_NAME}
        main.cpp
        CardBancar.cpp CardBancar.h
        Configuratie.cpp Configuratie.h
        DateAutentificare.cpp DateAutentificare.h
        EroareAcces.cpp EroareAcces.h
        EroareValidare.cpp EroareValidare.h
        GestionarParole.cpp GestionarParole.h
        Identitate.cpp Identitate.h
        Manager.cpp Manager.h
        NotitaSecurizata.cpp NotitaSecurizata.h
        ObiectNegasit.cpp ObiectNegasit.h
        ScriptException.cpp ScriptException.h
        Seif.cpp Seif.h
        SeifFactory.cpp SeifFactory.h
        Utilizator.cpp Utilizator.h
        ResourceManager.cpp ResourceManager.hpp
        generated/src/Helper.cpp
)

# Setările de compilare ale laboranților
set_compiler_flags(RUN_SANITIZERS TRUE TARGET_NAMES ${MAIN_EXECUTABLE_NAME})

###############################################################################
# INCLUDE ȘI LINK (Legăm SFML-ul de proiectul tău)
###############################################################################

target_include_directories(${MAIN_EXECUTABLE_NAME} SYSTEM PRIVATE generated/include)

# Legăm bibliotecile SFML instalate prin pacman
target_link_libraries(${MAIN_EXECUTABLE_NAME}
        PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        Threads::Threads
)

if(APPLE)
elseif(UNIX)
    target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE X11)
endif()

###############################################################################
# COPIEREA RESURSELOR (Instalare și foldere locale)
###############################################################################

install(TARGETS ${MAIN_EXECUTABLE_NAME} DESTINATION ${DESTINATION_DIR})
if(APPLE)
    install(FILES launcher.command DESTINATION ${DESTINATION_DIR})
endif()

# Copiem fișierele și folderele necesare pentru SFML 3.0
copy_files(FILES tastatura.txt COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})
copy_files(DIRECTORY images fonts COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})
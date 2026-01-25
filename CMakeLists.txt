cmake_minimum_required(VERSION 3.16)

set(MAIN_PROJECT_NAME "oop")
set(MAIN_EXECUTABLE_NAME "${MAIN_PROJECT_NAME}")

project(${MAIN_PROJECT_NAME})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/Options.cmake)
include(cmake/CompilerFlags.cmake)
include(cmake/CopyHelper.cmake)

###############################################################################
# DEPENDENȚE EXTERNE - FetchContent
###############################################################################

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

# Dezactivăm tratarea avertismentelor ca erori pentru SFML (previne eșecul build-ului)
set(WARNINGS_AS_ERRORS OFF)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        3.0.0
        GIT_SHALLOW    1
)

FetchContent_MakeAvailable(SFML)

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
        Observer.cpp
        Observer.h
        SecurityMonitor.cpp SecurityMonitor.h
)

set_compiler_flags(RUN_SANITIZERS TRUE TARGET_NAMES ${MAIN_EXECUTABLE_NAME})

###############################################################################
# LINK
###############################################################################

target_include_directories(${MAIN_EXECUTABLE_NAME} SYSTEM PRIVATE
        generated/include
        ${SFML_SOURCE_DIR}/include
)

target_link_directories(${MAIN_EXECUTABLE_NAME} PRIVATE ${SFML_BINARY_DIR}/lib)

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
# COPIEREA RESURSELOR (Imagini, Fonturi)
###############################################################################

install(TARGETS ${MAIN_EXECUTABLE_NAME} DESTINATION ${DESTINATION_DIR})
if(APPLE)
    install(FILES launcher.command DESTINATION ${DESTINATION_DIR})
endif()

copy_files(FILES tastatura.txt COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})
copy_files(DIRECTORY images fonts COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})
/**
 * @file main.cpp
 * @brief Entry point for the S.C.R.I.P.T Password Manager application.
 *
 * This driver file initializes the application wrapper and handles top-level exception catching.
 * It strictly avoids technical streams like 'cerr' and uses the custom 'ScriptException'
 * hierarchy to provide helpful hints to the user.
 *
 * @author Andrei Nituica
 * @date 2026
 */

#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

// Include the Application Wrapper
#include "UserInterface.h"

// Include custom Error classes for specific handling
#include "ScriptException.h"
#include "EroareFisier.h"

/**
 * @brief Main execution function.
 *
 * 1. Sets up the environment (UTF-8).
 * 2. Instantiates the UserInterface (which triggers resource loading).
 * 3. Starts the main loop.
 * 4. Catches specific errors (File missing, Script logic) to show hints.
 *
 * @retval 0 Success.
 * @retval -1 Missing resource file (EroareFisier).
 * @retval -2 Application error (ScriptException).
 * @retval -3 Standard exception.
 * @retval -4 Unknown error.
 */
int main()
{
    // Ensure the Windows console supports UTF-8 characters.
    // This allows printing characters like 'ă', 'ș', 'ț' correctly.
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    try
    {
        // 1. Initialize the Application Wrapper
        // The constructor handles resource loading. If "logo.png" or "font.ttf"
        // is missing, it will throw an 'EroareFisier'.
        UserInterface app;

        // 2. Start the Application Loop
        // The app runs here until the window is closed.
        app.run();
    }
    catch (const EroareFisier& e)
    {
        // SPECIFIC CATCH: Missing Resource Files
        // This is the most common startup error. We treat it specially.

        std::cout << "\n[!] Resource Error: " << e.what() << "\n";
        std::cout << " -> Suggestion: " << e.getSugestie() << "\n";

        #ifdef _WIN32
        // Show a visual popup because the console might close too fast
        MessageBoxA(nullptr, e.getSugestie().c_str(), "Missing File", MB_ICONERROR);
        #endif

        return -1;
    }
    catch (const ScriptException& e)
    {
        // GENERAL CATCH: Other custom logic errors (Validation, Access, etc.)
        // Uses the polymorphic 'getSugestie()' to help the user.

        std::cout << "\n[!] Application Error: " << e.what() << "\n";
        std::cout << " -> Suggestion: " << e.getSugestie() << "\n";

        #ifdef _WIN32
        std::string msg = std::string(e.what()) + "\n\n" + e.getSugestie();
        MessageBoxA(nullptr, msg.c_str(), "Application Error", MB_ICONWARNING);
        #endif

        return -2;
    }
    catch (const std::exception& e)
    {
        // FALLBACK CATCH: Standard C++ errors (memory, runtime_error)
        // We use cout instead of cerr as requested.

        std::cout << "\n[!] System Error: " << e.what() << "\n";

        #ifdef _WIN32
        MessageBoxA(nullptr, e.what(), "Critical Error", MB_ICONERROR);
        #endif

        return -3;
    }
    catch (...)
    {
        // UNKNOWN ERROR
        std::cout << "\n[!] An unknown error occurred. The application must close.\n";
        return -4;
    }

    return 0;
}

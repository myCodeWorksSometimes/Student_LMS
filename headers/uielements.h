#pragma once

#include <vector>
#include <string>
#include "User.h" // To get user details

// --- FTXUI ---
// This is a third-party library for creating Terminal User Interfaces.
// You must have it included in your project.
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"


namespace UI {

using namespace ftxui;

/**
 * @brief Displays an interactive, dashboard-style menu for a user.
 * 
 * Creates a terminal window with a title, user's name/role, and a list
 * of selectable options. The user can navigate with arrow keys and select
 * with Enter. Pressing ESC quits the menu.
 * 
 * @param user The currently logged-in user object.
 * @param options A vector of strings representing the menu choices.
 * @param title The title to display at the top of the dashboard.
 * @return The 0-based index of the selected option. Returns -1 if the user
 *         quits the menu without making a selection (e.g., by pressing ESC).
 */
static int showDashboard(const User& user, const std::vector<std::string>& options, const std::string& title) {
    auto screen = ScreenInteractive::TerminalOutput();

    int selected_option = -1; // Default to -1 (quit)

    // -- Component Definition --
    // Use a vertical menu. When the user presses 'Enter', exit the screen loop.
    MenuOption menu_option;
    menu_option.on_enter = screen.ExitLoopClosure();
    auto menu = Menu(&options, &selected_option, menu_option);

    // -- Layout --
    // Create a component that lays out the static header and the interactive menu.
    auto main_component = Renderer(menu, [&] {
        return vbox({
            // Header
            text(title) | bold | hcenter,
            separator(),
            hbox(text(" Welcome, " + user.getName()), filler(), text("Role: " + user.getRole() + " ")),
            separator(),
            // Menu
            menu->Render() | vscroll_indicator | frame | flex,
            separator(),
            text("Use Arrow Keys to navigate, Enter to select, ESC to exit.") | dim | hcenter,
        }) | border;
    });

    // -- Event Handling --
    // Add a top-level event catcher to handle the ESC key for quitting.
    main_component |= CatchEvent([&](Event event) {
        if (event == Event::Escape) {
            selected_option = -1; // Ensure quit returns -1
            screen.Exit();
            return true;
        }
        return false;
    });

    screen.Loop(main_component);

    // The selected_option is updated by the Menu component.
    return selected_option;
}

} 
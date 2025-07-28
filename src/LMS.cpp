#include "../headers/LMS.h"
#include "../headers/FileManager.h"
#include "../headers/Utils.h"
#include <iostream>
#include <algorithm>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
using namespace ftxui;

LMS::LMS() : currentUser(nullptr) {}

void LMS::loadAllData() {
    std::cout << "Loading system data...\n";
    users = FileManager::loadAllUsers();
    std::cout << "Data loaded successfully. " << users.size() << " users found.\n";
    Utils::pause();
}

void LMS::run() {
    loadAllData();
    while (true) {
        displayLoginScreen();
        if (currentUser) {
            currentUser->displayMenu();
            // After logout, re-add the user to the main list
            auto it = std::find_if(users.begin(), users.end(), [&](const auto& u) {
                return u->getUserID() == currentUser->getUserID();
            });
            if (it == users.end()) {
                 users.push_back(std::move(currentUser));
            }
            currentUser = nullptr;
        } else {
            std::string exitChoice = Utils::toLower(Utils::getNonEmptyString("Invalid credentials. Type 'exit' to quit or press Enter to retry: "));
            if (exitChoice == "exit") {
                break;
            }
        }
    }
    std::cout << "Exiting Learning Management System. Goodbye!\n";
}

void LMS::displayLoginScreen() {
    auto screen = ScreenInteractive::Fullscreen();
    
    std::string user_id = "";
    std::string password = "";
    
    auto input_user_id = Input(&user_id, "User ID");
    auto input_password = Input(&password, "Password");
    
    bool should_exit = false;
    auto login_button = Button("Login", [&]() {
        if (!user_id.empty() && !password.empty()) {
            attemptLogin(user_id, password);
        }
        should_exit = true;
    });
    
    auto container = Container::Vertical({
        input_user_id,
        input_password,
        login_button
    });
    
    auto renderer = Renderer(container, [&] {
        return vbox({
            text("Advanced Learning Management System") | bold | color(Color::Cyan) | center,
            text("") | size(HEIGHT, EQUAL, 1),
            hbox({text("User ID: "), input_user_id->Render() | size(WIDTH, EQUAL, 20)}) | color(Color::White),
            text("") | size(HEIGHT, EQUAL, 1),
            hbox({text("Password: "), input_password->Render() | size(WIDTH, EQUAL, 20)}) | color(Color::White),
            text("") | size(HEIGHT, EQUAL, 1),
            login_button->Render() | center,
        }) | border | center;
    });
    
    renderer |= CatchEvent([&](Event event) {
        if (event == Event::Escape || should_exit) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });
    
    screen.Loop(renderer);
}
bool LMS::attemptLogin(const std::string& id, const std::string& password) {
    auto it = std::find_if(users.begin(), users.end(), [&](const std::unique_ptr<User>& u) {
        return u->getUserID() == id;
    });

    if (it != users.end()) {
        if ((*it)->verifyPassword(password)) {
            currentUser = std::move(*it);
            users.erase(it); // Temporarily remove from list to simulate a "session"
            return true;
        }
    }
    return false;
}


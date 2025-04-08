#include <SFML/Graphics.hpp>
#include <lo/lo.h>
#include <iostream>
#include <thread>
#include <chrono>

#include "detected_object.h"
#include "message_handling.h"
#include "utils.h"

int main() {
    SharedState state;

    // Create an OSC server listening on port 5005
    lo_server_thread server = lo_server_thread_new("9001", nullptr);
    if (!server) {
        std::cerr << "Failed to create OSC server!" << std::endl;
        return 1;
    }
    // Map the "/randomNum" address to the message handler
    lo_server_thread_add_method(server, "/objid", "i",  get_id, &state);
    lo_server_thread_add_method(server, "/coordinateX/coordinateY/coordinateZ", "fff",get_coordinates, &state);

    // Start the server
    lo_server_thread_start(server);
    std::cout << "Server listening on port 5005..." << std::endl;
    //std::cout << "Live ID: " << state.obj_id << ", X: " << state.x << std::endl;

    // Keep the server running
    //while (true) {
    //    std::this_thread::sleep_for(std::chrono::seconds(1));
    //    std::cout << "Live ID: " << state.obj_id << ", X: " << state.x << std::endl;
    //}

    sf::RenderWindow window(sf::VideoMode(800, 600), "Red Dot on Black Background");

    sf::CircleShape redDot(10); // Radius 10
    redDot.setFillColor(sf::Color::Red);

    // Initial position
    redDot.setPosition(map_x_coord(state.x),map_z_coord(state.z));

    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        std::cout << "Live ID: " << state.obj_id << ", mapped X: " << map_x_coord(state.x) << ", mapped Z: " << map_z_coord(state.z) << std::endl;

        // Example movement: move to the right
        redDot.setPosition(map_x_coord(state.x),map_z_coord(state.z));

        window.clear(sf::Color::Black);   // Clear with black
        window.draw(redDot);              // Draw the red dot
        window.display();                 // Show on screen
    }

    return 0;
}

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>  
#include <lo/lo.h>
#include <iostream>
#include <thread>
#include <chrono>

// Initial position of the circle's center
float x = 0.0f, y = 0.0f;
float radius = 0.1f;  // Radius of the circle
int segments = 100;   // Number of segments to approximate the circle

// Callback function for received messages
int message_handler(const char* path, const char* types, lo_arg** argv, int argc, lo_message msg, void* user_data) {
    std::cout << "Received message on " << path << ": " << argv[0]->f << argv[1]->f << argv[2]->f << std::endl;
    return 0;
}


int foo_handler(const char* path, const char* types, lo_arg** argv,
    int argc, lo_message data, void* user_data)
{
    /* example showing pulling the argument values out of the argv array */
    printf("foo: %s <-i:%d\n\n", path, argv[0]->i);
    fflush(stdout);

    return 0;
}

// Function to draw a circle
void drawCircle(float x, float y, float radius) {
    glBegin(GL_POLYGON);  // Start drawing a filled polygon (circle)

    for (int i = 0; i < segments; i++) {
        float angle = (2.0f * 3.14159f * i) / segments;  // Calculate the angle
        float dx = x + radius * cos(angle);  // X coordinate of a point on the circle
        float dy = y + radius * sin(angle);  // Y coordinate of a point on the circle
        glVertex2f(dx, dy);  // Specify the point
    }

    glEnd();  // End the polygon
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0f, 0.0f, 0.0f); // Red color
    drawCircle(x, y, radius);    // Draw the circle

    glFlush();
}


int main() {
    // Create an OSC server listening on port 5005
    lo_server_thread server = lo_server_thread_new("5005", nullptr);

    if (!server) {
        std::cerr << "Failed to create OSC server!" << std::endl;
        return 1;
    }

    // Map the "/randomNum" address to the message handler
    lo_server_thread_add_method(server, "/objid", "i", foo_handler, nullptr);
    lo_server_thread_add_method(server, "/coordinateX/coordinateY/coordinateZ", "fff", message_handler, nullptr);
    //lo_server_thread_add_method(server, "/coordinateY", "f", message_handler, nullptr);
    //lo_server_thread_add_method(server, "/coordinateZ", "f", message_handler, nullptr);

    // Start the server
    lo_server_thread_start(server);
    std::cout << "Server listening on port 5005..." << std::endl;

    // Keep the server running
    //while (true) {
       // std::this_thread::sleep_for(std::chrono::seconds(1));
    //}
    

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Create a window
    GLFWwindow* window = glfwCreateWindow(500, 500, "2D Visualizer", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW (Needed for OpenGL functions)
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return -1;
    }

    // Set the viewport and projection
    glViewport(0, 0, 500, 500);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);


    // Main loop
    while (!glfwWindowShouldClose(window)) {
        

        x = 0.00f; y = 0.00f; // Update position

        display(); // Render frame
        glfwSwapBuffers(window); // Swap front and back buffers
        glfwPollEvents(); // Check for events
    }

    glfwTerminate();
    return 0;
}


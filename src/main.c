#include <SDL2/SDL.h>
#include <SDL_timer.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h> // for the fixed integer types

#include "display.h"
#include "triangle.h"
#include "vector.h"
#include "mesh.h"

// Declare an array of vectors/points
// #define N_POINTS (9 * 9 * 9)
// vec3_t cube_points[N_POINTS]; // 9x9x9 cube
// vec2_t projected_points[N_POINTS];

triangle_t triangles_to_render[N_MESH_FACES];

vec3_t camera_position = {.x = 0, .y = 0, .z = -5};
vec3_t cube_rotation = {.x = 0, .y = 0, .z = 0};

float fov_factor = 640;

bool is_running = false;

int previous_frame_time = 0;

void setup(void) {
    
    // Allocate memory for the color buffer
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);
    
    // Creating an SDL texture that is used to displa the color buffer
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );
    
    // int point_count = 0;
    
    // // Start loading my array of vectors
    // // From -1 to 1 (in this 9x9x9 cube)
    // for(float x = -1; x <= 1; x += 0.25) {
    //     for(float y = -1; y <= 1; y += 0.25) {
    //         for(float z = -1; z <= 1; z += 0.25) {
    //             vec3_t new_point = {.x = x, .y = y, .z = z};
    //             cube_points[point_count++] = new_point;
    //         }
    //     }
    // }
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch(event.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE)
                is_running = false;
            break;
    }
}

// Function received 3D vector and returns projected 2D point
vec2_t project(vec3_t point) {
    vec2_t projected_point = {
        .x = (fov_factor * point.x) / point.z,
        .y = (fov_factor * point.y) / point.z
    };
    return projected_point;
}

void update(void) {

    // Old method - using a naive while loop to wait - wasted CPU cycles just to wait - Also called busy waiting
    // while(!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME));

    // Wait some time until we reach the target frame time in ms
    int time_to_wait = FRAME_TARGET_TIME -(SDL_GetTicks() - previous_frame_time);

    // Only delay execution when you are running too fast
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks();

    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;

    // Loop all the traingle faces of our mesh
    for(int i = 0; i < N_MESH_FACES; i++) {
        face_t mesh_face = mesh_faces[i];

        // For one face, get the three vertices
        vec3_t face_vertices[3];
        face_vertices[0] = mesh_vertices[mesh_face.a - 1];
        face_vertices[1] = mesh_vertices[mesh_face.b - 1];
        face_vertices[2] = mesh_vertices[mesh_face.c - 1];

        triangle_t projected_triangle;

        // Loop all three vertices of this current face and apply transformations
        for(int j = 0; j < 3; j++) {
            vec3_t transformed_vertex = face_vertices[j];

            transformed_vertex = vec3_rotate_x(transformed_vertex, cube_rotation.x); // Rotate around X
            transformed_vertex = vec3_rotate_y(transformed_vertex, cube_rotation.y); // Then Rotate around Y
            transformed_vertex = vec3_rotate_z(transformed_vertex, cube_rotation.z); // and then finally Rotate around Z

            // Translate the vertedx away from the camera (in Z)
            transformed_vertex.z -= camera_position.z;

            // Project our point after transforming
            vec2_t projected_point = project(transformed_vertex);

            // Scale and Translate Projected Point to the middle of the screen
            projected_point.x += (window_width / 2.0);
            projected_point.y += (window_height / 2.0);

            projected_triangle.points[j] = projected_point;
        }

        // Save the projected triangle in the array of triangles to render
        triangles_to_render[i] = projected_triangle;
    }


    /*
    for(int i = 0; i < N_POINTS; i++) {
        vec3_t point = cube_points[i];

        // Rotate the obtained point
        vec3_t transformed_point = vec3_rotate_x(point, cube_rotation.x);
        transformed_point = vec3_rotate_y(transformed_point, cube_rotation.y);
        transformed_point = vec3_rotate_z(transformed_point, cube_rotation.z);

        // Translate the points away from the camera
        transformed_point.z -= camera_position.z;
        
        // Project the current point
        vec2_t projected_point = project(transformed_point);
        // Save the projected 2D vector in the array of projected points
        projected_points[i] = projected_point;
    }
    */
}

void render(void) {
    draw_grid();
    
    // Loop all projected triangles and render them now
    for(int i = 0; i< N_MESH_FACES; i++) {
        triangle_t triangle = triangles_to_render[i];
        
        // Draw the vertices of the face / triangle
        draw_rect(triangle.points[0].x , triangle.points[0].y, 3, 3, 0xFFFFFF00);
        draw_rect(triangle.points[1].x , triangle.points[1].y, 3, 3, 0xFFFFFF00);
        draw_rect(triangle.points[2].x , triangle.points[2].y, 3, 3, 0xFFFFFF00);
    
        // Draw the edges of the face / triangle
        draw_line(triangle.points[0].x, triangle.points[0].y, triangle.points[1].x, triangle.points[1].y, 0xFFFFFF00);
        draw_line(triangle.points[1].x, triangle.points[1].y, triangle.points[2].x, triangle.points[2].y, 0xFFFFFF00);
        draw_line(triangle.points[2].x, triangle.points[2].y, triangle.points[0].x, triangle.points[0].y, 0xFFFFFF00);

    }

    // Loop all projected points and render them
    // for(int i = 0; i < N_POINTS; i++) {
    //     vec2_t projected_point = projected_points[i];
    //     draw_rect(projected_point.x + (window_width / 2.0), projected_point.y + (window_height / 2.0), 4, 4, 0xFFFFFF00);
    // }
    
    render_color_buffer();
    clear_color_buffer(0xFF000000);
    
    SDL_RenderPresent(renderer);
    
}

int main(void) {
    is_running = initialize_window();

    setup();

    while(is_running) {
        process_input();
        update();
        render();
    }
    
    destroy_window();

    return EXIT_SUCCESS;
}


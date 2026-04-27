#include <SDL2/SDL.h>
#include <SDL_timer.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h> // for the fixed integer types

#include "array.h"
#include "display.h"
#include "triangle.h"
#include "vector.h"
#include "mesh.h"

triangle_t* triangles_to_render = NULL;
vec3_t camera_position = {0, 0, 0};

float fov_factor = 640;
bool is_running = false;

int previous_frame_time = 0;

void setup(void) {

    // Start with Wireframe and Culling enabled
    render_method = RENDER_WIRE;
    cull_method = CULL_BACKFACE;

    // Allocate memory for the color buffer
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);
    
    // Creating an SDL texture that is used to display the color buffer
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );


    // Load OBJ mesh data into mesh global variable
    load_cube_mesh_data();
    // load_obj_file_data("./assets/f22.obj");


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
            else if(event.key.keysym.sym == SDLK_1)
                render_method = RENDER_WIRE_VERTEX;
            else if(event.key.keysym.sym == SDLK_2)
                render_method = RENDER_WIRE;
            else if(event.key.keysym.sym == SDLK_3)
                render_method = RENDER_FILL_TRIANGLE;
            else if(event.key.keysym.sym == SDLK_4)
                render_method = RENDER_FILL_TRIANGLE_WIRE;
            else if(event.key.keysym.sym == SDLK_c)
                cull_method = CULL_BACKFACE;
            else if(event.key.keysym.sym == SDLK_d)
                cull_method = CULL_NONE;
            break;
        default: ;
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

    // Wait some time until we reach the target frame time in ms
    int time_to_wait = FRAME_TARGET_TIME -(SDL_GetTicks() - previous_frame_time);

    // Only delay execution when you are running too fast
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks();

    // Initialize the dynamic array of triangles to render (reset to NULL)
    triangles_to_render = NULL;

    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    mesh.rotation.z += 0.01;

    // Loop all the traingle faces of our mesh
    int num_faces = array_length(mesh.faces);
    for(int i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh.faces[i];

        // For one face, get the three vertices
        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec3_t transformed_vertices[3];

        // Loop all three vertices of this current face and apply transformations
        for(int j = 0; j < 3; j++) {
            vec3_t transformed_vertex = face_vertices[j];

            transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x); // Rotate around X
            transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y); // Then Rotate around Y
            transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z); // and then finally Rotate around Z

            // Translate the vertex away from the camera by 5 units in Z
            transformed_vertex.z += 5;

            // Save the transformed vertex in the array of transformed vertices
            transformed_vertices[j] = transformed_vertex;
        }

        if(cull_method == CULL_BACKFACE) {
        
            // Check backface culling
            vec3_t vector_a = transformed_vertices[0]; /* A */
            vec3_t vector_b = transformed_vertices[1]; /* B */
            vec3_t vector_c = transformed_vertices[2]; /* C */

            // Get vector subtraction of B-A and C-A
            vec3_t vector_ab = vec3_sub(vector_b, vector_a);
            vec3_t vector_ac = vec3_sub(vector_c, vector_a);
            vec3_normalize(&vector_ab);
            vec3_normalize(&vector_ac);

            // Compute the face normal using cross product (we are using left-handed coordinate system)
            vec3_t normal = vec3_cross(vector_ab, vector_ac);

            // Normalize the face normal vector (pass by reference)
            vec3_normalize(&normal);

            // Find the vector between a point in the triangle and the camera origin
            vec3_t camera_ray = vec3_sub(camera_position, vector_a);

            // Calculate how aligned the camera ray is with the face normal (using dot product)
            float dot_normal_camera = vec3_dot(camera_ray, normal);

            // bypass the triangles that are looking away from the camera
            if (dot_normal_camera < 0) {
                continue;
            }
        }

        vec2_t projected_points[3];

        // Loop all the three vertices to perform projection
        for (int j = 0; j<3; j++) {
            // Project our point after transforming
            projected_points[j] = project(transformed_vertices[j]);

            // Scale and Translate Projected Point to the middle of the screen
            projected_points[j].x += (window_width / 2.0);
            projected_points[j].y += (window_height / 2.0);
        }

        triangle_t projected_triangle = {
            .points = {
                {projected_points[0].x, projected_points[0].y},
                {projected_points[1].x, projected_points[1].y},
                {projected_points[2].x, projected_points[2].y}
            },
            .color = mesh_face.color
        };

        // Save the projected triangle in the dynamic array of triangles
        array_push(triangles_to_render, projected_triangle);
    }

}

void render(void) {
    draw_grid();
    
    // Loop all projected triangles and render them now
    int num_triangles = array_length(triangles_to_render);
    for(int i = 0; i< num_triangles; i++) {
        triangle_t triangle = triangles_to_render[i];

        // Draw vertices
        if (render_method == RENDER_WIRE_VERTEX) {
            draw_rect(triangle.points[0].x, triangle.points[0].y, 5, 5, 0xFFFF0000);
            draw_rect(triangle.points[1].x, triangle.points[1].y, 5, 5, 0xFFFF0000);
            draw_rect(triangle.points[2].x, triangle.points[2].y, 5, 5, 0xFFFF0000);
        }

        // Draw filled triangle face
        if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE) {
            draw_filled_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                triangle.color
            );
        }

        // Draw unfilled triangle (wireframe)
        if (render_method == RENDER_WIRE_VERTEX || render_method == RENDER_WIRE || render_method == RENDER_FILL_TRIANGLE_WIRE) {
            draw_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                0xFF00FF00
            );
        }

    }

    

    // Clear the array of triangles every frame loop
    array_free(triangles_to_render);
    
    render_color_buffer();

    clear_color_buffer(0xFF000000);
    
    SDL_RenderPresent(renderer);
    
}

// Free memory that was dynamically allocated by the program
void free_resources(void) {
    free(color_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);

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
    free_resources();

    return EXIT_SUCCESS;
}


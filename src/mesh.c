#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "mesh.h"
#include "array.h"
#include "vector.h"

mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = {0, 0, 0}
};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    {.x = -1, .y = -1, .z = -1},
    {.x = -1, .y =  1, .z = -1},
    {.x =  1, .y =  1, .z = -1},
    {.x =  1, .y = -1, .z = -1},
    {.x =  1, .y =  1, .z =  1},
    {.x =  1, .y = -1, .z =  1},
    {.x = -1, .y =  1, .z =  1},
    {.x = -1, .y = -1, .z =  1},
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    {.a = 1, .b = 2, .c = 3, .color = 0xFFFF0000},
    {.a = 1, .b = 3, .c = 4, .color = 0xFFFF0000},
    // right
    {.a = 4, .b = 3, .c = 5, .color = 0xFF00FF00},
    {.a = 4, .b = 5, .c = 6, .color = 0xFF00FF00},
    // back
    {.a = 6, .b = 5, .c = 7, .color = 0xFF0000FF},
    {.a = 6, .b = 7, .c = 8, .color = 0xFF0000FF},
    // left
    {.a = 8, .b = 7, .c = 2, .color = 0xFFFFFF00},
    {.a = 8, .b = 2, .c = 1, .color = 0xFFFFFF00},
    // top
    {.a = 2, .b = 7, .c = 5, .color = 0xFF00FFFF},
    {.a = 2, .b = 5, .c = 3, .color = 0xFF00FFFF},
    // bottom
    {.a = 6, .b = 8, .c = 1, .color = 0xFFFF00FF},
    {.a = 6, .b = 1, .c = 4, .color = 0xFFFF00FF},
};

void load_cube_mesh_data(void) {
    
    for(int i = 0; i < N_CUBE_VERTICES; i++) {
        vec3_t cube_vertex = cube_vertices[i];
        array_push(mesh.vertices, cube_vertex);
    }

    for(int i=0; i < N_CUBE_FACES; i++) {
        face_t cube_face = cube_faces[i];
        array_push(mesh.faces, cube_face);
    }
}

// Read the content of OBJ file and load contents in global mesh variable
void load_obj_file_data(char* filename) {

    size_t line_size = 2048;
    size_t command_size = 32;
    size_t data_size = 2048;

    // Allocate each buffer on the heap
    char* line = malloc(line_size);
    char* command = malloc(command_size);
    char* data = malloc(data_size);

    if (line == NULL || command == NULL || data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(line);
        free(command);
        free(data);
        return;
    }

        FILE* fp = NULL;
    fp = fopen(filename, "r");
    if(fp == NULL) {
        fprintf(stderr, "Could not open .obj file.\n");
        free(line);
        free(command);
        free(data);
        return;
    }


    while(fgets(line, line_size, fp) != NULL) {
        int matched = sscanf(line, "%s %[^\n]", command, data);
        
        if (matched <= 0) continue; // empty lines

        if (strcmp(command, "#") == 0) continue; // comments

        // vertex
        if (strcmp(command, "v") == 0) {
            float vertices[3];

            matched = sscanf(data, "%f %f %f", &vertices[0], &vertices[1], &vertices[2]);

            if(matched <= 2) {
                fprintf(stderr, "Wrong format for .obj file.\n");
            }

            vec3_t vertex = {
                .x = vertices[0],
                .y = vertices[1],
                .z = vertices[2]
            };

            array_push(mesh.vertices, vertex);
        }

        // face
        if(strcmp(command, "f") == 0) {
            char v[3][128];

            matched = sscanf(data, "%s %s %s", v[0], v[1], v[2]);

            if(matched <= 2) {
                fprintf(stderr, "Wrong format for .obj data.\n");
            }

            // char vi[3][128], ti[3][128], ni[3][128];
            int vi[3], ti[3], ni[3];
            for(int i=0; i<3; i++) {
                matched = sscanf(v[i], "%d/%d/%d", &vi[i], &ti[i], &ni[i]);
                if(matched <= 0) {
                    fprintf(stderr, "Wrong format for .obj data.\n");
                }
            }

            face_t face = {
                .a = vi[0],
                .b = vi[1],
                .c = vi[2]
            };

            array_push(mesh.faces, face);
            
        }
    }

    fclose(fp);
    free(line); free(command); free(data);
}

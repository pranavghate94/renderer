#include "display.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;
int window_width = 800;
int window_height = 600;

bool initialize_window(void) {
    
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return EXIT_FAILURE;
    }
    
    // Use SDL to query fullscreen max. width and max height
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    window_width = display_mode.w;
    window_height = display_mode.h;
    
    // Create an SDL window
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS
    );
    if(!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return EXIT_FAILURE;
    }

    // Create an SDL renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return EXIT_FAILURE;
    }
    
    // Set Real Fullscreen
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

    return true;
}

void draw_grid(void) {
    for(int y = 0; y < window_height; y = y + 10) {
        for(int x = 0; x < window_width; x = x + 10) {
            color_buffer[(window_width * y) + x] = 0xFF333333;
        }
    }
}

void draw_pixel(int x, int y, uint32_t color) {
    if( x >= 0 && x < window_width && y >= 0 && y < window_height) {
        color_buffer[(window_width * y) + x] = color;
    }
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
    for(int i = 0; i < width; i++) {
        for(int j = 0; j < width; j++) {
            int current_x = x + i;
            int current_y = y + j;
            draw_pixel(current_x, current_y, color);
        }
    }
}

void render_color_buffer(void) {
    // update the texture to have the colors in the color_buffer
    SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer, (int)(window_width * sizeof(uint32_t)));
    
    // send the texture to the back-buffer
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color) {
    for(int y = 0; y < window_height; y++) {
        for(int x = 0; x < window_width; x++) {
            color_buffer[(window_width * y) + x] = color;
        }
    }
}

void destroy_window(void)
{
    // reverse order of everything you have created
    free(color_buffer); // free the buffer
    SDL_DestroyRenderer(renderer); // free renderer
    SDL_DestroyWindow(window); // free window
    SDL_Quit();
}

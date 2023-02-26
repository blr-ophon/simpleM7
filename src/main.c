#define CAMERA_HEIGHT 512
#define CAMERA_WIDTH 1024
#define CAMERA_DISTANCE 100

#define PI 3.14159265359
#define FOV 66
#define FOV_ANGLE (FOV*PI)/180
#define UNIT_ANGLE FOV_ANGLE/CAMERA_WIDTH

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

//TODO: points with negative x and z values produce noise on the screen
//TODO: camera distance still has to be higher than the plane to work.
//      This can be fixed by checking the deltas
//TODO: Controls to move player around. Needs speed parameters and bla bla bla
//TODO: Check negative or obtuse angles to see if the formula works

SDL_Window *window;
SDL_Renderer *renderer; 
SDL_Color array[64*64];

struct playerObj{
    float xPos;
    float yPos;
    float zPos;
    float angle;
};

struct playerObj Player = {
    20,
    512,
    20,
    0
};

bool running = 1;

float getDistancePlaneXZ(float yPos, float cam_yPos){ 
    if(cam_yPos >= 0){
        return CAMERA_DISTANCE*(yPos/(yPos - cam_yPos));
    }
    return CAMERA_DISTANCE*(yPos/(yPos + cam_yPos));
}

void getFloorPoint(float projPoint[], float cam_yPos, float angle){
    //receives the "scanline" or cam_yPos, an angle and returns the projected point
    //on the floor texture. Expects the caller to calculate the angle by adding and 
    //subtracting unit angles (based on FOV) from the player view angle.
    float delta = getDistancePlaneXZ(Player.yPos, cam_yPos);
    projPoint[0] = fmod(delta*cos(angle) + Player.xPos, 64);
    projPoint[1] = fmod(delta*sin(angle) + Player.zPos, 64);
}

void drawPoint(float projPoint[], int w, int h){
    int z = projPoint[1];
    int x = projPoint[0];
    SDL_Color color = array[z*64 + x];
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_Rect pixel = {w, CAMERA_HEIGHT - h, 1, 1}; //corrects y axis inversion
    SDL_RenderFillRect(renderer, &pixel);
}

void renderCameraPlane(void){
    float rayAngle = Player.angle;
    float texturePoint[2];
    //right side rays
    for(int h = 0; h < CAMERA_HEIGHT; h++){
        for(int w = CAMERA_WIDTH/2; w < CAMERA_WIDTH; w++){
            getFloorPoint(texturePoint, h, rayAngle);
            drawPoint(texturePoint, w, h);
            rayAngle += UNIT_ANGLE;

        }
        rayAngle = Player.angle; 
    }
    //left side rays
    rayAngle -= UNIT_ANGLE;
    for(int h = 0; h < CAMERA_HEIGHT; h++){
        for(int w = CAMERA_WIDTH/2 - 1; w >= 0; w--){
            getFloorPoint(texturePoint, h, rayAngle);
            drawPoint(texturePoint, w, h);
            rayAngle -= UNIT_ANGLE;
        }
        rayAngle = Player.angle; 
    }
    SDL_RenderPresent(renderer);
}

void process_input(void){
    SDL_Event event;
    SDL_PollEvent(&event);
    switch(event.type){
        case SDL_QUIT:
            break;
		case SDL_KEYDOWN:
			if(event.key.keysym.sym == SDLK_ESCAPE) {running = false;}
        default:
            break;
    }
}

int main(void){
    FILE *f = fopen("./textures/markitoouro.bmp", "rb");
    if(!f) fprintf(stdin, "\nNo texture loaded\n");

    uint8_t info[54];
    fread(info, sizeof(uint8_t), 54, f);

    uint32_t start_offset = info[10];
    uint32_t width = info[18];
    uint32_t height = info[22];
    uint16_t bpp = info[28];
    uint32_t compression = info[30];

    printf("WIDTH: %d\n", width);
    printf("HEIGHT: %d\n", height);
    printf("BPP:%d\n", bpp);
    printf("COMPRESSION: %d\n", compression);

    fseek(f, start_offset, SEEK_SET);
    for(int i = height-1; i >= 0; i--){ //FOLLOWING LOOP WORKS FOR BPP = 24
        for(int j = width*4-4; j >= 0; j-=4){
            fread(&array[i*width + j/4].b, 1, 1, f);
            fread(&array[i*width + j/4].g, 1, 1, f);
            fread(&array[i*width + j/4].r, 1, 1, f);
        }
    }

    window = SDL_CreateWindow(
            "BMPviewer",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1024,
            512,
            SDL_WINDOW_BORDERLESS
            );
    renderer = SDL_CreateRenderer(window, -1, 0);

    while(running){
        process_input();
        renderCameraPlane();
    }

    /*
    while(running){
        for(int y0 = 0; y0 < height; y0 ++){
            for(int x0 = 0; x0 < width; x0++){
                int x = x0;
                int y = y0;
                SDL_Color color = array[y0*width + x0];
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
                SDL_Rect pixel = {x0*4, y0*4, 4, 4};
                SDL_RenderFillRect(renderer, &pixel);

            }
        }
        SDL_RenderPresent(renderer);
        process_input();
    }
    */

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}



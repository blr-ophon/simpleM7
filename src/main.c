#include "main.h"

SDL_Window *window;
SDL_Renderer *renderer; 
SDL_Color array[64*64];

struct playerObj PlayerObj = {
    20,
    PLAYER_HEIGHT,
    20,
    0,
    PLAYER_HEIGHT-1
};
bool running = 1;
bool keymap[8];
int last_frame_t = 0;

//Gets the distance in plane xz between the projection of the player
//and the point the player is seeing. Distance in xz depends only on
//playeryPos and the scanline y coordinate
int getDistancePlaneXZ(float yPos, float cam_yPos){ 
        return CAMERA_DISTANCE*(yPos/(yPos - cam_yPos));
}

//receives the "scanline" or cam_yPos, an angle and returns the projected point
//on the floor texture. Expects the caller to calculate the angle by adding and 
//subtracting unit angles (based on FOV) from the player view angle.
void getFloorPoint(float projPoint[], float cam_yPos, float angle){
    //TODO: Implement fog and increase performance by ignoring points too distant
    //TODO: There should be a more efficient way to do this. Give it some thought
    float delta = getDistancePlaneXZ(PlayerObj.yPos, cam_yPos);
    projPoint[0] = fmod(delta*cos(angle) + PlayerObj.xPos, 64);
    projPoint[0] = fabs(projPoint[0]);
    projPoint[1] = fmod(delta*sin(angle) + PlayerObj.zPos, 64);
    projPoint[1] = fabs(projPoint[1]);
}

void drawPoint(float projPoint[], int w, int h){
    int x = projPoint[0];
    int z = projPoint[1];
    SDL_Color color = array[z*64 + x];
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_Rect pixel = {2*w, 2*h, 2, 2}; //corrects y axis inversion
    SDL_RenderFillRect(renderer, &pixel);
}

void renderCameraPlane(void){
    float rayAngle = PlayerObj.angle;
    float texturePoint[2];
    float cam_yPos = PlayerObj.cam_top;
    //right side rays
    for(int h = 0; h < CAMERA_HEIGHT; h++, cam_yPos--){
        for(int w = CAMERA_WIDTH/2; w < CAMERA_WIDTH; w++){
            getFloorPoint(texturePoint, cam_yPos, rayAngle);
            drawPoint(texturePoint, w, h);
            rayAngle += UNIT_ANGLE;
        }
        rayAngle = PlayerObj.angle; 
    }
    cam_yPos = PlayerObj.cam_top;

    //left side rays
    rayAngle -= UNIT_ANGLE;
    for(int h = 0; h < CAMERA_HEIGHT; h++, cam_yPos--){
        for(int w = CAMERA_WIDTH/2 - 1; w >= 0; w--){
            getFloorPoint(texturePoint, cam_yPos, rayAngle);
            drawPoint(texturePoint, w, h);
            rayAngle -= UNIT_ANGLE;
        }
        rayAngle = PlayerObj.angle; 
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
			if(event.key.keysym.sym == SDLK_ESCAPE) {running = false;}
			if(event.key.keysym.sym == SDLK_d) {keymap[KEY_TILT_RIGHT] = 1;}
			if(event.key.keysym.sym == SDLK_a) {keymap[KEY_TILT_LEFT] = 1;}
			if(event.key.keysym.sym == SDLK_w) {keymap[KEY_FORWARD] = 1;}
			if(event.key.keysym.sym == SDLK_s) {keymap[KEY_BACKWARD] = 1;}
			if(event.key.keysym.sym == SDLK_SPACE) {keymap[KEY_ASCEND] = 1;}
			if(event.key.keysym.sym == SDLK_LSHIFT) {keymap[KEY_DESCEND] = 1;}
			if(event.key.keysym.sym == SDLK_j) {keymap[KEY_LOOK_DOWN] = 1;}
			if(event.key.keysym.sym == SDLK_u) {keymap[KEY_LOOK_UP] = 1;}
            break;
		case SDL_KEYUP:
			if(event.key.keysym.sym == SDLK_d) {keymap[KEY_TILT_RIGHT] = 0;}
			if(event.key.keysym.sym == SDLK_a) {keymap[KEY_TILT_LEFT] = 0;}
			if(event.key.keysym.sym == SDLK_w) {keymap[KEY_FORWARD] = 0;}
			if(event.key.keysym.sym == SDLK_s) {keymap[KEY_BACKWARD] = 0;}
			if(event.key.keysym.sym == SDLK_SPACE) {keymap[KEY_ASCEND] = 0;}
			if(event.key.keysym.sym == SDLK_LSHIFT) {keymap[KEY_DESCEND] = 0;}
			if(event.key.keysym.sym == SDLK_j) {keymap[KEY_LOOK_DOWN] = 0;}
			if(event.key.keysym.sym == SDLK_u) {keymap[KEY_LOOK_UP] = 0;}
            break;
        default:
            break;
    }
    if(keymap[KEY_TILT_LEFT]) {PlayerObj.angle -= TURN_SPEED*UNIT_ANGLE;}
    if(keymap[KEY_TILT_RIGHT]) {PlayerObj.angle += TURN_SPEED*UNIT_ANGLE;}

    if(keymap[KEY_FORWARD]){
        PlayerObj.xPos += PLAYER_SPEED*cos(PlayerObj.angle);
        PlayerObj.zPos += PLAYER_SPEED*sin(PlayerObj.angle);
    }
    if(keymap[KEY_BACKWARD]){
        PlayerObj.xPos -= PLAYER_SPEED*cos(PlayerObj.angle);
        PlayerObj.zPos -= PLAYER_SPEED*sin(PlayerObj.angle);
    }
    if(keymap[KEY_ASCEND]){
        PlayerObj.yPos += 2*PLAYER_SPEED;
        PlayerObj.cam_top += 2*PLAYER_SPEED;
    }
    if(keymap[KEY_DESCEND]){
        PlayerObj.yPos -= 2*PLAYER_SPEED;
        if(PlayerObj.yPos < PLAYER_HEIGHT) PlayerObj.yPos = PLAYER_HEIGHT;
        PlayerObj.cam_top -= 2*PLAYER_SPEED;
    }
    if(keymap[KEY_LOOK_DOWN]){
        PlayerObj.cam_top -= PLAYER_SPEED;
    }
    if(keymap[KEY_LOOK_UP]){
        PlayerObj.cam_top += PLAYER_SPEED;
    }
}
                                        //

void update(void){
    int time_passed = SDL_GetTicks() - last_frame_t;
    int time_to_wait = FRAME_TARGET_TIME - time_passed;
    if(time_to_wait > 0){
        SDL_Delay(time_to_wait);
    }
}

int main(void){
    FILE *f = fopen("./textures/markitoouro.bmp", "rb");
    if(!f) {fprintf(stdin, "\nNo texture loaded\n"); exit(EXIT_SUCCESS);}

    uint8_t info[54];
    fread(info, sizeof(uint8_t), 54, f);

    uint32_t start_offset = info[10];
    uint32_t width = info[18];
    uint32_t height = info[22];
    uint16_t bpp = info[28];
    uint32_t compression = info[30];

    if(compression == 0 || bpp != 24){
        fprintf(stdin, "\nInvalid texture (compression or BPP)\n");
    }

    fseek(f, start_offset, SEEK_SET);
    for(int i = height-1; i >= 0; i--){ //FOLLOWING LOOP WORKS FOR BPP = 24
        for(int j = width*4-4; j >= 0; j-=4){
            fread(&array[i*width + j/4].b, 1, 1, f);
            fread(&array[i*width + j/4].g, 1, 1, f);
            fread(&array[i*width + j/4].r, 1, 1, f);
        }
    }

    window = SDL_CreateWindow(
            "Simple MODE 7 engine",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1024,
            512,
            SDL_WINDOW_BORDERLESS
            );
    renderer = SDL_CreateRenderer(window, -1, 0);

    while(running){
        process_input();
        update();
        renderCameraPlane();
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}



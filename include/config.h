#ifndef CONFIG_H
#define CONFIG_H

#define CAMERA_HEIGHT 256
#define CAMERA_WIDTH 512
#define CAMERA_DISTANCE 10
#define PLAYER_HEIGHT 256

#define PI 3.14159265359
#define FOV 66
#define FOV_ANGLE (FOV*PI)/180
#define UNIT_ANGLE FOV_ANGLE/CAMERA_WIDTH

#define KEY_TILT_RIGHT 0
#define KEY_TILT_LEFT 1
#define KEY_FORWARD 2
#define KEY_BACKWARD 3
#define KEY_ASCEND 4
#define KEY_DESCEND 5
#define KEY_LOOK_UP 6
#define KEY_LOOK_DOWN 7
#define KEY_CAMERA_PLUS 8
#define KEY_CAMERA_MINUS 9

#define FPS 60                          //frequencia
#define FRAME_TARGET_TIME (1000 / FPS)  //Periodo (em ms)

#define PLAYER_SPEED 15
#define TURN_SPEED 50

#endif

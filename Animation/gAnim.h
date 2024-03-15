#pragma once 

#include <SDL.h>
#include <stdbool.h>

typedef SDL_Point(*FrameToDistance)(int, int); //Params: First is current animation, Last is total animation count
typedef SDL_Color(*FrameToColor)(int, int);

typedef struct gAnim {
    int start_kframe, frame_count;
    float frame_p_kframe;
    SDL_Point origin;
    FrameToDistance ftd_fn;
    FrameToColor ftc_fn;
} gAnim;

gAnim* gAnimCreate(int start_kframe, int frame_count, int kframe_count, SDL_Point origin, FrameToDistance ftd_fn, FrameToColor ftc_fn);
void gAnimPerformFrame(gAnim* anim, int frame, int frame_w, int frame_h, SDL_Rect* clip, SDL_Point* pos, SDL_Color* color);
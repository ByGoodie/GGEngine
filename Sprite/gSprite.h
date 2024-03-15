#pragma once 

#include "gAnim.h"

typedef int(*AnimTransition)(int);

typedef struct gSprite {
    SDL_Texture* sprite_sheet;
    int anim_count, _anim, _frame, sheet_w, sheet_h;
    AnimTransition _animt_fn;
    bool repeat;
    SDL_Rect render_space, clip;
    SDL_Point anim_coord;
    SDL_Color color;
    float rotation_angle;
    gAnim** _anims;
} gSprite;

gSprite* gSpriteCreate(SDL_Texture* sprite_sheet, gAnim** anims, int anim_count, AnimTransition animt_fn, SDL_Rect clip, SDL_Rect render_space);
bool gSpriteNextFrame(gSprite* sprite);
bool gSpriteSetAnim(gSprite* sprite, int anim);
void gSpriteDraw(gSprite* sprite, SDL_Renderer* renderer);
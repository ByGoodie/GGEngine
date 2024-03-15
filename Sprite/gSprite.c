#include "../include/gSprite.h"

gSprite* gSpriteCreate(SDL_Texture* sprite_sheet, gAnim** anims, int anim_count, AnimTransition animt_fn, SDL_Rect clip, SDL_Rect render_space) {
	gSprite* sprite = (gSprite*)malloc(sizeof(gSprite));

	if (sprite == NULL)
		return NULL;

	sprite->_anims = anims;
	sprite->anim_count = anim_count;
	sprite->_animt_fn = animt_fn;
	sprite->clip = clip;
	sprite->render_space = render_space;
	sprite->rotation_angle = 0;
	sprite->sprite_sheet = sprite_sheet;
	sprite->color.r = sprite->color.g = sprite->color.b = sprite->color.a = 255;
	sprite->_frame = 0;
	sprite->_anim = -1;

	SDL_QueryTexture(sprite_sheet, NULL, NULL, &sprite->sheet_w, &sprite->sheet_h);

	return sprite;
}

bool gSpriteNextFrame(gSprite* sprite) {
	bool result = false;

	if (++sprite->_frame >= sprite->_anims[sprite->_anim]->frame_count) {
		sprite->anim_coord.x = sprite->anim_coord.y = 0;
		sprite->_anim = sprite->_animt_fn(sprite->_anim);
		sprite->_frame = 0;
		result = true;
	}

	gAnimPerformFrame(sprite->_anims[sprite->_anim], sprite->_frame, sprite->sheet_w, sprite->sheet_h, &sprite->clip, &sprite->anim_coord, &sprite->color);

	return result;
}

bool gSpriteSetAnim(gSprite* sprite, int anim) {
	if (sprite->anim_count <= anim)
		return false;

	sprite->anim_coord.x = sprite->anim_coord.y = 0;
	sprite->_frame = 0;
	sprite->_anim = anim;
	gAnimPerformFrame(sprite->_anims[sprite->_anim], sprite->_frame, sprite->sheet_w, sprite->sheet_h, &sprite->clip, &sprite->anim_coord, &sprite->color);

	return true;
}

void gSpriteDraw(gSprite* sprite, SDL_Renderer* renderer) {
	SDL_Rect absolute_coord = { sprite->render_space.x + sprite->anim_coord.x, sprite->render_space.y + sprite->anim_coord.y, sprite->render_space.w, sprite->render_space.h };
	SDL_SetTextureColorMod(sprite->sprite_sheet, sprite->color.r, sprite->color.g, sprite->color.b);
	SDL_RenderCopyEx(renderer, sprite->sprite_sheet, &sprite->clip, &absolute_coord, sprite->rotation_angle, NULL, SDL_FLIP_NONE);
 }
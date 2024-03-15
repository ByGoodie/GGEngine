#include "../include/gAnim.h"
#include "../include/gUtils.h"

gAnim* gAnimCreate(int start_kframe, int frame_count, int kframe_count, SDL_Point origin, FrameToDistance ftd_fn, FrameToColor ftc_fn) {
	gAnim* anim = (gAnim*)malloc(sizeof(gAnim));

	if (anim == NULL)
		return NULL;

	anim->frame_count = frame_count;
	anim->frame_p_kframe = frame_count / (float)kframe_count;
	anim->start_kframe = start_kframe;
	anim->origin = origin;
	anim->ftd_fn = ftd_fn;
	anim->ftc_fn = ftc_fn;

	return anim;
}

void gAnimPerformFrame(gAnim* anim, int frame, int sprite_w, int sprite_h, SDL_Rect* clip, SDL_Point* pos, SDL_Color* color) {
	if (anim->ftd_fn != NULL && pos != NULL) {
		*pos = anim->origin;

		SDL_Point distance = anim->ftd_fn(frame, anim->frame_count);
		pos->x += distance.x;
		pos->y += distance.y;
	}
	if (anim->ftc_fn != NULL && color != NULL) {
		*color = anim->ftc_fn(frame, anim->frame_count);
	}

	int kframe = anim->start_kframe + (int)(frame / anim->frame_p_kframe);

	clip->x = (kframe % (sprite_w / clip->w)) * clip->w;
	clip->y = (kframe / (sprite_w / clip->w)) * clip->h;
}
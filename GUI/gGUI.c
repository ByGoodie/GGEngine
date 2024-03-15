#include "../include/gGUI.h"
#include "../include/gVec.h"
#include "../include/gUtils.h"

static int check_val = 0;

static bool gGUIMaxHeightCheckIf(void* element) {
	gGUIVA* va = gGUITo(element, GGUI_OFFSET_OBJ);

	if (va->location.y == check_val) {
		return true;
	}

	return false;
}

static bool gGUIMaxHeightGetInt(void* element) {
	gGUIVA* va = gGUITo(element, GGUI_OFFSET_OBJ);

	return va->size.y;
}

static bool gGUIMaxWidthCheckIf(void* element) {
	gGUIVA* va = gGUITo(element, GGUI_OFFSET_OBJ);

	if (va->location.x == check_val) {
		return true;
	}

	return false;
}

static bool gGUIMaxWidthGetInt(void* element) {
	gGUIVA* va = gGUITo(element, GGUI_OFFSET_OBJ);

	return va->size.x;
}

static bool gGUIContentMaxHeightInsideSizeCheckIf(void* element) {
	gGUIVA* va = gGUITo(element, GGUI_OFFSET_OBJ);

	if (va->location.y + va->size.y <= check_val)
		return true;

	return false;
}

static bool gGUIContentMaxHeightGetInt(void* element) {
	gGUIVA* va = gGUITo(element, GGUI_OFFSET_OBJ);

	return va->location.y + va->size.y;
}

static bool gGUIContentMaxWidthInsideSizeCheckIf(void* element) {
	gGUIVA* va = gGUITo(element, GGUI_OFFSET_OBJ);

	if (va->location.x + va->size.x <= check_val)
		return true;

	return false;
}

static bool gGUIContentMaxWidthGetInt(void* element) {
	gGUIVA* va = gGUITo(element, GGUI_OFFSET_OBJ);

	return va->location.x + va->size.x;
}

static gcolor_t gGUIGetStateColor(gcolor_t color, gGUIState state) {
	switch (state) {
	case GGUI_STATE_CLICKING:
		return (gcolor_t) { color.r * 0.8, color.g * 0.8, color.b * 0.8, color.a };

	case GGUI_STATE_HOVERING:
		return (gcolor_t) { color.r * 0.9, color.g * 0.9, color.b * 0.9, color.a };
	}

	return color;
}

gmargin_t gGUIMarginFromRTLB(int mr, int mt, int ml, int mb) {
	gmargin_t r = {mr, mt, ml, mb};
	return r;
}

gpadding_t gGUIPaddingFromRTLB(int pr, int pt, int pl, int pb) {
	gpadding_t r = {pr, pt, pl, pb};
	return r;
}

gGUIContainer* gGUICreateContainer(int x, int y, int w, int h, guint32 c_align_x, guint32 c_align_y, guint32 c_direction) {
	static guint32 id_counter = 0;

	gGUIObject* obj = (gGUIObject*)calloc(1, sizeof(gGUIObject) + sizeof(gGUIContainer));

	if (obj == NULL) return NULL;

	obj->is_active = true;
	obj->is_visible = true;
	obj->is_draggable = false;
	obj->type = GGUI_CONTAINER;
	obj->id = id_counter++;
	obj->state = GGUI_STATE_IDLE;

	gGUIContainer* result = gGUITo(obj, GGUI_OFFSET_OBJ);

	result->va.location.x = x;
	result->va.location.y = y;
	result->va.c_align[0] = c_align_x;
	result->va.c_align[1] = c_align_y;
	result->va.c_direction = c_direction;
	result->va.size_format[0] = (w < 0) ? (w) : (GGUI_SIZE_NONE);
	result->va.size_format[1] = (h < 0) ? (h) : (GGUI_SIZE_NONE);

	result->children = gVecNew(sizeof(gGUIObject*), 0);

	/*va_list list;
	va_start(list, initial_children_count);
	
	gGUIObject** children = result->children->buffer;

	for (size_t i = 0; i < initial_children_count; i++) {
		children[i] = va_arg(list, gGUIObject*);
	}
	
	va_end(list);*/

	return result;
}

gGUILabel* gGUICreateLabel(char* text, gfont_t* font, grenderer_t* renderer, gcolor_t color, int x, int y, int w, gGUISizeFormat size_f_x, gGUISizeFormat size_f_y, float per_x, float per_y) {
	static guint32 id_counter = 0;

	gGUIObject* obj = (gGUIObject*)calloc(1, sizeof(gGUIObject) + sizeof(gGUILabel));

	if (obj == NULL) return NULL;

	

	gsurface_t* surface = TTF_RenderText_Blended(font, text, color);

	if (surface == NULL) {
		free(obj);
		return NULL;
	}

	gtexture_t* texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_FreeSurface(surface);
	if (texture == NULL) {
		free(obj);
		return NULL;
	}

	gGUILabel* label = gGUITo(obj, GGUI_OFFSET_OBJ);

	label->va.color = color;
	label->va.location.x = x;
	label->va.location.y = y;
	label->texture = texture;
	label->text = text;
	label->va.size_format[0] = size_f_x;
	label->va.size_format[1] = size_f_y;
	label->va.size_percentage[0] = per_x;
	label->va.size_percentage[1] = per_y;
	
	SDL_QueryTexture(label->texture, NULL, NULL, &label->va.size.x, &label->va.size.y);

	obj->is_active = true;
	obj->is_visible = true;
	obj->is_draggable = false;
	obj->type = GGUI_LABEL;
	obj->id = id_counter++;
	obj->state = GGUI_STATE_IDLE;

	return label;
}

gGUIButton* gGUICreateButton(gGUIContainer* container, gtexture_t* bg, gcolor_t color, int x, int y, int w, int h, float per_x, float per_y, gmargin_t margin, gpadding_t padding, gGUIOnClick on_click) {
	static guint32 id_counter = 0;

	gGUIObject* obj = (gGUIObject*)calloc(1, sizeof(gGUIObject) + sizeof(gGUIButton));

	if (obj == NULL) return NULL;

	gGUIButton* btn = gGUITo(obj, GGUI_OFFSET_OBJ);
	
	obj->is_active = true;
	obj->is_visible = true;
	obj->is_draggable = false;
	obj->type = GGUI_BUTTON;
	obj->id = id_counter++;
	obj->state = GGUI_STATE_IDLE;
	
	btn->bg_texture = bg;
	btn->container = container;

	btn->va.location.x = x;
	btn->va.location.y = y;
	btn->va.color = color;
	btn->va.margin = margin;
	btn->va.padding = padding;
	btn->acts.on_click = on_click;
	btn->va.size_format[0] = (w < 0) ? (w) : (GGUI_SIZE_NONE);
	btn->va.size_format[1] = (h < 0) ? (w) : (GGUI_SIZE_NONE);
	btn->va.size_percentage[0] = per_x;
	btn->va.size_percentage[1] = per_y;
	btn->va.size.x = w;
	btn->va.size.y = h;

	/*if (w == GGUI_SIZE_CONTENT) {
		btn->va.size.x = label->va.size.x + padding.x + padding.w;
	}
	if (h == GGUI_SIZE_CONTENT) {
		btn->va.size.y = label->va.size.y + padding.y + padding.h;
	}

	if (label->va.location.x == GGUI_ALIGN_START) {
		label->va.location.x = padding.w;
	}
	else if (label->va.location.x == GGUI_ALIGN_CENTER) {
		label->va.location.x = padding.w + (w + padding.x + padding.w - label->va.size.x) / 2;
	}
	else if (label->va.location.x == GGUI_ALIGN_END) {
		label->va.location.x = padding.w + w + padding.x - label->va.size.x;
	}

	
	if (label->va.location.y == GGUI_ALIGN_START) {
		label->va.location.y = padding.y;
	}
	else if (label->va.location.y == GGUI_ALIGN_CENTER) {
		label->va.location.y = (w + padding.y + padding.h - label->va.size.y) / 2;
	}
	else if (label->va.location.y == GGUI_ALIGN_END) {
		label->va.location.y = padding.y + h + padding.h - label->va.size.y;
	}
	*/
	return btn;
}
gGUIIcon* gGUICreateIcon(gtexture_t* texture, int x, int y, int w, int h, float per_x, float per_y, gcolor_t color) {
	gGUIIcon* icon = (gGUIIcon*)calloc(1, sizeof(gGUIIcon));

	if (icon == NULL) return NULL;

	icon->texture = texture;
	icon->va.location.x = x;
	icon->va.location.y = y;
	icon->va.size.x = w;
	icon->va.size.y = h;
	icon->va.color = color;
	icon->va.size_format[0] = (w < 0) ? (w) : (GGUI_SIZE_NONE);
	icon->va.size_format[1] = (h < 0) ? (h) : (GGUI_SIZE_NONE);
	icon->va.size_percentage[0] = per_x;
	icon->va.size_percentage[1] = per_y;

	return icon;
}

void gGUIDrawContainer(gGUIContainer* container, grenderer_t* renderer, glocation_t rel) {
	if (container == NULL || renderer == NULL) 
		return;

	{
		gGUIObject* obj = gGUITo(container, -GGUI_OFFSET_OBJ);
		if (!obj->is_visible) return;
	}

	glocation_t loc = {rel.x + container->va.location.x, rel.y + container->va.location.y};

	check_val = container->va.size.x;

	if (container->va.c_align[0] == GGUI_ALIGN_CENTER && (container->va.c_direction == GGUI_DIRECTION_COLUMN || container->va.c_direction == GGUI_DIRECTION_ROW)) {
		loc.x += (container->va.size.x - gMaxIntOf(container->children->buffer, container->children->size, gGUIContentMaxWidthInsideSizeCheckIf, gGUIContentMaxWidthGetInt)) / 2;
	}
	else if (container->va.c_align[0] == GGUI_ALIGN_END && (container->va.c_direction == GGUI_DIRECTION_COLUMN || container->va.c_direction == GGUI_DIRECTION_ROW)) {
		loc.x += container->va.size.x - gMaxIntOf(container->children->buffer, container->children->size, gGUIContentMaxWidthInsideSizeCheckIf, gGUIContentMaxWidthGetInt);
	}

	check_val = container->va.size.y;

	if (container->va.c_align[1] == GGUI_ALIGN_CENTER && (container->va.c_direction == GGUI_DIRECTION_COLUMN || container->va.c_direction == GGUI_DIRECTION_ROW)) {
		loc.y += (container->va.size.y - gMaxIntOf(container->children->buffer, container->children->size, gGUIContentMaxHeightInsideSizeCheckIf, gGUIContentMaxHeightGetInt)) / 2;
	}
	else if (container->va.c_align[1] == GGUI_ALIGN_END && (container->va.c_direction == GGUI_DIRECTION_COLUMN || container->va.c_direction == GGUI_DIRECTION_ROW)) {
		loc.y += container->va.size.y - gMaxIntOf(container->children->buffer, container->children->size, gGUIContentMaxHeightInsideSizeCheckIf, gGUIContentMaxHeightGetInt);
	}

	gGUIObject** children = container->children->buffer;

	for (int i = 0; i < container->children->size; i++) {
		switch (children[i]->type) {
		case GGUI_LABEL:
			gGUIDrawLabel(gGUITo(children[i], GGUI_OFFSET_OBJ), renderer, loc);
			break;

		case GGUI_BUTTON:
			gGUIDrawButton(gGUITo(children[i], GGUI_OFFSET_OBJ), renderer, loc);
			break;

		case GGUI_CONTAINER:
			gGUIDrawContainer(gGUITo(children[i], GGUI_OFFSET_OBJ), renderer, loc);
			break;
		}
	}	
}

void gGUIDrawLabel(gGUILabel* label, grenderer_t* renderer, glocation_t rel) {
	if (label == NULL || renderer == NULL) 
		return;

	{
		gGUIObject* obj = gGUITo(label, -GGUI_OFFSET_OBJ);
		if (!obj->is_visible) return;
	}

	grect_t rect = {rel.x + label->va.location.x, rel.y + label->va.location.y, label->va.size.x, label->va.size.y};

	if (label->texture != NULL) SDL_RenderCopy(renderer, label->texture, NULL, &rect);
}

void gGUIDrawButton(gGUIButton* btn, grenderer_t* renderer, glocation_t rel) {
	if (btn == NULL || renderer == NULL) 
		return;

	
	gGUIObject* obj = gGUITo(btn, -GGUI_OFFSET_OBJ);
	if (!obj->is_visible) return;


	grect_t rect = {rel.x + btn->va.location.x, rel.y + btn->va.location.y, btn->va.size.x, btn->va.size.y};

	gcolor_t color = gGUIGetStateColor(btn->va.color, obj->state);
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	if (btn->bg_texture != NULL) SDL_RenderCopy(renderer, btn->bg_texture, NULL, &rect);
	else SDL_RenderFillRect(renderer, &rect);

	glocation_t a_rel = {rect.x, rect.y};

	gGUIDrawContainer(btn->container, renderer, a_rel);
}

void gGUIDrawIcon(gGUIIcon* icon, grenderer_t* renderer, glocation_t rel) {
	if (icon == NULL || renderer == NULL || icon->texture == NULL) return;

	grect_t rect = {rel.x + icon->va.location.x, rel.y + icon->va.location.y, icon->va.size.x, icon->va.size.y};
	
	SDL_RenderCopy(renderer, icon->texture, NULL, &rect);
}

void gGUIDestroyContainer(gGUIContainer* container) {
	gGUIObject** children = container->children->buffer;

	for (int i = 0; i < container->children->size; i++) {
		switch (children[i]->type) {
		case GGUI_CONTAINER:
			gGUIDestroyContainer(gGUITo(children[i], GGUI_OFFSET_OBJ));
			break;

		case GGUI_LABEL:
			gGUIDestroyLabel(gGUITo(children[i], GGUI_OFFSET_OBJ));
			break;

		case GGUI_BUTTON:
			gGUIDestroyButton(gGUITo(children[i], GGUI_OFFSET_OBJ));
			break;
		}
	}
}

void gGUIDestroyLabel(gGUILabel* label) {
	if (label == NULL) return;

	if (label->texture != NULL) SDL_DestroyTexture(label->texture);

	free(gGUITo(label, -GGUI_OFFSET_OBJ));
}

void gGUIDestroyButton(gGUIButton* btn) {
	if (btn == NULL) return;

	gGUIDestroyContainer(btn->container);

	free(gGUITo(btn, -GGUI_OFFSET_OBJ));
}

void gGUIDestroyIcon(gGUIIcon* icon) {
	if (icon == NULL) return;

	free(icon);
}

void gGUIContainerResized(gGUIContainer* cont, gsize_t p_size) {
	if (cont == NULL) return;
	
	switch (cont->va.size_format[0]) {
	case GGUI_SIZE_MAX:
		cont->va.size.x = p_size.x;
		break;

	case GGUI_SIZE_PERCENTAGE:
		cont->va.size.x = p_size.x * cont->va.size_percentage[0];
		break;

	}

	switch (cont->va.size_format[1]) {
	case GGUI_SIZE_MAX:
		cont->va.size.y = p_size.y;
		break;

	case GGUI_SIZE_PERCENTAGE:
		cont->va.size.y = p_size.y * cont->va.size_percentage[1];
		break;

	}

	gsize_t size = p_size;

	if (cont->va.size_format[0] != GGUI_SIZE_CONTENT)
		size.x = cont->va.size.x;
	if (cont->va.size_format[1] != GGUI_SIZE_CONTENT)
		size.y = cont->va.size.y;

	gGUIObject** children = cont->children->buffer;

	for (int i = 0; i < cont->children->size; i++) {
		switch (children[i]->type) {
		case GGUI_CONTAINER:
			gGUIContainerResized(gGUITo(children[i], GGUI_OFFSET_OBJ), size);
			break;

		case GGUI_LABEL:
			gGUILabelResized(gGUITo(children[i], GGUI_OFFSET_OBJ), size);
			break;

		case GGUI_BUTTON:
			gGUIButtonResized(gGUITo(children[i], GGUI_OFFSET_OBJ), size);
			break;
		}

		gGUIVA* lo_va = NULL;

		if (i > 0) {
			lo_va = gGUITo(children[i - 1], GGUI_OFFSET_OBJ);
		}

		gGUIVA* c_va = children[i];

		gGUIArrangeChild(&cont, c_va, cont->children, lo_va);
	}

	if (cont->va.size_format == GGUI_SIZE_CONTENT) {
		cont->va.size.x = gMaxIntOf(children, cont->children->size, gMaxIntOfCheckAlways, gGUIContentMaxWidthGetInt);
		cont->va.size.y = gMaxIntOf(children, cont->children->size, gMaxIntOfCheckAlways, gGUIContentMaxHeightGetInt);
	}
}

void gGUILabelResized(gGUILabel* lbl, gsize_t size) {
	if (lbl == NULL) return;

	gGUIVA* va = lbl;

	switch (va->size_format[0]) {
	case GGUI_SIZE_MAX:
		va->size.x = size.x;
		break;

	case GGUI_SIZE_PERCENTAGE:
		va->size.x = size.x * va->size_percentage[0] / 100;
		break;
	}

	switch (va->size_format[1]) {
	case GGUI_SIZE_MAX:
		va->size.y = size.y;
		break;

	case GGUI_SIZE_PERCENTAGE:
		va->size.y = size.y * va->size_percentage[1] / 100;
		break;
	}
}

void gGUIButtonResized(gGUIButton* btn, gsize_t p_size) {
	if (btn == NULL) return;

	gGUIVA* va = btn;

	switch (va->size_format[0]) {
	case GGUI_SIZE_MAX:
		va->size.x = p_size.x;
		break;

	case GGUI_SIZE_PERCENTAGE:
		va->size.x = p_size.x * va->size_percentage[0] / 100;
		break;
	}

	switch (va->size_format[1]) {
	case GGUI_SIZE_MAX:
		va->size.y = p_size.y;
		break;

	case GGUI_SIZE_PERCENTAGE:
		va->size.y = p_size.y * va->size_percentage[1] / 100;
		break;
	}


	gsize_t size = p_size;

	if (va->size_format[0] != GGUI_SIZE_CONTENT)
		size.x = btn->va.size.x;
	if (va->size_format[1] != GGUI_SIZE_CONTENT)
		size.y = btn->va.size.y;

	gGUIContainerResized(btn->container, size);

	if (va->size_format[0] == GGUI_SIZE_CONTENT)
		va->size.x = btn->container->va.size.x;

	if (va->size_format[1] == GGUI_SIZE_CONTENT)
		va->size.y = btn->container->va.size.y;
}

void gGUIPushChild(gGUIVA* p_va, gVec* vec, gGUIObject* obj, gsize_t w_size) {
	if (p_va == NULL ||vec == NULL || obj == NULL) return;

	if (!gVecPushBack(vec)) return;

	gGUIObject** objects = vec->buffer;

	objects[vec->size - 1] = gGUITo(obj, -GGUI_OFFSET_OBJ);

	gGUIVA* lo_va = NULL;

	if (vec->size > 1) {
		lo_va = gGUITo(objects[vec->size - 2], GGUI_OFFSET_OBJ);
	}

	gGUIVA* c_va = obj;

	gGUIArrangeChild(p_va, c_va, vec, lo_va);

	if (p_va->size_format == GGUI_SIZE_CONTENT) {
		p_va->size.x = gMaxIntOf(objects, vec->size, gMaxIntOfCheckAlways, gGUIContentMaxWidthGetInt);
		p_va->size.y = gMaxIntOf(objects, vec->size, gMaxIntOfCheckAlways, gGUIContentMaxHeightGetInt);
	}
}

void gGUIArrangeChild(gGUIVA* p_va, gGUIVA* c_va, gVec* vec, gGUIVA* rc_va) {
	if (p_va->c_direction == GGUI_DIRECTION_ROW || p_va->c_direction == GGUI_DIRECTION_ROWCOLUMN) {
		c_va->location.x = (rc_va == NULL) ? (0) : (rc_va->location.x + rc_va->size.x + rc_va->margin.x) + c_va->margin.w;
		c_va->location.y = (rc_va == NULL) ? (0) : (rc_va->location.y);

		if (rc_va != NULL && p_va->size_format[0] != GGUI_SIZE_CONTENT && p_va->c_direction == GGUI_DIRECTION_ROWCOLUMN) {
			check_val = rc_va->location.y;

			if (c_va->location.x + c_va->size.x > p_va->size.x) c_va->location.y = rc_va->location.y + gMaxIntOf(vec->buffer, vec->size, gGUIMaxHeightCheckIf, gGUIMaxHeightGetInt);
		}
	}
	else {
		c_va->location.y = (rc_va == NULL) ? (0) : (rc_va->location.y + rc_va->size.y + rc_va->margin.h) + c_va->margin.y;

		if (rc_va != NULL && p_va->size_format[0] != GGUI_SIZE_CONTENT && p_va->c_direction == GGUI_DIRECTION_COLUMNROW) {
			check_val = rc_va->location.x;

			if (c_va->location.y + c_va->size.y > p_va->size.y) c_va->location.x = rc_va->location.x + gMaxIntOf(vec->buffer, vec->size, gGUIMaxHeightCheckIf, gGUIMaxHeightGetInt);
		}
	}
}

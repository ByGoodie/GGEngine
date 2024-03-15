#include "gTypedefs.h"
#include "gVec.h"
#include <stdbool.h>
#include <stdarg.h>

#pragma warning(disable:4146)

/* TODO:
* 1. Fixing a bug about ROWCOLUMN and COLUMNROW directions.
* 2. Adding action system.
*/

typedef void(*gGUIOnClick)(void*, void*, bool); //object, user data, data about if the event is at the end.
typedef void(*gGUIOnHover)(void*, void*, bool); //object, user data, data about if the event is at the end.
typedef void(*gGUIOnHover)(void*, void*, bool); 
typedef void(*gGUIOnExtObj)(void*, grenderer_t*); //object, renderer //Ext = External

typedef grect_t gmargin_t;
typedef grect_t gpadding_t;

typedef enum gGUIState {
	GGUI_STATE_IDLE,
	GGUI_STATE_CLICKING,
	GGUI_STATE_DRAGGING,
	GGUI_STATE_HOVERING
} gGUIState;

typedef enum gGUIAlign {
	GGUI_ALIGN_START = -4,
	GGUI_ALIGN_CENTER,
	GGUI_ALIGN_END,
	GGUI_ALIGN_NONE
} gGUIAlign;

typedef enum gGUISizeFormat {
	GGUI_SIZE_MAX = -4,
	GGUI_SIZE_CONTENT, //Content format is only can be used for Container object. You can give it to the button for the use while constructing but won't be used during resizing and be acted like NONE.
	GGUI_SIZE_PERCENTAGE,
	GGUI_SIZE_NONE
} gGUISizeFormat;

typedef enum gGUIType {
	GGUI_CONTAINER,
	GGUI_LABEL,
	GGUI_BUTTON
} gGUIType;

typedef enum gGUIDirection {
	GGUI_DIRECTION_ROW,
	GGUI_DIRECTION_COLUMN,
	GGUI_DIRECTION_ROWCOLUMN,
	GGUI_DIRECTION_COLUMNROW
} gGUIDirection;

typedef struct gGUIObject {
	guint32 type, id; //The type indicates what is the object. You can make your own types to identify your own objects to this GUI library. Check the documentation for further information about making your own object.
	gGUIState state; //The circumstance of the object to determine the color of it.
	bool is_active, is_visible, is_draggable;
	struct gGUIObject* parent;
} gGUIObject;

typedef struct gGUIVA { //VA = Visual Attribute
	glocation_t location; //Location is relative to the parent.
	gcolor_t color;
	gsize_t size;
	gmargin_t margin;
	gpadding_t padding;
	gGUISizeFormat size_format[2];
	float size_percentage[2];
	gGUIAlign c_align[2];
	gGUIDirection c_direction;
} gGUIVA;

typedef struct gGUIActs { //Action callback functions for the user control.
	gGUIOnClick on_click;
} gGUIActs;

typedef struct gGUIIcon { //Doesn't count as an object
	gGUIVA va;
	gtexture_t* texture;
} gGUIIcon;

typedef struct gGUIBorder { //Doesn't count as an object
	guint32 width;
	gcolor_t color;
} gGUIBorder; 

typedef struct gGUIContainer {
	gGUIVA va;
	gVec* children;
} gGUIContainer;

typedef struct gGUILabel {
	gGUIVA va;
	gfont_t* font;
	gtexture_t* texture;
	char* text;
} gGUILabel;

typedef struct gGUIButton {
	gGUIVA va;
	gGUIActs acts;
	gGUIContainer* container;
	gtexture_t* bg_texture;
	gGUIBorder* border;
} gGUIButton;

//gGUIAlign can be used when providing x and y coordinates. If you provide them with gGUIAlign it will aligned by the selected.

gmargin_t gGUIMarginFromRTLB(int mr, int mt, int ml, int mb); //right top left bottom
gpadding_t gGUIPaddingFromRTLB(int mr, int mt, int ml, int mb); //right top left bottom						    
gGUIContainer* gGUICreateContainer(int x, int y, int w, int h, guint32 c_align_x, guint32 c_align_y, guint32 c_direction); //c = content
gGUILabel* gGUICreateLabel(char* text, gfont_t* font, grenderer_t* renderer, gcolor_t color, int x, int y, int w, gGUISizeFormat size_f_x, gGUISizeFormat size_f_y, float per_x, float per_y); //providing w with 0 will automaticly adjust the width according to the width of text.
gGUIButton* gGUICreateButton(gGUIContainer* container, gtexture_t* bg, gcolor_t color, int x, int y, int w, int h, float per_x, float per_y, gmargin_t margin, gpadding_t padding, gGUIOnClick on_click); //provide w and h -1 to fit the size to label's size. Providing label or icon with NULL means ignoring them.
gGUIIcon* gGUICreateIcon(gtexture_t* texture, int x, int y, int w, int h, float per_x, float per_y, gcolor_t color); //provide w and h with -1 if you want to size the icon with the texture's original size.
void gGUIDrawContainer(gGUIContainer* container, grenderer_t* renderer, glocation_t rel); //rel = relative
void gGUIDrawLabel(gGUILabel* label, grenderer_t* renderer, glocation_t rel);
void gGUIDrawButton(gGUIButton* btn, grenderer_t* renderer, glocation_t rel);
void gGUIDrawIcon(gGUIIcon* icon, grenderer_t* renderer, glocation_t rel);
void gGUIDestroyContainer(gGUIContainer* container);
void gGUIDestroyLabel(gGUILabel* container);
void gGUIDestroyButton(gGUIButton* container);
void gGUIDestroyIcon(gGUIIcon* icon);
void gGUIContainerResized(gGUIContainer* cont, gsize_t size);
void gGUILabelResized(gGUILabel* lbl, gsize_t size);
void gGUIButtonResized(gGUIButton* btn, gsize_t size);
void gGUIPushChild(gGUIVA* p_va, gVec* vec, gGUIObject* obj, gsize_t w_size); //p = parent, w = window
void gGUIArrangeChild(gGUIVA* p_va, gGUIVA* c_va, gVec* vec, gGUIVA* rc_va); //p = parent, c = child, rc = recent child

#define gGUITo(addr, offset) (((char*)(addr)) + (offset))
#define GGUI_OFFSET_OBJ sizeof(gGUIObject)
#define GGUI_OFFSET_VA sizeof(gGUIVA)
#define GGUI_OFFSET_ACTS sizeof(gGUIActs)

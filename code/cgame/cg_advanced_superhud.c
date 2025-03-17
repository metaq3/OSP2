#include "cg_advanced_superhud.h"
#include "cg_local.h"
#include "cg_superhud_private.h"
#include "qcommon.h"

// **************************************************************
// *                        BASE ELEMENT                        *
// **************************************************************

advancedSuperhudElementStyle_t* CG_ASHUD_CreateBaseElementStyle() 
{
    advancedSuperhudElementStyle_t *style;

    style = Z_Malloc(sizeof(advancedSuperhudElementStyle_t));

    OSP_MEMORY_CHECK(style);

    memset(style, 0, sizeof(advancedSuperhudElementStyle_t));

    return style;
}

void CG_ASHUD_ConstructBaseElement(advancedSuperhudBaseElement_t* element)
{
    memset(element, 0, sizeof(advancedSuperhudBaseElement_t));

    element->updateViewbox = (void (*)(void*))CG_ASHUD_UpdateBaseViewbox;
    element->render = (void (*)(void*))CG_ASHUD_RenderBaseElement;

    element->maxWidth = 65535.0f;
    element->maxHeight = 65535.0f;
}

void CG_ASHUD_UpdateBaseViewbox(advancedSuperhudBaseElement_t* element)
{
    element->width = Com_Clamp(element->minWidth, element->maxWidth, element->width);
    element->height = Com_Clamp(element->minHeight, element->maxHeight, element->height);
}

void CG_ASHUD_RenderBaseElement(advancedSuperhudBaseElement_t* element)
{
    float x, y, width, height, borderWidth;

    x = element->coord.named.x;
    y = element->coord.named.y;
    width = element->width;
    height = element->height;
    borderWidth = element->style->border.width;

    CG_AdjustFrom640(&x, &y, &width, &height);
    CG_AdjustFrom640(&borderWidth, NULL, NULL, NULL);
    CG_AdjustFrom640(&element->style->padding.left, &element->style->padding.right, &element->style->padding.top, &element->style->padding.bottom);

    width += element->style->padding.left + element->style->padding.right;
    height += element->style->padding.top + element->style->padding.bottom;

    if (element->style->border.width > 0)
    {
        trap_R_SetColor(element->style->border.color);

        // This is a spooky way to draw 4 lines, forming a stroke rectangle!
        trap_R_DrawStretchPic(x - borderWidth, y - borderWidth, borderWidth, height + borderWidth * 2, 0, 0, 0, 0, cgs.media.whiteShader);
        trap_R_DrawStretchPic(x + width, y - borderWidth, borderWidth, height + borderWidth * 2, 0, 0, 0, 0, cgs.media.whiteShader);
        trap_R_DrawStretchPic(x, y - borderWidth, width, borderWidth, 0, 0, 0, 0, cgs.media.whiteShader);
        trap_R_DrawStretchPic(x, y + height, width, borderWidth, 0, 0, 0, 0, cgs.media.whiteShader);
    }

    trap_R_SetColor(element->style->background.color);

    if (element->style->background.image)
    {
        trap_R_DrawStretchPic(x, y, width, height, 0, 0, 1, 1, element->style->background.image);
    }
    else
    {
        trap_R_DrawStretchPic(x, y, width, height, 0, 0, 0, 0, cgs.media.whiteShader);
    }

    trap_R_SetColor(NULL);
}

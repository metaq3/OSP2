#include "cg_advanced_superhud.h"
#include "cg_local.h"
#include "cg_superhud_private.h"
#include "qcommon.h"

float minf(float a, float b)
{
    return a < b ? a : b;
}

float maxf(float a, float b)
{
    return a > b ? a : b;
}

advancedSuperhudFlexContainerStyle_t* CG_ASHUD_CreateFlexContainerStyle()
{
    advancedSuperhudFlexContainerStyle_t *style;

    style = Z_Malloc(sizeof(advancedSuperhudFlexContainerStyle_t));

    OSP_MEMORY_CHECK(style);

    memset(style, 0, sizeof(advancedSuperhudFlexContainerStyle_t));

    style->alignItems = FLEX_START;
    style->direction = FLEX_ROW;
    style->justifyContent = FLEX_HUG_CONTENT;

    return style;
}

void CG_ASHUD_ConstructFlexContainer(advancedSuperhudFlexContainer_t* element)
{
    CG_ASHUD_ConstructBaseElement(&element->base);

    element->base.updateViewbox = (void(*)(void*))CG_ASHUD_UpdateFlexViewbox;
    element->base.render = (void(*)(void*))CG_ASHUD_RenderFlexContainer;
}

float CG_ASHUD_ElementPaddedWidth(void* element)
{
    advancedSuperhudBaseElement_t* baseElement;

    baseElement = (advancedSuperhudBaseElement_t*)element;

    return baseElement->width
           + baseElement->style->padding.left
           + baseElement->style->padding.right;
}

float CG_ASHUD_ElementPaddedHeight(void* element)
{
    advancedSuperhudBaseElement_t* baseElement;

    baseElement = (advancedSuperhudBaseElement_t*)element;

    return baseElement->height
           + baseElement->style->padding.top
           + baseElement->style->padding.bottom;
}

void CG_ASHUD_UpdateFlexViewbox(advancedSuperhudFlexContainer_t* element)
{
    int i;
    advancedSuperhudBaseElement_t* current;
    advancedSuperhudFlexContainerStyle_t* style;
    float width = 0.0f, height = 0.0f, maxElementWidth = 0.0f, maxElementHeight = 0.0f, elementWidth = 0.0f, elementHeight = 0.0f;

    style = (advancedSuperhudFlexContainerStyle_t*)element->base.style;

    for(i = 0; i < element->elementsCount; ++i)
    {
        current = &element->elements[i];

        current->updateViewbox(&element->elements[i]);

        elementWidth = CG_ASHUD_ElementPaddedWidth(current);
        elementHeight = CG_ASHUD_ElementPaddedHeight(current);

        maxElementWidth = maxf(maxElementWidth, elementWidth);
        maxElementHeight = maxf(maxElementHeight, elementHeight);

        width += elementWidth;
        height += elementHeight;
    }

    switch(style->direction)
    {
        case FLEX_ROW:
        case FLEX_ROW_REVERSE:
        {
            width += (float)(element->elementsCount - 1) * style->gap;
            break;
        }

        case FLEX_COLUMN:
        case FLEX_COLUMN_REVERSE:
        {
            height += (float)(element->elementsCount - 1) * style->gap;
            break;
        }
    }

    switch (style->justifyContent)
    {
        case FLEX_HUG_CONTENT:
        {
            switch(style->direction)
            {
                case FLEX_ROW:
                case FLEX_ROW_REVERSE:
                {
                    element->base.width = width;
                    element->base.height = maxElementHeight;
                    break;
                }

                case FLEX_COLUMN:
                case FLEX_COLUMN_REVERSE:
                {
                    element->base.width = maxElementWidth;
                    element->base.height = height;
                    break;
                }
            }
            break;
        }

        case FLEX_SPACE_AROUND:
        case FLEX_SPACE_EVEN:
        case FLEX_SPACE_BETWEEN:
        {
            switch(style->direction)
            {
                case FLEX_ROW:
                case FLEX_ROW_REVERSE:
                {
                    element->base.width = maxf(element->base.width, width);
                    element->base.height = maxf(element->base.height, maxElementHeight);
                    break;
                }

                case FLEX_COLUMN:
                case FLEX_COLUMN_REVERSE:
                {
                    element->base.width = maxf(element->base.width, maxElementWidth);
                    element->base.height = maxf(element->base.height, height);
                    break;
                }
            }
            break;
        }

        case FLEX_STRETCH: break;
    }
    
    CG_ASHUD_UpdateBaseViewbox(&element->base);
}

/**
 * Calculates flex container's element's total width/height ( depending
 * on flex's direction )
 */
float CG_ASHUD_FlexContainerEvaluateDimensionSize(advancedSuperhudFlexContainer_t* element)
{
    advancedSuperhudFlexContainerStyle_t* style;
    int i;
    float elementsDimension = 0.0f;

    style = (advancedSuperhudFlexContainerStyle_t*)element->base.style;

    switch(style->direction)
    {
        case FLEX_ROW_REVERSE:
        case FLEX_ROW:
        {
            for (i = 0; i < element->elementsCount; ++i)
            {
                elementsDimension += CG_ASHUD_ElementPaddedWidth(&element->elements[i]);
            }
            break;
        }

        case FLEX_COLUMN_REVERSE:
        case FLEX_COLUMN:
        {
            for (i = 0; i < element->elementsCount; ++i)
            {
                elementsDimension += CG_ASHUD_ElementPaddedHeight(&element->elements[i]);
            }
            break;
        }
    }

    return elementsDimension;
}

/**
 * Calculates gap between flex's container elements
 */
float CG_ASHUD_FlexContainerCalculateGapSize(advancedSuperhudFlexContainer_t* element)
{
    advancedSuperhudFlexContainerStyle_t* style;
    int i;
    int gapsCount;
    float gap, elementsDimension;

    style = (advancedSuperhudFlexContainerStyle_t*)element->base.style;

    gap = style->gap;
    elementsDimension = CG_ASHUD_FlexContainerEvaluateDimensionSize(element);

    gapsCount = element->elementsCount - 1;

    switch(style->justifyContent)
    {
        case FLEX_STRETCH:
        case FLEX_HUG_CONTENT: return style->gap;
        case FLEX_SPACE_BETWEEN: break;

        // Adds left and right padding with half gap size each
        case FLEX_SPACE_AROUND:
        {
            gapsCount += 1;
            break;
        }

        // Adds left and right padding with gap size each
        case FLEX_SPACE_EVEN:
        {
            gapsCount += 2;
            break;
        }
    }

    switch(style->direction)
    {
        case FLEX_ROW_REVERSE:
        case FLEX_ROW:
        {
            gap = (element->base.width - elementsDimension) / (float)gapsCount;
            break;
        }

        case FLEX_COLUMN_REVERSE:
        case FLEX_COLUMN:
        {
            gap = (element->base.height - elementsDimension) / (float)gapsCount;
            break;
        }
    }

    return gap;
}

void CG_ASHUD_RenderFlexContainer(advancedSuperhudFlexContainer_t* element)
{
    advancedSuperhudFlexContainerStyle_t* style;
    advancedSuperhudBaseElement_t* current;
    float gap, dimension, alignPaddingCoef;
    int i, start, end, step; // `for` cycle

    CG_ASHUD_RenderBaseElement(&element->base);

    style = (advancedSuperhudFlexContainerStyle_t*)element->base.style;

    gap = style->gap; // can be overwritten later
    dimension = 0.0f;
    alignPaddingCoef = 0.0f;

    start = 0;
    end = element->elementsCount;
    step = 1;

    if (style->direction == FLEX_ROW_REVERSE || style->direction == FLEX_COLUMN_REVERSE)
    {
        step = -step;
        start = end - 1;
        end = 0 - 1;
    }

    switch(style->alignItems)
    {
        case FLEX_START: alignPaddingCoef = 0.0f; break;
        case FLEX_CENTER: alignPaddingCoef = 0.5f; break;
        case FLEX_END: alignPaddingCoef = 1.0f; break;
    }

    gap = CG_ASHUD_FlexContainerCalculateGapSize(element);

    switch (style->justifyContent)
    {
        case FLEX_STRETCH:
        case FLEX_HUG_CONTENT:
        case FLEX_SPACE_BETWEEN: break;

        case FLEX_SPACE_AROUND:
        {
            dimension = gap * 0.5f;
            break;
        }

        case FLEX_SPACE_EVEN:
        {
            dimension = gap;
            break;
        }
    }

    // Stretch elements, if needed
    if (style->justifyContent == FLEX_STRETCH)
    {
        switch(style->direction)
        {
            case FLEX_ROW_REVERSE:
            case FLEX_ROW:
            {
                for (i = start; i != end; i += step)
                {
                    current = &element->elements[i];
        
                    current->width = (element->base.width - gap * (element->elementsCount - 1)) / (float)(element->elementsCount);
                }
                break;
            }

            case FLEX_COLUMN_REVERSE:
            case FLEX_COLUMN:
            {
                for (i = start; i != end; i += step)
                {
                    current = &element->elements[i];

                    current->height = (element->base.height - gap * (element->elementsCount - 1)) / (float)(element->elementsCount);
                }
                break;
            }
        }
    }

    // Placing elements in flex's local coordinates
    switch(style->direction)
    {
        case FLEX_ROW_REVERSE:
        case FLEX_ROW:
        {
            for (i = start; i != end; i += step)
            {
                current = &element->elements[i];

                // parent element's relative coordinates
                current->coord.named.x = dimension;
                current->coord.named.y = (element->base.height - CG_ASHUD_ElementPaddedHeight(current)) * alignPaddingCoef;

                dimension += gap + current->width;
            }
            break;
        }

        case FLEX_COLUMN_REVERSE:
        case FLEX_COLUMN:
        {
            for (i = start; i != end; i += step)
            {
                current = &element->elements[i];

                // parent element's relative coordinates
                current->coord.named.x = (element->base.width - CG_ASHUD_ElementPaddedWidth(current)) * alignPaddingCoef;
                current->coord.named.y = dimension;

                dimension += gap + current->height;
            }
            break;
        }
    }

    // Shift element's coordinates to make them global coordinates
    for (i = start; i != end; i += step)
    {
        current = &element->elements[i];

        current->coord.named.x += element->base.coord.named.x + style->base.padding.left;
        current->coord.named.y += element->base.coord.named.y + style->base.padding.top;

        current->render(current);
    }
}
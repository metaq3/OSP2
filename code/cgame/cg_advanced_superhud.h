/*
   **************************************************************
   *                     ADVANCED SUPERHUD                      *
   **************************************************************
 . Beholds useful structures to make drawing complex elements easier.
 . There're some containers for positioning ( like flex box ) and rendering
 . bunch of elements.

 ~ @meta for snems/OSP2 fork
*/

#include "cg_superhud_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  struct {
    vec4_t color;
    float width;
  } border;

  struct {
    float top, right, bottom, left;
  } padding;

  struct {
    // image = 0 makes background fill with color instead of image
    qhandle_t image;
    vec4_t color;
  } background;
} advancedSuperhudElementStyle_t;

// **************************************************************
// *                        BASE ELEMENT                        *
// **************************************************************
// * Holds common element information and functionality for correct
// * and convenient rendering
typedef struct advancedSuperhudBaseElement_s {
  superhudCoord_t coord;

  float width, height, minWidth, minHeight, maxWidth, maxHeight;

  advancedSuperhudElementStyle_t *style;

  /**
   * Updates element's width and height. Called before render() in block
   * elements to determine it's view rectangle.
   *
   * @note non-block elements like flex box can override width and height
   *       in purposes of streching element.
   */
  void (*updateViewbox)(void *element);
  void (*render)(void *element);
} advancedSuperhudBaseElement_t;

/**
 * Allocates new style object and initializes it
 */
advancedSuperhudElementStyle_t *CG_ASHUD_CreateBaseElementStyle();

/**
 * Initializes base element.
 *
 * @param element - pointer to allocated element
 *
 * @note should be called for every inherited element.
 */
void CG_ASHUD_ConstructBaseElement(advancedSuperhudBaseElement_t *element);

/**
 * Makes basic calculations, such as clamping width and height with
 * minWidth/minHeight and maxWidth/maxHeight.
 *
 * @note should be called for every inherited element.
 */
void CG_ASHUD_UpdateBaseViewbox(advancedSuperhudBaseElement_t *element);

/**
 * Renders background and borders.
 *
 * @note should be called for every inherited element
 */
void CG_ASHUD_RenderBaseElement(advancedSuperhudBaseElement_t *element);

// **************************************************************
// *                       FLEX CONTAINER                       *
// **************************************************************
// * Holds bunch of elements, placing them along vertical/horizontal
// * line, allowing to set item stretch/displace to fit line's width/height.

typedef enum {
  FLEX_SPACE_BETWEEN, // fills the gaps between elements so they fill flex
  FLEX_SPACE_AROUND,  // FLEX_SPACE_BETWEEN, but extra padding is added
  FLEX_SPACE_EVEN,    // FLEX_SPACE_BETWEEN, but paddings are the same as gaps
  FLEX_STRETCH,       // stretches elements so they fill whole flex
  FLEX_HUG_CONTENT    // stretches the flex so it "hugs" the content
} flexJustifyContent;

typedef enum {
  FLEX_ROW,
  FLEX_ROW_REVERSE,
  FLEX_COLUMN,
  FLEX_COLUMN_REVERSE
} flexDirection;

typedef enum { FLEX_START, FLEX_END, FLEX_CENTER } flexAlignItems;

typedef struct {
  advancedSuperhudElementStyle_t base;

  flexJustifyContent justifyContent;
  flexAlignItems alignItems;
  flexDirection direction;

  // Gap between elements
  float gap;
} advancedSuperhudFlexContainerStyle_t;

typedef struct {
  advancedSuperhudBaseElement_t base;

  advancedSuperhudBaseElement_t *elements;
  unsigned int elementsCount;
} advancedSuperhudFlexContainer_t;

advancedSuperhudFlexContainerStyle_t *CG_ASHUD_CreateFlexContainerStyle();

void CG_ASHUD_ConstructFlexContainer(advancedSuperhudFlexContainer_t *element);

void CG_ASHUD_UpdateFlexViewbox(advancedSuperhudFlexContainer_t *element);

void CG_ASHUD_RenderFlexContainer(advancedSuperhudFlexContainer_t *element);

#ifdef __cplusplus
}
#endif
#include "cg_local.h"
#include "cg_superhud_private.h"
#include "../qcommon/qcommon.h"

typedef struct
{
    superhudConfig_t config;
    statAccuracy_t accuracy;
    superhudTextContext_t textContext;
} shudElementAcc_t;

void* CG_SHUDElementAccCreate(const superhudConfig_t* config)
{
    shudElementAcc_t* element;

    SHUD_ELEMENT_INIT(element, config);

    CG_SHUDTextMakeContext(&element->config, &element->textContext);
    element->textContext.maxchars = 24;

    return element;
}

void CG_SHUDElementAccRoutine(void* context)
{
    shudElementAcc_t* element = (shudElementAcc_t*)context;
    float           accuracy;
    statAccuracy_t  accStat = cgstat.accuracy;

    if (accStat.attacks == 0)
    {
        accuracy = 0.0f;
    }
    else
    {
        accuracy = (float)(accStat.hits) / (float)(accStat.attacks) * 100.f;
    }

    // Com_Printf("Accuracy: %.2f% (%i/%i)\n", accuracy, accStat.hits, accStat.attacks);
    
    element->textContext.text = va("%.2f (%i/%i)", accuracy, accStat.hits, accStat.attacks);

    CG_SHUDFill(&element->config);

    CG_SHUDTextPrint(&element->config, &element->textContext);
}

void CG_SHUDElementAccDestroy(void* context)
{
    if (!context) {
        return;
    }

    Z_Free(context);
}
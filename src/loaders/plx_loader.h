// ==================================================================
// Filename:    plx_loader.h
// Description: loader for the .PLX model format (modified by
//              Andre LaMothe .PLG format)
//
// Created:     03.04.2025 by DimaSkup              
// ==================================================================
#ifndef PLX_LOADER_H
#define PLX_LOADER_H

#include "../log.h"
#include "../macros.h"
#include "../model.h"
#include "../color.h"

// ==================================================================
// bitmasks for testing
// ==================================================================
#define PLX_RGB_MASK        0x8000     // a mask to get color in RGB format or get color index
#define PLX_SHADE_MODE_MASK 0x6000     // a mask to get shading mode
#define PLX_2SIDED_MASK     0x1000     // a mask to define if polygon is 2-sided
#define PLX_COLOR_MASK      0x0FFF     // xxxxrrrrggggbbbb - 4 bits by each RGB chanel
                                       // xxxxxxxxiiiiiiii - 8 bits color index in index mode

// ==================================================================
// comparison flags, used after mask applying
// ==================================================================

// color mode
#define PLX_COLOR_MODE_RGB_FLAG     0x8000   // polygon color in RGB format
#define PLX_COLOR_MODE_INDEXED_FLAG 0x0000   // 8-bit indexed color of polygon

// flags to test 2-siding
#define PLX_2SIDED_FLAG 0x1000
#define PLX_1SIDED_FLAG 0x0000

// polygon shading
#define PLX_SHADE_MODE_PURE_FLAG     0x0000   // full shading
#define PLX_SHADE_MODE_FLAT_FLAG     0x2000   // flat shading
#define PLX_SHADE_MODE_GOURAUD_FLAG  0x4000   // Gouraud shading
#define PLX_SHADE_MODE_PHONG_FLAG    0x6000   // Phong shading


// ==================================================================
// functions declarations
// ==================================================================
char* GetLinePLG(char* buffer, const int maxLength, FILE* pFile)
{
    // read line one by one from the file in format .PLG and
    // skip comments and empty lines.
    //
    // return: ptr to buffer of valuable symbols or NULL if line is empty

    int index = 0;    // common index
    int length = 0;   // common length                      

    while (1)
    {
        // read line
        if (!fgets(buffer, maxLength, pFile))
            return NULL;

        // remove spaces
        for (length = strlen(buffer), index = 0; isspace(buffer[index]); index++);

        // check if line is empty or it is a comment
        if (index >= length || buffer[index] == '#')
            continue;

        // read line is valuable
        return (&buffer[index]);

    } // while    
}

///////////////////////////////////////////////////////////

int LoadObjectPLG(
    Model* pObj,
    char* filename,      // filename in .PLG format
    const Vec3 scale,  // initial scale
    const Vec3 pos,    // initial position
    const Vec3 rot)    // initial rotation
{
    // load from the disk object in .PLG format, and
    // also setup initial scale, position, rotation for
    // this object to prevent other additional calls
    // of functions for scatic objects


    FILE* pFile = NULL;
    char  buffer[256];
    char  logBuf[64];   // a buffer which is used for log messages
    char* tokenString;  // point to the read string; preparing to analyze

    // fill in the working buffer with zeros
    memset(buffer, 0, 256);
    memset(logBuf, 0, 64);

    sprintf(logBuf, "Load object(model) from .PLG/.PLX file: %s", filename);
    LogPrint(LOG_INFO, logBuf);

    // STAGE 1: reset and initialization of the object
    memset(pObj, 0, sizeof(Model));

    // set object state as active and visible
    pObj->state = OBJECT_STATE_ACTIVE | OBJECT_STATE_VISIBLE;

    // set object position
    pObj->worldPos.x = pos.x;
    pObj->worldPos.y = pos.y;
    pObj->worldPos.z = pos.z;
    pObj->worldPos.w = 1.0f;


    // ----------------------------------------------------

    // STAGE 2: open the .PLG file for reading
    if (!(pFile = fopen(filename, "r")))
    {
        sprintf(logBuf, "Couldn't open PLG file %s.", filename);
        LogError(LOG_INFO, logBuf);
        return 0;
    }

    // STAGE 3: get the first lexem which must be the object descriptor
    if (!(tokenString = GetLinePLG(buffer, 255, pFile)))
    {
        sprintf(logBuf, "PLG file error with file %s (object descriptor is invalid)", filename);
        LogError(LOG_INFO, logBuf);
        return 0;
    }

    sprintf(logBuf, "Object descriptor: %s\n", tokenString);
    LogDebug(logBuf);

    // analyze params of the object
    sscanf(tokenString, "%s %d %d", pObj->name, &pObj->numVertices, &pObj->numFaces);


    // ----------------------------------------------------

    // STAGE 4: load a list of vertices
    for (int vIdx = 0; vIdx < pObj->numVertices; ++vIdx)
    {
        // get vertex
        if (!(tokenString = GetLinePLG(buffer, 255, pFile)))
        {
            sprintf(logBuf, "PLG file error with file %s (vertex list is invalid).", filename);
            LogError(LOG_INFO, logBuf);
            return 0;
        }

        // analyze the vertex
        sscanf(tokenString, "%f %f %f",
                &pObj->verticesLocal[vIdx].x,
                &pObj->verticesLocal[vIdx].y,
                &pObj->verticesLocal[vIdx].z);

        // execute scaling of vertices
        pObj->verticesLocal[vIdx].x *= scale.x;        
        pObj->verticesLocal[vIdx].y *= scale.y;        
        pObj->verticesLocal[vIdx].z *= scale.z;        

        LogDebug("\nVertex %d = %f, %f, %f\n", vIdx,
                pObj->verticesLocal[vIdx].x,
                pObj->verticesLocal[vIdx].y,
                pObj->verticesLocal[vIdx].z);
    } // for

    // compute average and maximal radius of the object
    ComputeModelRadius(pObj);
    LogDebug("\nObject average radius = %f, max radius = %f\n", pObj->avgRadius, pObj->maxRadius);

    // ----------------------------------------------------

    int polySurfaceDesc = 0;  // surface descriptor in format PLG/PLX
    int polyNumVertices = 0;  // the number of vertices in the current polygon (expected to be always 3)                              
    char tmpString[8];        // for storing surface descriptor; also check if we need to convert it from hex

    // STAGE 5: load a list of polygons (faces)
    for (int idx = 0; idx < pObj->numFaces; ++idx)    
    {
        // get descriptor of the polygon
        if (!(tokenString = GetLinePLG(buffer, 255, pFile)))
        {
            sprintf(logBuf, "PLG file error: %s (face descriptor is invalid)\n", filename);
            LogError(LOG_INFO, logBuf);
            return 0;
        }

        LogDebug("\nPolygon %d:", idx);

        // in each line we expect data for only 3 vertices;
        sscanf(tokenString, "%s %d %d %d %d", tmpString,
               &polyNumVertices,              // always must be == 3
               &pObj->faces[idx].idxs[0],     // index to face's vertex_0
               &pObj->faces[idx].idxs[1],
               &pObj->faces[idx].idxs[2]);

        // since polygon descriptor can be in hex format we have to check it
        if (tmpString[0] == '0' && toupper(tmpString[1]) == 'X')
            sscanf(tmpString, "%x", &polySurfaceDesc);
        else
            polySurfaceDesc = atoi(tmpString);

        LogDebug("\nSurface Desc = 0x%.4x, numVertices = %d, indices [%d %d %d]\n",
                polySurfaceDesc,
                polyNumVertices,
                pObj->faces[idx].idxs[0],
                pObj->faces[idx].idxs[1],
                pObj->faces[idx].idxs[2]);

        // ------------------------------------------------

        // Now, when vertices list and indices list are loaded,
        // we analyze surface descriptor and accordingly to it
        // setup state/attributes fields of the face
        
        // get from the descriptor all the fields;
        // first, we define if face is 1-side or 2-side
        if ((polySurfaceDesc & PLX_2SIDED_FLAG))
        {
            SET_BIT(pObj->faces[idx].attr, FACE_ATTR_2SIDED);
            LogDebug(LOG_INFO, "\n2 sided");
        }
        else
        {
            LogDebug(LOG_INFO, "\n1 sided");
        }

        // setup color mode and color value
        if ((polySurfaceDesc & PLX_COLOR_MODE_RGB_FLAG))
        {
            // surface in mode RGB 4.4.4
            SET_BIT(pObj->faces[idx].attr, FACE_ATTR_RGB16);

            // get color and store it in proper 16-bits format into the face;
            // it's 0x0RGB format, where 4-bits per chanel
            const int red   = ((polySurfaceDesc & 0x0F00) >> 8);
            const int green = ((polySurfaceDesc & 0x00F0) >> 4);
            const int blue  = (polySurfaceDesc & 0x000F);

            // get got data in 4.4.4 format but GPU works either in
            // 5.5.5 format or 5.6.5. Our virtual system converts 8.8.8
            // format into 5.5.5 or 5.6.5. But before it we have to 
            // convert 4.4.4 into 8.8.8.
            pObj->faces[idx].color = RGB16Bit(red * 16, green * 16, blue * 16);

            LogDebug("\nRGB color = [%d %d %d]", red, green, blue);

        } // if
        else
        {
            // surface color is set in 8-bits format
            SET_BIT(pObj->faces[idx].attr, FACE_ATTR_8BITCOLOR);

            // extract last 8 bits; so it is a color index
            pObj->faces[idx].color = (polySurfaceDesc & 0x00FF);

            LogDebug("\n8-bit color index = %d", pObj->faces[idx].color);

        } // else
    
        // handle shading mode
        const int shadeMode = (polySurfaceDesc & PLX_SHADE_MODE_MASK);

        // set polygon (face) shading mode
        switch (shadeMode)
        {
            case PLX_SHADE_MODE_PURE_FLAG:
            {
                SET_BIT(pObj->faces[idx].attr, FACE_ATTR_SHADE_MODE_PURE);

                LogDebug("\nShade mode = pure");
                break;
            }
            case PLX_SHADE_MODE_FLAT_FLAG:
            {
                SET_BIT(pObj->faces[idx].attr, FACE_ATTR_SHADE_MODE_FLAT);
                LogDebug("\nShade mode = flat");
                break;
            }
            case PLX_SHADE_MODE_GOURAUD_FLAG:
            {
                SET_BIT(pObj->faces[idx].attr, FACE_ATTR_SHADE_MODE_GOURAUD);
                LogDebug("\nShade mode = gouraud");
                break;
            }
            case PLX_SHADE_MODE_PHONG_FLAG:
            {
                SET_BIT(pObj->faces[idx].attr, FACE_ATTR_SHADE_MODE_PHONG);
                LogDebug("\nShade mode = phong");
                break;
            }

            default: break;
        } // switch
    

        // set active state of polygon
        pObj->faces[idx].state = FACE_STATE_ACTIVE;

    } // for faces


    // close the file
    fclose(pFile);

    LogPrint("Object(model) is successfully loaded from .PLG/PLX file");

    // return success
    return 1;
}

#endif

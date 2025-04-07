// ==================================================================
// Filename:    application.c
// Created:     03.02.25 by DimaSkup
// ==================================================================
#include "application.h"
#include "macros.h"
#include "log.h"
#include <assert.h>
//#include "./loaders/plx_loader.h"


// ==================================================================
// initialize global variables
// ==================================================================
Vec3       g_TransformedVertices[30000];
Vec3       g_TransformedNormals[10000];
RenderList g_RenderList;
Model      g_Models[MAX_NUM_MODELS];
int        g_NumModels = 0;

///////////////////////////////////////////////////////////

bool   g_IsRunning     = false;
int    g_PrevFrameTime = 0;
float  g_DeltaTime     = 0;

Vec3   g_RotationStep  = { 0,0,0 };

// compute the central pixel pos on the screen
int g_WndHalfWidth  = 400;
int g_WndHalfHeight = 300;

int g_NumTransformedVertices = 0;
int g_NumTrianglesToRender = 0;


// ==================================================================
// Declaration of global transformation matrices
// ==================================================================
Matrix g_WorldMatrix;
Matrix g_ViewMatrix;
Matrix g_ProjMatrix;


// ==================================================================
// implementation of functions
// ================================================================== 

void Initialize(void) 
{
    // initialize window, some global variables and game objects

    if (!InitLogger())
        exit(-1);

    g_IsRunning = InitializeWindow();

    if (!g_IsRunning)
    {
        printf("can't initialize the window\n");
        Shutdown();
        exit(-1);
    }

    const int wndWidth  = GetWindowWidth();
    const int wndHeight = GetWindowHeight();

    // initialize render model and culling method
    SetRenderMethod(RENDER_TEXTURED);
    SetCullMethod(CULL_BACK);

    g_WndHalfWidth  = (wndWidth  >> 1);
    g_WndHalfHeight = (wndHeight >> 1);

    // initialize the scene direction light
    InitDirectedLight(Vec3Init(0, -1, 0));

#if 0
    LoadMesh(
        "assets/runway.obj",
        "assets/runway.png",
        Vec3Init(0, -1.5f, 23),
        Vec3Init(0, 0, 0),
        Vec3Init(1, 1, 1));

    LoadMesh(
        "assets/f117.obj",
        "assets/f117.png",
        Vec3Init(0, -1.3f, 5),
        Vec3Init(0, -PI_DIV_2, 0),
        Vec3Init(1, 1, 1));
#endif
#if 0
    LoadMesh(
        "assets/f22.obj", 
        "assets/f22.png",
        Vec3Init(-2, -1.3f, 9),
        Vec3Init(0, -PI_DIV_2, 0),
        Vec3Init(1, 1, 1));
#endif
#if 0 
    LoadMesh(
        "assets/efa.obj",
        "assets/efa.png",
        Vec3Init(2, -1.3f, 9),
        Vec3Init(0, -PI_DIV_2, 0),
        Vec3Init(1, 1, 1));
#endif

#if 0 
    LoadMesh(
        "assets/cube.obj",
        "assets/cube.png",
        Vec3Init(2, -1.3f, 5),
        Vec3Init(0, -PI_DIV_2, 0),
        Vec3Init(1, 1, 1));
#endif
#if 0
    LoadMesh(
        "assets/tree_spruce/tree_spruce.obj",
        "assets/tree_spruce/tree_spruce_diffuse.png",
        Vec3Init(5, -1.3f, 9),
        Vec3Init(0, -PI_DIV_2, 0),
        Vec3Init(1, 1, 1));

#endif

    g_RotationStep.y = 0.005f;

    // initialize the main camera
    const Vec3 cameraPos = { 0,0,0 };
    const Vec3 cameraDir = { 0,0,1 };
    InitCamera(cameraPos, cameraDir);

    // Initialize the perspective projection matrix
    const float aspectX = (float)wndWidth / (float)wndHeight;                                                  
    const float aspectY = (float)wndHeight / (float)wndWidth;
    const float fovY    = PI_DIV_3;
    const float fovX    = 2.0f * atan(tan(fovY/2) * aspectX);
    const float nearZ   = 1.0f;
    const float farZ    = 100.0f;
    g_ProjMatrix = MatrixInitPerspective(fovY, aspectY, nearZ, farZ);

    // initialize frustum planes with a point and a normal vector
    InitFrustumPlanes(fovX, fovY, nearZ, farZ);

    // setup mouse stuff
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);   // to make able mouse cursor move past the window's border

#if 0
    Model model;
    const Vec3 scale = { 1,1,1 };
    const Vec3 pos = { 0,0,0 };
    const Vec3 rot = { 0,0,0 };

    LoadObjectPLG(&model, "assets/cube.plg", &scale, &pos, &rot);
#endif

    LogPrint(LOG_INFO, "kekw!");

    printf("Application is initialized\n");
    exit(-1);
}

///////////////////////////////////////////////////////////

void Run(void)
{
    // run the main loop
    while (g_IsRunning) 
    {
        ProcessInput();
        Update();
        Render();
    }
}

///////////////////////////////////////////////////////////

void Shutdown(void)
{
    // call this func after finishing of the main game loop
    
    printf("Application shutdown:\n");

    CloseLogger();
    DestroyWindow();
    FreeResources();
}


// ==================================================================
// Input handlers
// ==================================================================

void ProcessKeydown(const int keycode)
{
    // handle pressing of different keyboard keys
    switch (keycode)
    {
        case SDLK_ESCAPE:
        {
            // if click "Esc" we close the app
            g_IsRunning = false;
            break;
        }
        case SDLK_1:
        {
            // render both vertices and wireframe lines
            SetRenderMethod(RENDER_WIRE_VERTEX);
            break;
        }
        case SDLK_2:
        {
            // render only wireframe lines
            SetRenderMethod(RENDER_WIRE);
            break;
        }
        case SDLK_3:
        {
            // render only solid color of the faces
            SetRenderMethod(RENDER_FILL_SOLID);
            break;
        }
        case SDLK_4:
        {
            // render both filled triangles and wireframe lines
            SetRenderMethod(RENDER_FILL_SOLID_WIRE);
            break;
        }
        case SDLK_5:
        {
            // render only textured triangles
            SetRenderMethod(RENDER_TEXTURED);
            break;
        } 
        case SDLK_6:
        {
            // render both textured and wireframed triangles
            SetRenderMethod(RENDER_TEXTURED_WIRE);
            break;
        }
        case SDLK_F12:
        {
            SDL_DisplayMode displayMode;
            SDL_GetCurrentDisplayMode(0, &displayMode);

            const int fullscreenWidth  = displayMode.w;
            const int fullscreenHeight = displayMode.h;

            SDL_Surface* sshot = SDL_CreateRGBSurfaceWithFormat(
                0, 
                fullscreenWidth,
                fullscreenHeight,
                24,
                SDL_PIXELFORMAT_RGB24);

            SDL_RenderReadPixels(
                GetRenderer(), 
                NULL, 
                SDL_PIXELFORMAT_RGB24,
                sshot->pixels,
                sshot->pitch);

            SDL_SaveBMP(sshot, "../screenshot.bmp");

            SDL_FreeSurface(sshot);

            break;
        }
        case SDLK_c:
        {
            // enable back-culling
            SetCullMethod(CULL_BACK);
            break;
        }
        case SDLK_x:
        {
            // disable back-culling
            SetCullMethod(CULL_NONE);
            break;
        }
    }
}

///////////////////////////////////////////////////////////

void ProcessInput(void) 
{
    // Poll system events and handle keyboard input

    const uint8_t* keys = SDL_GetKeyboardState(NULL);

    const float cameraMoveSpeed = GetCameraMovementSpeed() * g_DeltaTime;

    // move left(A)/right(D)
    if (keys[SDL_SCANCODE_A])
    {    
        SetCameraForwardVelocity(Vec3Mul(GetCameraRight(), cameraMoveSpeed));
        SetCameraPosition(Vec3Sub(GetCameraPosition(), GetCameraForwardVelocity()));
    }
    else if (keys[SDL_SCANCODE_D])
    {
        SetCameraForwardVelocity(Vec3Mul(GetCameraRight(), cameraMoveSpeed));
        SetCameraPosition(Vec3Add(GetCameraPosition(), GetCameraForwardVelocity()));
    }

    // move forward(W)/backward(S)
    if (keys[SDL_SCANCODE_S])
    {
        SetCameraForwardVelocity(Vec3Mul(GetCameraDirection(), cameraMoveSpeed));
        SetCameraPosition(Vec3Sub(GetCameraPosition(), GetCameraForwardVelocity()));
    }
    else if (keys[SDL_SCANCODE_W])
    {
        SetCameraForwardVelocity(Vec3Mul(GetCameraDirection(), cameraMoveSpeed));
        SetCameraPosition(Vec3Add(GetCameraPosition(), GetCameraForwardVelocity()));
    }

    // move up(space)/down(Z)
    if (keys[SDL_SCANCODE_Z])
    {
        SetCameraForwardVelocity(Vec3Init(0, cameraMoveSpeed, 0));
        SetCameraPosition(Vec3Sub(GetCameraPosition(), GetCameraForwardVelocity()));
    }
    else if (keys[SDL_SCANCODE_SPACE])
    {
        SetCameraForwardVelocity(Vec3Init(0, cameraMoveSpeed, 0));
        SetCameraPosition(Vec3Add(GetCameraPosition(), GetCameraForwardVelocity()));
    }

    // ------------------------------------------

    SDL_Event event;

    while (SDL_PollEvent(&event) != 0)
    {
        switch (event.type) 
        {
            case SDL_QUIT:             // if click "X" button of window
            {
                g_IsRunning = false;
                break;
            }
            case SDL_MOUSEMOTION:
            {
                static int deltaX = 0;
                static int deltaY = 0;

                SDL_GetRelativeMouseState(&deltaX, &deltaY);
                const float rotationSpeed = GetCameraRotationSpeed();

                if (deltaY)
                    RotateCameraPitch(rotationSpeed * deltaY * g_DeltaTime);

                if (deltaX)
                    RotateCameraYaw(rotationSpeed * deltaX * g_DeltaTime);


                break;
            }
            case SDL_KEYDOWN:
            {
                ProcessKeydown(event.key.keysym.sym);
                break;
            }      
        }
    }
}

////////////////////////////////////////////////////////////////////////////
// +-------------+
// | Model space |  <-- original mesh vertices
// +-------------+
// |   +-------------+
// `-> | World space |  <-- multiply by world matrix
//     +-------------+
//     |   +--------------+
//     `-> | Camera space |  <-- multiply by view matrix
//         +--------------+
//         |    +------------+
//         `--> |  Clipping  |  <-- clip against the six frustum planes
//              +------------+
//              |    +------------+
//              `--> | Projection |  <-- multiply by projection matrix
//                   +------------+
//                   |    +-------------+
//                   `--> | Image space |  <-- apply perspective divide
//                        +-------------+
//                        |    +--------------+
//                        `--> | Screen space |  <-- ready to render
//                             +--------------+
////////////////////////////////////////////////////////////////////////////

void TransformVertices(
    const Matrix* pWorld,
    const Matrix* pView,
    Mesh* pMesh)
{
#if 0
    // transform all the vertices of the input mesh
    // first using the world matrix, and then using the view matrix

    Vec3* vertices = g_TransformedVertices;
    
    // convert all the faces of the mesh from Vec3 into Vec4
    for (int i = 0, vIdx = 0; i < pMesh->numFaces; ++i)
    {
        const int idx0 = pMesh->faces[i].idxs[0];
        const int idx1 = pMesh->faces[i].idxs[1];
        const int idx2 = pMesh->faces[i].idxs[2];
        
        vertices[vIdx++] = pMesh->vertices[idx0];
        vertices[vIdx++] = pMesh->vertices[idx1];
        vertices[vIdx++] = pMesh->vertices[idx2];
    }


#if 0
    // MORPHING
    //
    static int upDown = 1;  // -1 means reducing
                            // +1 means rising

    static float scale = 1; // current scale

    // make bigger or smaller?
    if (upDown == 1)
    {
        for (int i = 0, vIdx = 0; i < pMesh->numFaces; ++i, vIdx += 3)
        {
            vertices[vIdx + 0] = Vec3Mul(vertices[vIdx + 0], 1.05f);
            vertices[vIdx + 1] = Vec3Mul(vertices[vIdx + 1], 1.05f);
            vertices[vIdx + 2] = Vec3Mul(vertices[vIdx + 2], 1.05f);
        }
        scale *= (1.0f + g_DeltaTime);
    }
    else
    {
        for (int i = 0, vIdx = 0; i < pMesh->numFaces; ++i, vIdx += 3)
        {
            vertices[vIdx + 0] = Vec3Mul(vertices[vIdx + 0], 0.95f);
            vertices[vIdx + 1] = Vec3Mul(vertices[vIdx + 1], 0.95f);
            vertices[vIdx + 2] = Vec3Mul(vertices[vIdx + 2], 0.95f);
        }
        scale *= (1.0f - g_DeltaTime);
    }

    if (scale < 0.45f || scale > 1.55f)
        upDown = -upDown;
#endif


    // transform all the vertices in the mesh using the world matrix
    for (int i = 0, vIdx = 0; i < pMesh->numFaces; ++i, vIdx += 3)
    {
        vertices[vIdx + 0] = MatrixMulVec3(&g_WorldMatrix, vertices[vIdx + 0]);
        vertices[vIdx + 1] = MatrixMulVec3(&g_WorldMatrix, vertices[vIdx + 1]);
        vertices[vIdx + 2] = MatrixMulVec3(&g_WorldMatrix, vertices[vIdx + 2]);
    }

#if 1
    // transform all the vertices using the view matrix
    for (int i = 0, vIdx = 0; i < pMesh->numFaces; ++i, vIdx += 3)
    {
        vertices[vIdx + 0] = MatrixMulVec3(&g_ViewMatrix, vertices[vIdx + 0]);
        vertices[vIdx + 1] = MatrixMulVec3(&g_ViewMatrix, vertices[vIdx + 1]);
        vertices[vIdx + 2] = MatrixMulVec3(&g_ViewMatrix, vertices[vIdx + 2]);
    }

    // compute normal vectors
    for (int i = 0, vIdx = 0; i < pMesh->numFaces; ++i, vIdx += 3)
    { 
        g_TransformedNormals[i] = GetTriangleNormal(vertices[vIdx + 0], vertices[vIdx + 1], vertices[vIdx + 2]);
    }

    #endif
#endif
}

///////////////////////////////////////////////////////////

void ProcessMesh(Mesh* pMesh)
{
#if 0

    Vec3* vertices = g_TransformedVertices;
    upng_t* pMeshTexture = pMesh->pTexture;

    //const Vec3 dirLightDirection = {0, -1, 0};//GetDirectedLightDirection();
    const bool isBackfaceCullEnabled = IsCullBackface();
    const int numTriangles = (pMesh->numFaces);

    // create a world matrix combining scale, rotation and translation matrices;
    MatrixInitWorld(
        &pMesh->scale, 
        &pMesh->rotation, 
        &pMesh->translation, 
        &g_WorldMatrix);

    // transform with world, view matrices
    TransformVertices(&g_WorldMatrix, &g_ViewMatrix, pMesh);


    for (int i = 0, vIdx = 0; i < numTriangles; ++i)
    {
        const Face* pFace = pMesh->faces + i;
        //const u32 triangleColor = pFace->color;

        //Vec4 vertex0 = Vec4FromVec3(&vertices[vIdx++]);
        //Vec4 vertex1 = Vec4FromVec3(&vertices[vIdx++]);
        //Vec4 vertex2 = Vec4FromVec3(&vertices[vIdx++]);

        const Vec3 vertex0 = vertices[vIdx++];
        const Vec3 vertex1 = vertices[vIdx++];
        const Vec3 vertex2 = vertices[vIdx++];
#if 1
        // ------------------------------------------------

        // calculate the triangle face normal
        const Vec3 faceNormal = g_TransformedNormals[i];  //GetTriangleNormal(vertex0, vertex1, vertex2);
 
        // backface culling, bypassing triangles which we don't see
        if (isBackfaceCullEnabled)
        {
            // find the camera ray vector (pointA => camera_pos_in_view_space)
            Vec3 cameraRay = Vec3Sub(Vec3Init(0,0,0), vertex0);
            
            // take the dot product btw the normal and the camera ray;
            // and if this dot prod is < 0, then we don't display the face
            // (the triangle is looking away from the camera)
            if (Vec3Dot(faceNormal, cameraRay) < 0)
                continue;
        }
#endif
        // ------------------------------------------------
        

        // create a polygon from the original transformed triangle to be clipped
        Polygon polygon = CreatePolygonFromTriangle(
            vertex0,
            vertex1,
            vertex2,
            pFace->texCoords[0],
            pFace->texCoords[1],
            pFace->texCoords[2]);
        
        // clip the polygon and return a new polygon with potential new vertices
        ClipPolygon(&polygon);

        // after clipping we break the polygon into triangles
        Triangle trianglesAfterClipping[MAX_NUM_POLYGON_TRIANGLES];
        int numTrianglesAfterClipping = 0;

        CreateTrianglesFromPolygon(&polygon, trianglesAfterClipping, &numTrianglesAfterClipping);

        // loop all the assembled triangles after clipping
        for (int t = 0; t < numTrianglesAfterClipping; ++t)
        {
            // create a projected 2D triangle which will be rendered
            Triangle triangleToRender = trianglesAfterClipping[t];           

            // loop all three vertices to perform projection
            for (int j = 0; j < 3; ++j)
            {
                // project the current vertex 
                MatrixMulVec4Project(
                    &g_ProjMatrix, 
                    triangleToRender.points[j], 
                    &triangleToRender.points[j]);

                // scale into the view
                triangleToRender.points[j].x *= g_WndHalfWidth;
                triangleToRender.points[j].y *= g_WndHalfHeight;

                // invert the Y values to account for flipped screen Y coordinate
                triangleToRender.points[j].y *= -1;

                // translate the projected points to the middle of the screen
                triangleToRender.points[j].x += g_WndHalfWidth;
                triangleToRender.points[j].y += g_WndHalfHeight;
            }

            uint8_t r = abs((int)(faceNormal.z * 255.0f));
            uint8_t g = abs((int)(faceNormal.y * 255.0f));
            uint8_t b = abs((int)(faceNormal.x * 255.0f));

            uint32_t color = 0xFF000000;
            color |= r;
            color |= (g << 8);
            color |= (b << 16);
            
            triangleToRender.color = color;
            triangleToRender.pTexture = pMeshTexture;
            

            // calculate the light intensity based on face normal and light direction
            triangleToRender.lightIntensity =  1.0f;//-Vec3Dot(faceNormal, GetDirectedLightDirection());
         
            // save the projected triangle in the arr of triangles to render
            g_RenderList.triangles[g_RenderList.numTriangles] = triangleToRender;
            g_RenderList.numTriangles++;

        } // end loop through triangles after clipping
    } // end loop throught triangles of the mesh
#endif
}

///////////////////////////////////////////////////////////

void ResetRenderList(RenderList* pRenderList)
{
    pRenderList->numTriangles = 0;
}

///////////////////////////////////////////////////////////

void Update(void)
{
    // get a delta time factor converted to seconds to be used to update our game objects
    g_DeltaTime = (SDL_GetTicks() - g_PrevFrameTime) * 0.001; 
        
    g_PrevFrameTime = SDL_GetTicks();

    ResetRenderList(&g_RenderList);

    // normalize the directed light vector, if we don't do this
    // we might explode the brightness value of the triangles colors
    //Vec3Normalize(&g_LightDir.direction);
    // reset the number of faces to render for this frame    
    g_NumTrianglesToRender = 0;

    g_NumTransformedVertices = 0;
    // update transformation of the mesh
    //g_Mesh.rotation.x    += (g_RotationStep.x * g_DeltaTime);
    //g_Mesh.rotation.y    += (g_RotationStep.y * g_DeltaTime);
    //g_Mesh.rotation.z    += 0.005f * gDeltaTime;
    //g_Mesh.scale.x       += 0.0002f;
    //g_Mesh.translation.x += 0.01f;
    //g_Mesh.translation.z  = 3.5f;

    // update the world matrix for this frame
    const Vec3 worldUp = { 0, 1, 0 };
    const Vec3 target = GetCameraLookAtTarget();
    MatrixView(GetCameraPosition(), target, worldUp, &g_ViewMatrix);

    // update the all meshes for this frame
    // and load store all the visible triangles of 
    // these meshes for rendering
#if 0
    for (int meshIdx = 0; meshIdx < GetNumMeshes(); ++meshIdx)
    {
        ProcessMesh(GetMeshPtrByIdx(meshIdx));
    }
#endif
}

///////////////////////////////////////////////////////////

void RenderTriangles(const Triangle* triangles, const int numTriangles)
{
    // render an array of triangles according to chosen render method

    const u32 red   = 0xFF0000FF;   // ABGR
    //const u32 white = 0xFFFFFFFF;
    //const u32 black = 0xFF000000;
    //const u32 green = 0xFF00FF00;

    // draw filled triangles (solid color)
    if (ShouldRenderFilledTriangles())
    {
        for (int i = 0; i < numTriangles; ++i)
        {
            const Vec4* p = triangles[i].points;   // an arr of three Vec2 points

            DrawFilledTriangle(
                p[0].x, p[0].y, p[0].w,
                p[1].x, p[1].y, p[1].w,
                p[2].x, p[2].y, p[2].w,
                triangles[i].color);
        }        
    }

    // draw textured triangle
    if (ShouldRenderTexturedTriangles())
    {
#if 0
        for (int i = 0; i < numTriangles; ++i)
        {
            const Triangle* tr = triangles + i;

            const upng_t* pTexture   = tr->pTexture;
            const int textureWidth        = upng_get_width(pTexture);
            const int textureHeight       = upng_get_height(pTexture); 
            const uint32_t* textureBuffer = (uint32_t*) upng_get_buffer(pTexture);


            const Vec4* p   = tr->points;    // points of the projected triangle
            const Vec2* tex = tr->texCoords;     

            DrawTexturedTriangle(
                p[0].x, p[0].y, p[0].z, p[0].w,
                p[1].x, p[1].y, p[1].z, p[1].w,
                p[2].x, p[2].y, p[2].z, p[2].w,
                tex[0], tex[1], tex[2],
                tr->lightIntensity,
                textureWidth,
                textureHeight,
                textureBuffer);
        }
#endif
    }

    // draw unfilled triangle (wireframe)
    if (ShouldRenderWireframe())
    {
        for (int i = 0; i < numTriangles; ++i)
        {
            const Vec4* p = triangles[i].points;   // an arr of three Vec2 points
            
            DrawTriangle(
                p[0].x, p[0].y, 
                p[1].x, p[1].y, 
                p[2].x, p[2].y, 
                red);
        }      
    }

    // draw vertices
    if (ShouldRenderWireVertices())
    {
        for (int i = 0; i < numTriangles; ++i)
        {
            const Vec4* p = triangles[i].points;   // and array of three Vec2 points
            DrawRect(p[0].x, p[0].y, 3, 3, red);   // vertex A
            DrawRect(p[1].x, p[1].y, 3, 3, red);   // vertex B
            DrawRect(p[2].x, p[2].y, 3, 3, red);   // vertex C
     
        }
    }
}

///////////////////////////////////////////////////////////

void Render(void)
{   
    //SDL_RenderClear(g_pRenderer);
    ClearColorBuffer(0xFFAAAA00);  // ABGR black
    ClearZBuffer();

    // draw the background grid (grey dots)
    DrawGrid();

    // loop all projected triangles and render them
    RenderTriangles(g_RenderList.triangles, g_RenderList.numTriangles);

    RenderColorBuffer();
}

///////////////////////////////////////////////////////////

void FreeAssetResources(Mesh* pMesh)
{
    // FIXME

#if 0
    printf("Free mesh: %s\n", pMesh->name);

    if (pMesh->pTexture)
        upng_free(pMesh->pTexture);
   
    if (pMesh->faces) 
        ArrayFree((void**)&(pMesh->faces));

    if (pMesh->normals)
        ArrayFree((void**)&(pMesh->normals));

    if (pMesh->vertices)
        ArrayFree((void**)&(pMesh->vertices));
#endif
}

///////////////////////////////////////////////////////////

void FreeResources(void)
{
#if 0
    // free the memory that was dynamically allocated by the program
    for (int meshIdx = 0; meshIdx < GetNumMeshes(); ++meshIdx)
    {
        FreeAssetResources(GetMeshPtrByIdx(meshIdx));
    }
#endif
}

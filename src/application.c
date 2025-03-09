// ==================================================================
// Filename:    application.c
// Created:     03.02.25 by DimaSkup
// ==================================================================
#include "application.h"
#include <assert.h>

// ==================================================================
// initialize global variables
// ==================================================================

Triangle g_TrianglesToRender[10000];

bool   g_IsRunning     = false;
int    g_PrevFrameTime = 0;
float  g_DeltaTime     = 0;

Vec3   g_RotationStep  = { 0,0,0 };

enum CullMethod   g_CullMethod = CULL_BACK;
enum RenderMethod g_RenderMethod = RENDER_TEXTURED;

// compute the central pixel pos on the screen
int g_WndHalfWidth  = 400;
int g_WndHalfHeight = 300;
int g_NumTrianglesToRender = 0;

typedef enum 
{
    F22,
    F117,
    DRONE,
    CRAB
} AssetType;

AssetType g_AssetType = DRONE;  // asset type which are currently rendered

// ==================================================================
// Declaration of global transformation matrices
// ==================================================================
Matrix g_WorldMatrix;
Matrix g_ViewMatrix;
Matrix g_ProjMatrix;


// ==================================================================
// implementation of functions
// ================================================================== 

void LoadAsset(const char* fileDataPath, const char* texturePath)
{
    assert((fileDataPath != NULL) && (texturePath != NULL) && "invalid input args");

    // load mesh data from the file
    bool result = LoadObjFileData(fileDataPath);
    if (!result)
    {
        printf("\nERROR: can't read in .obj file data: %s\n", fileDataPath);
        Shutdown();
        exit(-1);
    }

    // load mesh texture
    LoadPngTextureData(texturePath);
}

///////////////////////////////////////////////////////////

void Initialize(void) 
{
    // initialize window, some global variables and game objects

    g_IsRunning = InitializeWindow();

    if (!g_IsRunning)
    {
        printf("can't initialize the window\n");
        Shutdown();
        exit(-1);
    }

    // initialize render model and culling method
    g_RenderMethod = RENDER_TEXTURED;
    g_CullMethod = CULL_BACK;

    // allocate the required memory in bytes to hold the color buffer
    g_ColorBuffer = (u32*)malloc(sizeof(u32) * g_WindowWidth * g_WindowHeight);

    // ... and the z-buffer
    g_ZBuffer = (float*)malloc(sizeof(float) * g_WindowWidth * g_WindowHeight);

    ClearZBuffer();

    // creating a SDL texture that is used to display the color buffer
    g_pColorBufferTexture = SDL_CreateTexture(
        g_pRenderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        g_WindowWidth,
        g_WindowHeight);    


    g_WndHalfWidth  = (g_WindowWidth  >> 1);
    g_WndHalfHeight = (g_WindowHeight >> 1);


    LoadAsset("assets/cube.obj", "assets/cube.png");
    g_AssetType = DRONE;
    g_RotationStep.y = 0.005f;

    Vec3Normalize(&g_LightDir.direction);

    // Initialize the perspective projection matrix
    const float aspectX = (float)g_WindowWidth / (float)g_WindowHeight;                                                  
    const float aspectY = (float)g_WindowHeight / (float)g_WindowWidth;
    const float fovY    = M_PIDIV3;
    const float fovX    = 2.0f * atan(tan(fovY/2) * aspectX);
    const float nearZ   = 1.0f;
    const float farZ    = 100.0f;
    g_ProjMatrix = MatrixInitPerspective(fovY, aspectY, nearZ, farZ);

    // initialize frustum planes with a point and a normal vector
    InitFrustumPlanes(fovX, fovY, nearZ, farZ);

    // setup mouse stuff
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);   // to make able mouse cursor move past the window's border

    printf("Application is initialized\n");
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
            g_RenderMethod = RENDER_WIRE_VERTEX;
            break;
        }
        case SDLK_2:
        {
            // render only wireframe lines
            g_RenderMethod = RENDER_WIRE;
            break;
        }
        case SDLK_3:
        {
            // render only solid color of the faces
            g_RenderMethod = RENDER_FILL_SOLID;
            break;
        }
        case SDLK_4:
        {
            // render both filled triangles and wireframe lines
            g_RenderMethod = RENDER_FILL_SOLID_WIRE;
            break;
        }
        case SDLK_5:
        {
            // render only textured triangles
            g_RenderMethod = RENDER_TEXTURED;
            break;
        } 
        case SDLK_6:
        {
            // render both textured and wireframed triangles
            g_RenderMethod = RENDER_TEXTURED_WIRE;
            break;
        }
        case SDLK_7:
        {
            // switch to show the drone asset

            if (g_AssetType != DRONE)
            {            
                FreeAssetResources();
                LoadAsset("assets/drone.obj", "assets/drone.png");
                g_AssetType = DRONE;
                g_Mesh.rotation.x = 0;
                g_Mesh.rotation.y = 0;
                g_RotationStep.x = 0.0f;
                g_RotationStep.y = 0.5f;
            }
            break;
        }
        case SDLK_8:
        {
            // switch to show the crab asset
            if (g_AssetType != CRAB)
            {
                FreeAssetResources();
                LoadAsset("assets/crab.obj", "assets/crab.png");
                g_AssetType = CRAB;
                g_Mesh.rotation.x = 0;
                g_Mesh.rotation.y = 0;
                g_RotationStep.x = 0.0f;
                g_RotationStep.y = 0.5f;
            }
            break;
        }
        case SDLK_9:
        {
            // switch to show the f22 asset
            if (g_AssetType != F22)
            {
                FreeAssetResources();
                LoadAsset("assets/f22.obj", "assets/f22.png");
                g_AssetType = F22;
                g_Mesh.rotation.x = -1.59f;
                g_Mesh.rotation.y = 0;
                g_RotationStep.x = 0.5f;
                g_RotationStep.y = 0.0f;
            }
            break;
        }
        case SDLK_0:
        {
            // switch to show the f117 asset
            if (g_AssetType != F117)
            {
                FreeAssetResources();
                LoadAsset("assets/f117.obj", "assets/f117.png");
                g_AssetType = F117;
                g_Mesh.rotation.x = -1.59f;
                g_Mesh.rotation.y = 0;
                g_RotationStep.x = 0.5f;
                g_RotationStep.y = 0.0f;
            }
            break;
        }

        case SDLK_c:
        {
            // enable back-culling
            g_CullMethod = CULL_BACK;
            break;
        }
        case SDLK_x:
        {
            // disable back-culling
            g_CullMethod = CULL_NONE;
            break;
        }
    }
}

///////////////////////////////////////////////////////////

void ProcessInput(void) 
{
    // Poll system events and handle keyboard input

    const uint8_t* keys = SDL_GetKeyboardState(NULL);

    const float cameraMoveSpeed = 3.0f * g_DeltaTime;

    // move left(A)/right(D)
    if (keys[SDL_SCANCODE_A])
    {
        Vec3 up = {0,1,0};
        g_Camera.right = Vec3Cross(up, g_Camera.direction);
        Vec3Normalize(&g_Camera.right);

        g_Camera.velocity = Vec3Mul(g_Camera.right, cameraMoveSpeed);
        g_Camera.position = Vec3Sub(g_Camera.position, g_Camera.velocity);
    }
    else if (keys[SDL_SCANCODE_D])
    {
        Vec3 up = {0,1,0};
        g_Camera.right = Vec3Cross(up, g_Camera.direction);
        Vec3Normalize(&g_Camera.right);

        g_Camera.velocity = Vec3Mul(g_Camera.right, cameraMoveSpeed);
        g_Camera.position = Vec3Add(g_Camera.position, g_Camera.velocity);
    }

    // move forward(W)/backward(S)
    if (keys[SDL_SCANCODE_S])
    {
        g_Camera.velocity = Vec3Mul(g_Camera.direction, cameraMoveSpeed);
        g_Camera.position = Vec3Sub(g_Camera.position, g_Camera.velocity);
    }
    else if (keys[SDL_SCANCODE_W])
    {
        g_Camera.velocity = Vec3Mul(g_Camera.direction, cameraMoveSpeed);
        g_Camera.position = Vec3Add(g_Camera.position, g_Camera.velocity);
    }

    // move up(space)/down(Z)
    if (keys[SDL_SCANCODE_Z])
        g_Camera.position.y -= cameraMoveSpeed;
    else if (keys[SDL_SCANCODE_SPACE])
        g_Camera.position.y += cameraMoveSpeed;

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
                ComputeCameraAngles(deltaX, deltaY, g_DeltaTime);

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

///////////////////////////////////////////////////////////

void Update(void)
{
#if 0
    // wait some time until the reach the target frame time in ms
    int timeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - g_PrevFrameTime);

    // only delay execution if we are running too fast
    if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME)
    {
        SDL_Delay(timeToWait);   
    }
#endif    

    // get a delta time factor converted to seconds to be used to update our game objects
    g_DeltaTime = (SDL_GetTicks() - g_PrevFrameTime) * 0.001; 
        
    g_PrevFrameTime = SDL_GetTicks();

    // normalize the directed light vector, if we don't do this
    // we might explode the brightness value of the triangles colors
    Vec3Normalize(&g_LightDir.direction);

    // reset the number of faces to render for this frame    
    g_NumTrianglesToRender = 0;

    // update transformation of the mesh
    //g_Mesh.rotation.x    += (g_RotationStep.x * g_DeltaTime);
    //g_Mesh.rotation.y    += (g_RotationStep.y * g_DeltaTime);
    //g_Mesh.rotation.z    += 0.005f * gDeltaTime;
    //g_Mesh.scale.x       += 0.0002f;
    //g_Mesh.translation.x += 0.01f;
    g_Mesh.translation.z  = 3.5f;

    // compute the new camera rotation and translation for the FPS camera movement
    Vec3 worldUp = { 0,1,0 };
    Vec4 target  = { 0,0,1,1 };
    Matrix cameraRotationY;
    Matrix cameraRotation;

    Matrix R = MatrixRotationAxis(g_Camera.right, g_Camera.pitch);
    MatrixRotationY(g_Camera.yaw, &cameraRotationY);
    cameraRotation = MatrixMulMatrix(&R, &cameraRotationY);

    MatrixMulVec4(&cameraRotation, target, &target);
    g_Camera.direction = Vec3FromVec4(&target);

    // offset the camera position in the direction where the camera is pointing at
    Vec3 computedTarget = Vec3Add(g_Camera.position, g_Camera.direction);

    // create a world matrix combining scale, rotation and translation matrices;
    // and create a view matrix as well

    MatrixInitWorld(&g_Mesh.scale, &g_Mesh.rotation, &g_Mesh.translation, &g_WorldMatrix);
    MatrixView(g_Camera.position, computedTarget, worldUp, &g_ViewMatrix);

    // loop all triangle faces of our mesh
    for (int i = 0; i < g_Mesh.numFaces; ++i)
    {
        const int idx0 = g_Mesh.faces[i].a;
        const int idx1 = g_Mesh.faces[i].b;
        const int idx2 = g_Mesh.faces[i].c;

        // loop all three vertices of this face and apply transformations
        Vec4 transVertices[3] = 
        {
            Vec4FromVec3(&g_Mesh.vertices[idx0]),
            Vec4FromVec3(&g_Mesh.vertices[idx1]),
            Vec4FromVec3(&g_Mesh.vertices[idx2]),
        };

        // multiply the world matrix by each original vertex
        MatrixMulVec4(&g_WorldMatrix, transVertices[0], &transVertices[0]);
        MatrixMulVec4(&g_WorldMatrix, transVertices[1], &transVertices[1]);
        MatrixMulVec4(&g_WorldMatrix, transVertices[2], &transVertices[2]);

        // multiply the view matrix by the vector to transform the scene to camera space
        MatrixMulVec4(&g_ViewMatrix, transVertices[0], &transVertices[0]);
        MatrixMulVec4(&g_ViewMatrix, transVertices[1], &transVertices[1]);
        MatrixMulVec4(&g_ViewMatrix, transVertices[2], &transVertices[2]);
        
        // ------------------------------------------------

        //
        // check backface culling
        //
        Vec3 vecA = Vec3FromVec4(&transVertices[0]);  /*    A    */
        Vec3 vecB = Vec3FromVec4(&transVertices[1]);  /*   / \   */
        Vec3 vecC = Vec3FromVec4(&transVertices[2]);  /*  C---B  */

        // get the vector subtraction of B-A and C-A
        Vec3 vecAB = Vec3Sub(vecB, vecA);
        Vec3 vecAC = Vec3Sub(vecC, vecA);

        Vec3Normalize(&vecAB);
        Vec3Normalize(&vecAC);

        // compute the face normal vector
        Vec3 normal = Vec3Cross(vecAB, vecAC); 

        // normalize the face normal vector
        Vec3Normalize(&normal);
        
        // find the camera ray vector (pointA => camera_pos)
        Vec3 origin = { 0, 0, 0 };
        Vec3 cameraRay = Vec3Sub(origin, vecA);

        // take the dot product btw the normal and the camera ray;
        // and if this dot prod is < 0, then we don't display the face
        // (the triangle is looking away from the camera)
        float dotNCamRay = Vec3Dot(normal, cameraRay);

        // backface culling, bypassing triangles which we don't see
        if ((g_CullMethod == CULL_BACK) && (dotNCamRay < 0))
        {
            continue;
        }

        // ------------------------------------------------
        
        const Face* face = g_Mesh.faces + i;

        // create a polygon from the original transformed triangle to be clipped
        Polygon polygon = CreatePolygonFromTriangle(
            transVertices[0],
            transVertices[1],
            transVertices[2],
            face->aUV,
            face->bUV,
            face->cUV);
        
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

            triangleToRender.color = g_Mesh.faces[i].color;

            // calculate the light intensity based on face normal and light direction
            triangleToRender.lightIntensity = -Vec3Dot(normal, g_LightDir.direction);
         
            // save the projected triangle in the arr of triangles to render
            g_TrianglesToRender[g_NumTrianglesToRender] = triangleToRender;
            g_NumTrianglesToRender++;

        } // end loop through triangles in mesh
    } // end loop through meshes
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
    if (g_RenderMethod == RENDER_FILL_SOLID || 
        g_RenderMethod == RENDER_FILL_SOLID_WIRE)
    {
        for (int i = 0; i < numTriangles; ++i)
        {
            const Vec4* p = triangles[i].points;   // an arr of three Vec2 points

            DrawFilledTriangle(
                p[0].x, p[0].y, p[0].w,
                p[1].x, p[1].y, p[1].w,
                p[2].x, p[2].y, p[2].w,
                triangles[i].lightIntensity,
                triangles[i].color);
        }        
    }

    // draw textured triangle
    if (g_RenderMethod == RENDER_TEXTURED || 
        g_RenderMethod == RENDER_TEXTURED_WIRE)
    {
        for (int i = 0; i < numTriangles; ++i)
        {
            const Vec4* p   = triangles[i].points;    // points of the projected triangle
            const Tex2* tex = triangles[i].texCoords;     

            DrawTexturedTriangle(
                p[0].x, p[0].y, p[0].z, p[0].w,
                p[1].x, p[1].y, p[1].z, p[1].w,
                p[2].x, p[2].y, p[2].z, p[2].w,
                tex[0].u, tex[0].v,
                tex[1].u, tex[1].v,
                tex[2].u, tex[2].v,
                triangles[i].lightIntensity,
                g_MeshTexture);
        }
    }

    // draw unfilled triangle (wireframe)
    if (g_RenderMethod == RENDER_WIRE || 
        g_RenderMethod == RENDER_WIRE_VERTEX ||
        g_RenderMethod == RENDER_FILL_SOLID_WIRE ||
        g_RenderMethod == RENDER_TEXTURED_WIRE)
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
    if (g_RenderMethod == RENDER_WIRE_VERTEX)
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
    SDL_RenderClear(g_pRenderer);

    // draw the background grid (grey dots)
    DrawGrid();

    // loop all projected triangles and render them
    RenderTriangles(g_TrianglesToRender, g_NumTrianglesToRender);

    RenderColorBuffer();
    ClearColorBuffer(0xFF000000);  // ABGR black
    ClearZBuffer();

    SDL_RenderPresent(g_pRenderer);
}

///////////////////////////////////////////////////////////

void FreeAssetResources(void)
{
    upng_free(g_PngTexture);
    
    ArrayFree((void**)&g_Mesh.faces);
    ArrayFree((void**)&g_Mesh.normals);
    ArrayFree((void**)&g_Mesh.vertices);
}

///////////////////////////////////////////////////////////

void FreeResources(void)
{
    // free the memory that was dynamicall allocated by the program
    free(g_ColorBuffer);
    free(g_ZBuffer);

    FreeAssetResources();
}

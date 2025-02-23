// ==================================================================
// Filename:    application.c
// Created:     03.02.25 by DimaSkup
// ==================================================================
#include "application.h"
#include "upng.h"

// ==================================================================
// initialize global variables
// ==================================================================

Triangle* g_TrianglesToRender = NULL;

bool   g_IsRunning     = false;
int    g_PrevFrameTime = 0;

Vec3   g_CameraPos     = { 0,0,0 };
Matrix g_ProjMatrix;

enum CullMethod   g_CullMethod = CULL_BACK;
enum RenderMethod g_RenderMethod = RENDER_TEXTURED;

// compute the central pixel pos on the screen
int g_WndHalfWidth  = 400;
int g_WndHalfHeight = 300;

int g_NumFacesToRender = 0;


// ==================================================================
// implementation of functions
// ==================================================================
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


    // load mesh data from the file
#if 1
    bool result = LoadObjFileData("assets/drone.obj");
    if (!result)
    {
        printf("\nERROR: can't read in .obj file data\n");
        Shutdown();
        exit(-1);
    }

    LoadPngTextureData("./assets/drone.png");
#else
    // load hardcoded cube mesh data
    LoadCubeMeshData();
    LoadPngTextureData("./assets/cube.png");
#endif

    g_TrianglesToRender = (Triangle*)malloc(sizeof(Triangle) * g_Mesh.numFaces);

    // Initialize the perspective projection matrix
    float fov         = 1.047197;                 // 60 degrees = PI/3
    float aspectRatio = (float)g_WindowHeight / (float)g_WindowWidth;
    float nearZ       = 0.1f;
    float farZ        = 100.0f;
    g_ProjMatrix = MatrixInitPerspective(fov, aspectRatio, nearZ, farZ);

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
        case SDLK_c:
        {
            // enable back-culling
            g_CullMethod = CULL_BACK;
            break;
        }
        case SDLK_d:
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

    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) 
    {
        case SDL_QUIT:             // if click "X" button of window
        {
            g_IsRunning = false;
            break;
        }
        case SDL_KEYDOWN:
        {
            ProcessKeydown(event.key.keysym.sym);
            break;
        }      
    }
}

///////////////////////////////////////////////////////////

void Update(void)
{
    // wait some time until the reach the target frame time in ms
    int timeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - g_PrevFrameTime);

    // only delay execution if we are running too fast
    if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME)
    {
        SDL_Delay(timeToWait);   
        g_PrevFrameTime = SDL_GetTicks();
    }    

    // normalize the directed light vector, if we don't do this
    // we might explode the brightness value of the triangles colors
    Vec3Normalize(&g_LightDir.direction);

    // reset the number of faces to render for this frame    
    g_NumFacesToRender = 0;

    // update transformation of the mesh
    //g_Mesh.rotation.x    += 0.005f;
    g_Mesh.rotation.y    += 0.005f;
    //g_Mesh.rotation.z    += 0.005f;
    //g_Mesh.scale.x       += 0.0002f;
    //g_Mesh.translation.x += 0.01f;
    g_Mesh.translation.z  = 3.5f;

    // create a world matrix combining scale, rotation and translation matrices
    Matrix worldMatrix;
    MatrixInitWorld(&g_Mesh.scale, &g_Mesh.rotation, &g_Mesh.translation, &worldMatrix);

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
        MatrixMulVec4(&worldMatrix, transVertices[0], &transVertices[0]);
        MatrixMulVec4(&worldMatrix, transVertices[1], &transVertices[1]);
        MatrixMulVec4(&worldMatrix, transVertices[2], &transVertices[2]);

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
        Vec3 cameraRay = Vec3Sub(g_CameraPos, vecA);

        Vec3Normalize(&cameraRay);

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

        // projected triangle and projected points
        Triangle projTriangle;           
        Vec4 projPoints[3];         

        // loop all three vertices to perform projection
        for (int j = 0; j < 3; ++j)
        {
            // project the current vertex 
            MatrixMulVec4Project(
                &g_ProjMatrix,
                transVertices[j],
                &projPoints[j]);

            // scale into the view
            projPoints[j].x *= g_WndHalfWidth;
            projPoints[j].y *= g_WndHalfHeight;

            // invert the Y values to account for flipped screen Y coordinate
            projPoints[j].y *= -1;

            // translate the projected points to the middle of the screen
            projPoints[j].x += g_WndHalfWidth;
            projPoints[j].y += g_WndHalfHeight;
        }


        //
        // create a projected 2D triangle which will be rendered
        //
        
        // store the projected points into the projected triangle
        projTriangle.points[0] = projPoints[0];
        projTriangle.points[1] = projPoints[1];
        projTriangle.points[2] = projPoints[2];

        // calculate the average depth for each face based on
        // the vertices after transformation
        projTriangle.avgDepth = (
            transVertices[0].z + 
            transVertices[1].z +  
            transVertices[2].z) / 3.0f;

        // copy the textures coords into the projected triangle
        const Face* face = g_Mesh.faces + i;
        projTriangle.texCoords[0] = face->aUV;
        projTriangle.texCoords[1] = face->bUV;
        projTriangle.texCoords[2] = face->cUV;

        // calculate the light intensity based on face normal and light direction
        float lightIntensityFactor = -Vec3Dot(normal, g_LightDir.direction);
        
        // calculate the triangle color based on the light angle
        projTriangle.color = LightApplyIntensity(
            g_Mesh.faces[i].color, 
            lightIntensityFactor);


        // save the projected triangle in the arr of triangles to render
        g_TrianglesToRender[g_NumFacesToRender] = projTriangle;
        g_NumFacesToRender++;
        //ArrayPush(g_TrianglesToRender, projTriangle);
    }

    //int numFacesToRender = ArrayLength(g_TrianglesToRender);
#if 0
    // sort the triangles to render by their average depth
    SortTriangles(
        g_TrianglesToRender, 
        g_NumFacesToRender,
        0,                        // low
        g_NumFacesToRender - 1);   // hight
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
    if (g_RenderMethod == RENDER_FILL_SOLID || 
        g_RenderMethod == RENDER_FILL_SOLID_WIRE)
    {
        for (int i = 0; i < numTriangles; ++i)
        {
            const Vec4* p = triangles[i].points;   // an arr of three Vec2 points

            DrawFilledTriangle(
                p[0].x, p[0].y, 
                p[1].x, p[1].y, 
                p[2].x, p[2].y, 
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


    // draw vertices and wireframe
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

    // loop all projected triangles  and render them
    //const int numTriangles = ArrayLength(g_TrianglesToRender);
    RenderTriangles(g_TrianglesToRender, g_NumFacesToRender);

    RenderColorBuffer();
    ClearColorBuffer(0xFF000000);  // ABGR black
    ClearZBuffer();

    SDL_RenderPresent(g_pRenderer);
}

///////////////////////////////////////////////////////////

void FreeResources(void)
{
    // free the memory that was dynamicall allocated by the program
    free(g_ColorBuffer);
    upng_free(g_PngTexture);
    
    ArrayFree((void**)&g_Mesh.faces);
    ArrayFree((void**)&g_Mesh.normals);
    ArrayFree((void**)&g_Mesh.vertices);
    free(g_TrianglesToRender);    
    free(g_ZBuffer);
}

///////////////////////////////////////////////////////////



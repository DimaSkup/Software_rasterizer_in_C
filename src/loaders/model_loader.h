// ==================================================================
// Filename:     model_loader.h
// Description:  functional for loading models from external formats
//               (imporing) and for loading from the internal format
//             
// Created:      04.04.2025 by DimaSkup
// ==================================================================
#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H


void LoadMesh(
    const char* fileDataPath, 
    const char* texturePath,
    const Vec3 translation,
    const Vec3 rotation,
    const Vec3 scale);

int  LoadObjFileData(Mesh* pMesh, const char* filepath);


#endif

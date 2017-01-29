#include <vector>

#include "verts.hpp"

void calcSurface(float xOff, float yOff,
                    float xRes, float yRes,
                    int xSteps, int ySteps,
                    float (*func)(float,float),
                    vector<Vert<float, 4>> *vertices){
    bool color = false;

    for(int ix=0; ix <= xSteps; ix++) {
        for(int iy=0; iy <= ySteps; iy++) {
            float x = xOff + (ix * xRes);
            float y = yOff + (iy * yRes);
            float c = color ? 1.0f : 0.0f;

            vertices->push_back(Vert<float, 4>{x, y, func(x,y), c});
            color = !color;
        }
    }
}

void genIndicies(int xSteps, int ySteps, vector<int> *triangles) {
    triangles->resize((6*xSteps*ySteps),0);
    for (int ti = 0, vi = 0, y = 0; y < ySteps; y++, vi++) {
        for (int x = 0; x < xSteps; x++, ti += 6, vi++) {
            (*triangles)[ti] = vi;
            (*triangles)[ti + 1] = vi + xSteps + 1;
            (*triangles)[ti + 2] = vi + 1;

            (*triangles)[ti + 3] = vi + 1;
            (*triangles)[ti + 4] = vi + xSteps + 1;
            (*triangles)[ti + 5] = vi + xSteps + 2;
        }
    }
}

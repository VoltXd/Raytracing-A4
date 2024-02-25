#include "camera.h"

#include "utils.h"

camera_t initializeCamera(const uint16_t WITDH, const uint16_t HEIGHT)
{
    camera_t camera;

    // Position & target
    camera.lookFrom = (vec3_t){ 13.0f, 2.0f, 3.0f };
    camera.lookAt = (vec3_t){ 0.0f, 0.0f, 0.0f };
    camera.up = (vec3_t){ 0.0f, 1.0f, 0.0f };
    
    // Camera orthonormal base
    camera.w = vec3_sub(&camera.lookFrom, &camera.lookAt);
    vec3_normalize(&camera.w);
    camera.u = vec3_cross(&camera.up, &camera.w);
    vec3_normalize(&camera.u);
    camera.v = vec3_cross(&camera.w, &camera.u);

    // Angles
    camera.fov = degrees_to_radians(20.0f);
    camera.focusDistance = 10.0f;

    // Viewport
    const float aspectRatio = (float)WITDH / HEIGHT;
    const float viewportHeight = 2.0f * tanf(camera.fov * 0.5f) * camera.focusDistance;
    const float viewportWidth = viewportHeight * aspectRatio;
    const vec3_t viewport_u = vec3_scalarMul_return(&camera.u, viewportWidth);
    const vec3_t viewport_v = vec3_scalarMul_return(&camera.v, viewportHeight);

    // Steps
    camera.step_u = vec3_scalarMul_return(&viewport_u, 1.0f / WITDH);
    camera.step_v = vec3_scalarMul_return(&viewport_v, 1.0f / HEIGHT);

    // Top left corner
    vec3_t half_viewport_u = vec3_scalarMul_return(&viewport_u, 0.5f);
    vec3_t half_viewport_v = vec3_scalarMul_return(&viewport_v, 0.5f);
    camera.viewportUpperLeft = vec3_scalarMul_return(&camera.w, -camera.focusDistance);
    camera.viewportUpperLeft = vec3_add(&camera.viewportUpperLeft, &camera.lookFrom);
    camera.viewportUpperLeft = vec3_sub(&camera.viewportUpperLeft, &half_viewport_u);
    camera.viewportUpperLeft = vec3_sub(&camera.viewportUpperLeft, &half_viewport_v);

    // Defocus blur
    camera.defocusAngle = degrees_to_radians(0.6f);
    float defocusRadius = camera.focusDistance * tanf(camera.defocusAngle * 0.5f);
    camera.defocus_disk_u = vec3_scalarMul_return(&camera.u, defocusRadius);
    camera.defocus_disk_v = vec3_scalarMul_return(&camera.v, defocusRadius);
    
    return camera;
}

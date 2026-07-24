#pragma once

class Camera;
class RenderProfiler;

struct RenderContext
{
    Camera& camera;
    RenderProfiler& profiler;
};

#pragma once

#include "Cube.h"
#include "Polygon2D.h"

#include <GL/glew.h>

#include <vector>
#include <memory>

class Scene
{
public:
    Scene() {};

    static Scene& Instance()
    {
        static Scene scene_instance;
        return scene_instance;
    }

    void AddEntity(std::shared_ptr<Entity> entity);
    std::shared_ptr<Entity> GetEntity(size_t index);

    std::vector<std::shared_ptr<Entity>>& GetEntities();
    
    size_t GetBufferAllocationSize() const;
    const GLfloat* GetBufferAsArray() const;

    //Intreaction
    void MouseMove(float xpos, float ypos, int width, int height);
    void SetSelected(int index, double xpos = 0.0, double ypos = 0.0);
    void SetRotationMode(bool switchedOn);

private:
    std::vector<std::shared_ptr<Entity>> entities;

    std::vector<GLfloat> buffer;

    int selected = -1;
    double xpos_selected = 0.0;
    double ypos_selected = 0.0;
    bool rotation_mode = false;
};


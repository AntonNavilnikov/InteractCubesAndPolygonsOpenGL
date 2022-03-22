#include "Scene.h"
#include "TriangulationVisitor.h"

#include <glm/gtc/matrix_transform.hpp>

void Scene::AddEntity(std::shared_ptr<Entity> entity)
{
    size_t startIndex = buffer.size();
    size_t entityAllocationSize = entity->GetTrianglesCount() * 3 * 3;
    buffer.resize(buffer.size() + entityAllocationSize);
    TriangulationVisitor* traingulation = new TriangulationVisitor(buffer, startIndex);
    entity->Accept(traingulation);
    entities.push_back(entity);
}

std::shared_ptr<Entity> Scene::GetEntity(size_t index)
{
    if (index >= entities.size())
    {
        throw std::exception("Index is out of entities");
    }

    return entities[index];
}

std::vector<std::shared_ptr<Entity>>& Scene::GetEntities()
{
    return entities;
}

size_t Scene::GetBufferAllocationSize() const
{
    return buffer.size() * sizeof(GLfloat);
}

const GLfloat* Scene::GetBufferAsArray() const
{
    return buffer.data();// &buffer[0];
}

void Scene::MouseMove(float xpos, float ypos, int width, int height) {

    if (selected == -1)
        return;

    double xdiff = (xpos - xpos_selected);
    double ydiff = (ypos_selected - ypos);

    std::shared_ptr<Entity> entity = GetEntity(selected);
    if (rotation_mode) {
        glm::vec4 rotationCenter =  entity->translation * glm::vec4(0.0, 0.0, 0.0, 1.0);
        float xreal = ((xpos / width) - 0.5) * 2.0;
        float yreal = (0.5 - (ypos / height)) * 2.0;
        if (xreal < rotationCenter.x) {
            ydiff = -ydiff;
        }
        if (yreal > rotationCenter.y) {
            xdiff = -xdiff;
        }
        entity->Rotate(xdiff, ydiff);
    }
    else {
        xdiff *= 2.0f / width;
        ydiff *= 2.0f / height;
        entity->translation = glm::translate(entity->translation, glm::vec3(xdiff, ydiff, 0.0));
    }

    xpos_selected = xpos;
    ypos_selected = ypos;
}

void Scene::SetSelected(int index, double xpos, double ypos)
{
    selected = index - 1;
    xpos_selected = xpos;
    ypos_selected = ypos;
    printf("selected %d\n", selected);
}

void Scene::SetRotationMode(bool switchedOn)
{
    rotation_mode = switchedOn;
}
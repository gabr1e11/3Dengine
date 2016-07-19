/**
 * @class ModelTransform
 * @brief Transforms the vertex data of a Model3D and stores the result
 *
 * @author	Roberto Cano (http://www.robertocano.es)
 */
#include "ModelTransform.hpp"
#include <glm/gtx/quaternion.hpp>
#include "Logging.hpp"

using namespace Logging;

void ModelTransform::Rotate(Model3D &model, const glm::vec3 eulerAngles)
{
    glm::mat4 rotation = glm::toMat4(glm::quat(eulerAngles));

    for (std::vector<Model3D::VertexData>::iterator it = model._modelData.begin(); it != model._modelData.end(); ++it) {
        it->vertex = glm::vec3(rotation * glm::vec4(it->vertex, 1.0f));
        it->normal = glm::mat3(rotation) * it->normal;
    }
}

void ModelTransform::Translate(Model3D &model, const glm::vec3 offsets)
{
    for (std::vector<Model3D::VertexData>::iterator it = model._modelData.begin(); it != model._modelData.end(); ++it) {
        it->vertex += offsets;
    }
}

void ModelTransform::Append(Model3D &to, const Model3D &from)
{
    uint32_t origIndexSize = to._modelIndices.size();

    AppendGeometryOnly(to, from);

    /* The vertex data, materials, textures and indices count can be appended directly,
     * as they are independant of the _modelData size */
    to._materials.insert(to._materials.end(), from.getMaterials().begin(), from.getMaterials().end());
    to._textures.insert(to._textures.end(), from.getTextures().begin(), from.getTextures().end());
    to._indicesCount.insert(to._indicesCount.end(), from.getIndicesCount().begin(), from.getIndicesCount().end());

    /* Copy the vertices ofssets. We need to add to the offsets the original size of the
     * _modelData array in the 'to' model */
    for (std::vector<uint32_t>::const_iterator it = from._indicesOffsets.begin(); it != from._indicesOffsets.end(); ++it) {
        to._indicesOffsets.push_back(*it + origIndexSize);
    }
}

void ModelTransform::AppendGeometryOnly(Model3D &to, const Model3D &from)
{
    uint32_t origDataSize = to._modelData.size();

    /* The vertex data, materials, textures and indices count can be appended directly,
     * as they are independant of the _modelData size */
    to._modelData.insert(to._modelData.end(), from.getVertexData().begin(), from.getVertexData().end());

    /* Copy the vertices indices. We need to add to the indices the original size of the
     * _modelData array in the 'to' model */
    for (std::vector<uint32_t>::const_iterator it = from._modelIndices.begin(); it != from._modelIndices.end(); ++it) {
        to._modelIndices.push_back(*it + origDataSize);
    }
}

void ModelTransform::SetUniqueMaterial(Model3D &model, const Material &material, const Texture &texture)
{
    /* Clear all previous data */
    model._materials.clear();
    model._textures.clear();
    model._indicesOffsets.clear();
    model._indicesCount.clear();

    model._materials.push_back(material);
    model._textures.push_back(texture);

    /* Only one set of indices */
    model._indicesOffsets.push_back(0);
    model._indicesCount.push_back(model._modelIndices.size());
}

void ModelTransform::SetUniqueMaterialFromColor(Model3D &model, const glm::vec3 &color)
{
    uint8_t rgb[3] = {(uint8_t)(color.r * 255), (uint8_t)(color.g * 255), (uint8_t)(color.b * 255)};

    SetUniqueMaterial(model, Material(), Texture(rgb, 1, 1, 8));
}

void ModelTransform::RecalculateNormals(Model3D &model) { /* TODO */}

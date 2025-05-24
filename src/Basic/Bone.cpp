#include "Bone.h"

glm::mat4 Bone::interpolatePosition(float animation_time)
{
    if (1 == num_position_keys)
        return glm::translate(glm::mat4(1.0f), translation_keys[0].translation);

    int index = getPositionIndex(animation_time);
    float scaleFactor = getScaleFactor(animation_time, translation_keys[index].time_stamp, translation_keys[index + 1].time_stamp);
    glm::vec3 finalPosition = translation_keys[index].translation + ((translation_keys[index + 1].translation - translation_keys[index].translation) * scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::interpolateRotation(float animation_time)
{
    if (1 == num_rotation_keys)
        return glm::mat4_cast(glm::normalize(rotation_keys[0].orientation));

    //didnt finish

    int index = getRotationIndex(animation_time);
    float scaleFactor = getScaleFactor(animation_time, rotation_keys[index].time_stamp, rotation_keys[index + 1].time_stamp);
    //liniowe
    //glm::quat finalRotation = rotation_keys[index].orientation + ((rotation_keys[index + 1].orientation - rotation_keys[index].orientation) * scaleFactor);
    //smooth
    glm::quat finalRotation = glm::slerp(rotation_keys[index].orientation, rotation_keys[index + 1].orientation, scaleFactor);
    return glm::mat4_cast(finalRotation);
}

glm::mat4 Bone::interpolateScale(float animation_time)
{
    if (1 == num_scale_keys)
        return glm::scale(glm::mat4(1.0f), scale_keys[0].scale);
    int index = getScaleIndex(animation_time);
    float scaleFactor = getScaleFactor(animation_time, scale_keys[index].time_stamp, scale_keys[index + 1].time_stamp);
    glm::vec3 finalScale = scale_keys[index].scale + ((scale_keys[index + 1].scale - scale_keys[index].scale) * scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}

float Bone::getScaleFactor(float animation_time, float start_time, float end_time)
{
    float range = end_time - start_time;
    float scale = (animation_time - start_time) / range;
    return scale;
}

int Bone::getPositionIndex(float animation_time)
{
    for (int i = 1; i < num_position_keys; i++) {
        if (translation_keys[i].time_stamp > animation_time) {
            return i - 1;
        }
    }
    return 0;
}



int Bone::getRotationIndex(float animation_time)
{
    for (int i = 1; i < num_rotation_keys; i++) {
        if (rotation_keys[i].time_stamp > animation_time) {
            return i - 1;
        }
    }
    return 0;
}

int Bone::getScaleIndex(float animation_time)
{
    for (int i = 1; i < num_scale_keys; i++) {
        if (scale_keys[i].time_stamp > animation_time) {
            return i - 1;
        }
    }
    return 0;
}

void Bone::update(float animation_time)
{
    glm::mat4 translation = interpolatePosition(animation_time);
    glm::mat4 rotation = interpolateRotation(animation_time);
    glm::mat4 scale = interpolateScale(animation_time);
    local_scl = scale;
    local_rot = rotation;
    local_pos = translation;
    local_model_matrix = translation * rotation * scale;
}
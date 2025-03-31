#include "Bone.h"

glm::mat4 Bone::interpolatePosition(float animation_time)
{
    if (1 == num_position_keys)
        return glm::translate(glm::mat4(1.0f), translation_keys[0].translation);

    int index = getScaleIndex(animation_time);
    float scaleFactor = getScaleFactor(animation_time, translation_keys[index].time_stamp, translation_keys[index + 1].time_stamp);
    glm::vec3 finalPosition = translation_keys[index].translation + ((translation_keys[index + 1].translation - translation_keys[index].translation) * scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::interpolateRotation(float animation_time)
{
    if (1 == num_rotation_keys)
        return glm::mat4_cast(glm::normalize(rotation_keys[0].orientation));

    //didnt finish

    int index = getScaleIndex(animation_time);
    float scaleFactor = getScaleFactor(animation_time, translation_keys[index].time_stamp, translation_keys[index + 1].time_stamp);
    glm::vec3 finalPosition = translation_keys[index].translation + ((translation_keys[index + 1].translation - translation_keys[index].translation) * scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
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

float getScaleFactor(float animation_time, float start_time, float end_time)
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

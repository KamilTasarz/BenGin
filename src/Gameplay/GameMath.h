//#pragma once
//
//#include <glm/glm.hpp>
//#include <glm/gtx/quaternion.hpp>
//#include <glm/gtc/constants.hpp>
//#include <algorithm>
//
//class GameMath
//{
//public:
//    // Liniowa interpolacja pomiêdzy a i b
//    static float Lerp(float a, float b, float t)
//    {
//        return a + t * (b - a);
//    }
//
//    static glm::vec3 Lerp(const glm::vec3& a, const glm::vec3& b, float t)
//    {
//        return a + t * (b - a);
//    }
//
//    // Sferyczna interpolacja miêdzy kwaternionami (rotacja)
//    static glm::quat Slerp(const glm::quat& a, const glm::quat& b, float t)
//    {
//        return glm::slerp(a, b, t);
//    }
//
//    // Ogranicz wartoœæ do przedzia³u [min, max]
//    static float Clamp(float value, float minVal, float maxVal)
//    {
//        return std::max(minVal, std::min(value, maxVal));
//    }
//
//    // Dystans miêdzy dwoma punktami
//    static float Distance(const glm::vec3& a, const glm::vec3& b)
//    {
//        return glm::length(b - a);
//    }
//
//    // Zwraca wektor o d³ugoœci 1
//    static glm::vec3 Normalize(const glm::vec3& v)
//    {
//        return glm::normalize(v);
//    }
//
//    // Mapowanie wartoœci z jednego zakresu na inny
//    static float Remap(float value, float fromMin, float fromMax, float toMin, float toMax)
//    {
//        float t = Clamp((value - fromMin) / (fromMax - fromMin), 0.0f, 1.0f);
//        return Lerp(toMin, toMax, t);
//    }
//
//    // Sprawdzenie czy dwie wartoœci float s¹ w przybli¿eniu równe
//    static bool NearlyEqual(float a, float b, float epsilon = 0.0001f)
//    {
//        return std::abs(a - b) < epsilon;
//    }
//
//    // Rzutowanie k¹ta w stopniach na przedzia³ 0-360
//    static float WrapAngleDegrees(float angle)
//    {
//        return std::fmod((angle < 0.0f ? angle + 360.0f : angle), 360.0f);
//    }
//};

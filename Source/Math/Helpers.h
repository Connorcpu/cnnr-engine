#pragma once

#include "Structures.h"
#include "Functions.h"
#include <initializer_list>
#include <cassert>
#include <cstring>

namespace Math
{
    struct Matrix3x2 : Matrix3x2F
    {
        constexpr Matrix3x2();
        constexpr Matrix3x2(const Matrix3x2F &mat);

        constexpr static Matrix3x2 Identity();
        constexpr static Matrix3x2 Translation(Vec2F vec);
        constexpr static Matrix3x2 Translation(Point2F point);
        constexpr static Matrix3x2 Scale(Size2F scale, Point2F center = Point2());
        constexpr static Matrix3x2 Rotation(float angle, Point2F center = Point2());
    };
}

constexpr Math::Matrix3x2::Matrix3x2()
    : Matrix3x2F { 0 } // zero-extend
{
}

constexpr Math::Matrix3x2::Matrix3x2(const Matrix3x2F &mat)
    : Matrix3x2F(mat)
{
}

constexpr Math::Matrix3x2 Math::Matrix3x2::Identity()
{
    return Matrix3x2F
    {
        1, 0,
        0, 1,
        0, 0,
    };
}

constexpr Math::Matrix3x2 Math::Matrix3x2::Translation(const Vec2F vec)
{
    const float x = vec.x, y = vec.y;
    return Matrix3x2F
    {
        1, 0,
        0, 1,
        x, y,
    };
}

constexpr Math::Matrix3x2 Math::Matrix3x2::Translation(const Point2F point)
{
    return Translation(Vec2(point));
}

constexpr Math::Matrix3x2 Math::Matrix3x2::Scale(const Size2F scale, const Point2F center)
{
    float w = scale.width, h = scale.height;
    float x = center.x - w * center.x, y = center.y - h * center.y;

    return Matrix3x2F
    {
        w, 0,
        0, h,
        x, y,
    };
}

constexpr Math::Matrix3x2 Math::Matrix3x2::Rotation(float angle, Point2F center)
{
    return Matrix3x2();
}


// Filename: SphereScissor.h
// 
// Copyright © James Acres
// http://www.jamesacres.com
// http://github.com/jacres
// @jimmyacres
// 
// Created: Fri Jan  8 17:11:10 2016 (-0500)
// Last-Updated: Sat Jan  9 11:57:01 2016 (-0500)

#pragma once

#include "ofMain.h"

namespace ofxRenderToolkit
{
    namespace util
    {
        enum class SphereProjectionResult
        {
            EMPTY,
            PARTIAL,
            FULL
        };

        //--------------------------------------------------------------
        SphereProjectionResult ProjectSphere(const ofVec3f & center, float radius, float focalLength, float aspectRatio, ofVec2f * rectMin, ofVec2f * rectMax)
        {
            auto cx = center.x;
            auto cy = center.y;
            auto cz = center.z;
            auto r2 = radius * radius;

            auto cx2 = cx * cx;
            auto cy2 = cy * cy;
            auto cz2 = cz * cz;
            auto cxz2 = cx2 + cz2;

            if (cxz2 + cy2 > r2)
            {
                auto left = -1.0f;
                auto right = 1.0f;
                auto bottom = -1.0;
                auto top = 1.0;

                auto rcz = 1.0f / cz;

                auto dx = r2 * cx2 - cxz2 * (r2 - cz2);
                if (dx > 0.0f)
                {
                    dx = sqrtf(dx);
                    auto ax = 1.0f / cxz2;
                    auto bx = radius * cx;

                    auto nx1 = (bx + dx) * ax;
                    auto nx2 = (bx - dx) * ax;

                    auto nz1 = (radius - nx1 * cx) * rcz;
                    auto nz2 = (radius - nx2 * cx) * rcz;

                    auto pz1 = cz - radius * nz1;
                    auto pz2 = cz - radius * nz2;

                    if (pz1 < 0.0f)
                    {
                        auto x = nz1 * focalLength / nx1;
                        if (nx1 > 0.0f)
                        {
                            left = fmaxf(left, x);
                        }
                        else
                        {
                            right = fminf(right, x);
                        }
                    }

                    if (pz2 < 0.0f)
                    {
                        auto x = nz2 * focalLength / nx2;
                        if (nx2 > 0.0f) 
                        {
                            left = fmaxf(left, x);
                        }
                        else
                        {
                            right = fminf(right, x);
                        }
                    }
                }

                auto cyz2 = cy2 + cz2;
                auto dy = r2 * cy2 - cyz2 * (r2 - cz2);
                if (dy > 0.0f)
                {
                    dy = sqrtf(dy);
                    auto ay = 1.0f / cyz2;
                    auto by = radius * cy;

                    auto ny1 = (by + dy) * ay;
                    auto ny2 = (by - dy) * ay;

                    auto nz1 = (radius - ny1 * cy) * rcz;
                    auto nz2 = (radius - ny2 * cy) * rcz;

                    auto pz1 = cz - radius * nz1;
                    auto pz2 = cz - radius * nz2;

                    if (pz1 < 0.0f)
                    {
                        auto y = (nz1 * focalLength) / (ny1 * aspectRatio);
                        if (ny1 > 0.0f)
                        {
                            bottom = fmaxf(bottom, y);
                        }
                        else
                        {
                            top = fminf(top, y);
                        }
                    }

                    if (pz2 < 0.0f)
                    {
                        auto y = (nz2 * focalLength) / (ny2 * aspectRatio);
                        if (ny2 > 0.0f)
                        {
                            bottom = fmaxf(bottom, y);
                        }
                        else
                        {
                            top = fminf(top, y);
                        }
                    }
                }

                // Outside of x and y bounds.
                if (!(left < right) && !(bottom < top))
                {
                    return SphereProjectionResult::EMPTY;
                }

                rectMin->x = left;
                rectMax->x = right;
                rectMin->y = bottom;
                rectMax->y = top;

                return SphereProjectionResult::PARTIAL;
            }

            return SphereProjectionResult::FULL;
        }

        //--------------------------------------------------------------
        ofVec2f ScreenCoordFromClipCoord(const ofVec3f & clipCoord, const ofRectangle & viewport)
        {
            float x = viewport.x + (clipCoord.x + 1.0f) * 0.5f * viewport.width;
            float y = viewport.y + (clipCoord.y + 1.0f) * 0.5f * viewport.height;

            return ofVec2f(x, y);
        }

        //--------------------------------------------------------------
        ofVec2f ScreenCoordFromClipCoord(const ofVec2f & clipCoord, const ofRectangle & viewport)
        {
            float x = viewport.x + (clipCoord.x + 1.0f) * 0.5f * viewport.width;
            float y = viewport.y + (clipCoord.y + 1.0f) * 0.5f * viewport.height;

            return ofVec2f(x, y);
        }
    }
}

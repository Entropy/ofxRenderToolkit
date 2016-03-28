#pragma once

#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include <math.h>

namespace ofxRenderToolkit
{
    struct Plane
    {
        Plane() {};

        Plane(const glm::vec3& _normalizedNormal, float _distance)
            : a(_normalizedNormal.x)
            , b(_normalizedNormal.y)
            , c(_normalizedNormal.z)
            , d(_distance)
        {};

        Plane(const glm::vec3& _normalizedNormal, const glm::vec3& _point)
            : a(_normalizedNormal.x)
            , b(_normalizedNormal.y)
            , c(_normalizedNormal.z)
        {
            d = -glm::dot(_normalizedNormal, _point);
        }

        Plane(const glm::vec3& _point, const glm::vec3& _v1, const glm::vec3& _v2)
        {
            Plane(glm::cross(_v1, _v2), _point);
        }

        Plane(const Plane& _plane)
            : a(_plane.a)
            , b(_plane.b)
            , c(_plane.c)
            , d(_plane.d)
        {}

        Plane& operator=(const Plane& _plane)
        {
            a = _plane.a;
            b = _plane.b;
            c = _plane.c;
            d = _plane.d;

            return *this;
        }

        glm::vec3 getNormal() const
        {
            return glm::vec3(a, b, c);
        }

        void normalize()
        {
            float invLen = 1.0f / sqrtf(a*a + b*b + c*c);
            a *= invLen;
            b *= invLen;
            c *= invLen;
            d *= invLen;
        }

        float signedDistance(const glm::vec3& _point) const
        {
            return a * _point.x + b * _point.y + c * _point.z + d;
        }

        float signedDistanceXPlane(const glm::vec3& _point) const
        {
            return a * _point.x + c * _point.z + d;
        }

        float unsignedDistance(const glm::vec3& _point) const
        {
            return fabsf(a * _point.x + b * _point.y + c * _point.z + d);
        }

        float unsignedDistanceX(const glm::vec3& _point) const
        {
            return fabsf(a * _point.x + c * _point.z + d);
        }

        glm::vec3 closestPoint(const glm::vec3& _point) const
        {
            return glm::vec3(_point.x - a, _point.y - b, _point.z - c) * signedDistance(_point);
        }

        glm::vec3 projectPoint(const glm::vec3& _point) const
        {
            return _point - getNormal() * signedDistance(_point);
        }

        glm::vec3 projectPointX(const glm::vec3& _point) const
        {
            return _point - glm::vec3(-1.0f, 0.0f, 0.0f) * signedDistance(_point);
        }

        glm::vec3 projectPointY(const glm::vec3& _point) const
        {
            return _point - glm::vec3(0.0f, -1.0f, 0.0f) * signedDistance(_point);
        }

        bool intersectsSphere(const glm::vec3& _center, float _radius, glm::vec3 * _circleMidPoint, float * _circleRadius) const
        {
            // http://www.ambrsoft.com/TrigoCalc/Sphere/SpherePlaneIntersection_.htm
            // Note: expects plane normal to be normalized (to save on the normalizing of the distToSphereCenter)

            float num = a * _center.x + b * _center.y + c * _center.z + d;
            float distToSphereCenter = fabsf(num);

            if (distToSphereCenter > _radius)
            {
                return false;
            }

            float abc2 = a*a + b*b + c*c;
            float rcpAbc2 = 1.0 / abc2;

            _circleMidPoint->x = _center.x - (a * num) * rcpAbc2;
            _circleMidPoint->y = _center.y - (b * num) * rcpAbc2;
            _circleMidPoint->z = _center.z - (c * num) * rcpAbc2;

            //  ci::app::console() << "RADIUS: " << _radius << ", distToSphereCenter: " << distToSphereCenter << std::endl;

            *_circleRadius = sqrt(_radius * _radius - distToSphereCenter * distToSphereCenter);
            return true;
        }

        // Optimized form for a Y plane
        bool intersectsSphereY(const glm::vec3& _center, float _radius, glm::vec3 * _circleMidPoint, float * _circleRadius) const
        {
            // http://www.ambrsoft.com/TrigoCalc/Sphere/SpherePlaneIntersection_.htm
            // Note: expects plane normal to be normalized (to save on the normalizing of the distToSphereCenter)

            float num = b * _center.y + c * _center.z + d;
            float distToSphereCenter = fabsf(num);

            if (distToSphereCenter > _radius)
            {
                return false;
            }

            float abc2 = b*b + c*c;
            float rcpAbc2 = 1.0 / abc2;

            _circleMidPoint->x = _center.x;
            _circleMidPoint->y = _center.y - (b * num) * rcpAbc2;
            _circleMidPoint->z = _center.z - (c * num) * rcpAbc2;

            *_circleRadius = sqrt(_radius * _radius - distToSphereCenter * distToSphereCenter);
            return true;
        }

        // Optimized form for a Z plane
        bool intersectsSphereZ(const glm::vec3& _center, float _radius, glm::vec3 * _circleMidPoint, float * _circleRadius) const
        {
            // http://www.ambrsoft.com/TrigoCalc/Sphere/SpherePlaneIntersection_.htm
            // Note: expects plane normal to be normalized (to save on the normalizing of the distToSphereCenter)

            float num = c * _center.z + d;
            float distToSphereCenter = fabsf(num);

            if (distToSphereCenter > _radius)
            {
                return false;
            }

            float rcpC2 = 1.0 / (c * c);

            _circleMidPoint->x = _center.x;
            _circleMidPoint->y = _center.y;
            _circleMidPoint->z = _center.z - (c * num) * rcpC2;

            *_circleRadius = sqrt(_radius * _radius - distToSphereCenter * distToSphereCenter);
            return true;
        }

        void flip()
        {
            a = -a;
            b = -b;
            c = -c;
            d = -d;
        }

        void flipNormal()
        {
            a = -a;
            b = -b;
            c = -c;
        }

        float   a;
        float   b;
        float   c;
        float   d;
    };
}

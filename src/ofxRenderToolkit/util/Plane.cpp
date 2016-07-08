#include "Plane.h"

namespace ofxRenderToolkit
{
    namespace util
    {
        //--------------------------------------------------------------
        Plane::Plane()
        {};

        //--------------------------------------------------------------
        Plane::Plane(const glm::vec3 & normalizedNormal, float distance)
            : a(normalizedNormal.x)
            , b(normalizedNormal.y)
            , c(normalizedNormal.z)
            , d(distance)
        {};

        //--------------------------------------------------------------
        Plane::Plane(const glm::vec3 & normalizedNormal, const glm::vec3 & point)
            : a(normalizedNormal.x)
            , b(normalizedNormal.y)
            , c(normalizedNormal.z)
        {
            this->d = -1.0f * glm::dot(normalizedNormal, point);
        }

        //--------------------------------------------------------------
        Plane::Plane(const glm::vec3 & point, const glm::vec3 & v1, const glm::vec3 & v2)
        {
            Plane(glm::cross(v1, v2), point);
        }

        //--------------------------------------------------------------
        Plane::Plane(const Plane & plane)
            : a(plane.a)
            , b(plane.b)
            , c(plane.c)
            , d(plane.d)
        {}

        //--------------------------------------------------------------
        Plane& Plane::operator=(const Plane & plane)
        {
            this->a = plane.a;
            this->b = plane.b;
            this->c = plane.c;
            this->d = plane.d;

            return *this;
        }

        //--------------------------------------------------------------
        glm::vec3 Plane::getNormal() const
        {
            return glm::vec3(this->a, this->b, this->c);
        }

        //--------------------------------------------------------------
        void Plane::normalize()
        {
            float invLen = 1.0f / sqrtf(this->a * this->a + this->b * this->b + this->c * this->c);
            this->a *= invLen;
            this->b *= invLen;
            this->c *= invLen;
            this->d *= invLen;
        }

        //--------------------------------------------------------------
        float Plane::signedDistance(const glm::vec3 & point) const
        {
            return (this->a * point.x + this->b * point.y + this->c * point.z + d);
        }

        //--------------------------------------------------------------
        float Plane::signedDistanceXPlane(const glm::vec3 & point) const
        {
            return (this->a * point.x + this->c * point.z + this->d);
        }

        //--------------------------------------------------------------
        float Plane::unsignedDistance(const glm::vec3 & point) const
        {
            return fabsf(this->a * point.x + this->b * point.y + this->c * point.z + this->d);
        }

        //--------------------------------------------------------------
        float Plane::unsignedDistanceX(const glm::vec3 & point) const
        {
            return fabsf(this->a * point.x + this->c * point.z + this->d);
        }

        //--------------------------------------------------------------
        glm::vec3 Plane::closestPoint(const glm::vec3 & point) const
        {
            return glm::vec3(point.x - this->a, point.y - this->b, point.z - this->c) * this->signedDistance(point);
        }

        //--------------------------------------------------------------
        glm::vec3 Plane::projectPoint(const glm::vec3 & point) const
        {
            return point - this->getNormal() * this->signedDistance(point);
        }

        //--------------------------------------------------------------
        glm::vec3 Plane::projectPointX(const glm::vec3 & point) const
        {
            return point - glm::vec3(-1.0f, 0.0f, 0.0f) * this->signedDistance(point);
        }

        //--------------------------------------------------------------
        glm::vec3 Plane::projectPointY(const glm::vec3 & point) const
        {
            return point - glm::vec3(0.0f, -1.0f, 0.0f) * this->signedDistance(point);
        }

        //--------------------------------------------------------------
        bool Plane::intersectsSphere(const glm::vec3 & center, float radius, glm::vec3 * circleMidPoint, float * circleRadius) const
        {
            // http://www.ambrsoft.com/TrigoCalc/Sphere/SpherePlaneIntersection_.htm
            // Note: expects plane normal to be normalized (to save on the normalizing of the distToSphereCenter)

            auto num = this->a * center.x + this->b * center.y + this->c * center.z + this->d;
            auto distToSphereCenter = fabsf(num);

            if (distToSphereCenter > radius)
            {
                return false;
            }

            auto abc2 = this->a * this->a + this->b * this->b + this->c * this->c;
            auto rcpAbc2 = 1.0f / abc2;

            circleMidPoint->x = center.x - (this->a * num) * rcpAbc2;
            circleMidPoint->y = center.y - (this->b * num) * rcpAbc2;
            circleMidPoint->z = center.z - (this->c * num) * rcpAbc2;

            //  ci::app::console() << "RADIUS: " << radius << ", distToSphereCenter: " << distToSphereCenter << std::endl;

            *circleRadius = sqrt(radius * radius - distToSphereCenter * distToSphereCenter);
            return true;
        }

        //--------------------------------------------------------------
        // Optimized form for a Y plane
        bool Plane::intersectsSphereY(const glm::vec3 & center, float radius, glm::vec3 * circleMidPoint, float * circleRadius) const
        {
            // http://www.ambrsoft.com/TrigoCalc/Sphere/SpherePlaneIntersection_.htm
            // Note: expects plane normal to be normalized (to save on the normalizing of the distToSphereCenter)

            auto num = b * center.y + this->c * center.z + this->d;
            auto distToSphereCenter = fabsf(num);

            if (distToSphereCenter > radius)
            {
                return false;
            }

            auto abc2 = this->b * this->b + this->c * this->c;
            auto rcpAbc2 = 1.0f / abc2;

            circleMidPoint->x = center.x;
            circleMidPoint->y = center.y - (this->b * num) * rcpAbc2;
            circleMidPoint->z = center.z - (this->c * num) * rcpAbc2;

            *circleRadius = sqrt(radius * radius - distToSphereCenter * distToSphereCenter);
            return true;
        }

        //--------------------------------------------------------------
        // Optimized form for a Z plane
        bool Plane::intersectsSphereZ(const glm::vec3 & center, float radius, glm::vec3 * circleMidPoint, float * circleRadius) const
        {
            // http://www.ambrsoft.com/TrigoCalc/Sphere/SpherePlaneIntersection_.htm
            // Note: expects plane normal to be normalized (to save on the normalizing of the distToSphereCenter)

            auto num = c * center.z + d;
            auto distToSphereCenter = fabsf(num);

            if (distToSphereCenter > radius)
            {
                return false;
            }

            auto rcpC2 = 1.0f / (this->c * this->c);

            circleMidPoint->x = center.x;
            circleMidPoint->y = center.y;
            circleMidPoint->z = center.z - (this->c * num) * rcpC2;

            *circleRadius = sqrt(radius * radius - distToSphereCenter * distToSphereCenter);
            return true;
        }

        //--------------------------------------------------------------
        void Plane::flip()
        {
            this->a = -this->a;
            this->b = -this->b;
            this->c = -this->c;
            this->d = -this->d;
        }

        //--------------------------------------------------------------
        void Plane::flipNormal()
        {
            this->a = -this->a;
            this->b = -this->b;
            this->c = -this->c;
        }
    }
}
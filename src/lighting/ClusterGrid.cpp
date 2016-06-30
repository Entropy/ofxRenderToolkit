#include "ClusterGrid.h"

#include "util/Plane.h"
#include "util/FrustumTools.h"
#include "util/MatrixTools.h"
#include "util/SphereScissor.h"
#include "util/RadixSort.h"
#include "util/GLError.h"

#include "glm/gtc/type_ptr.hpp"

namespace ofxRenderToolkit
{
    namespace lighting
    {
        //--------------------------------------------------------------
        ClusterGrid::ClusterGrid()
        {}

        //--------------------------------------------------------------
        void ClusterGrid::setup(const ProjInfo & projInfo)
        {
            this->projInfo = projInfo;

            memset(this->culledPointLightIndices, 0, sizeof(this->culledPointLightIndices[0]) * MAX_POINT_LIGHTS);
            this->numCulledLightIndices = 0;

            memset(this->lightIndices, 0, sizeof(this->lightIndices[0]) * MAX_CLUSTERED_LIGHTS);
            memset(this->lightSortKeys, 0, sizeof(this->lightSortKeys[0]) * MAX_CLUSTERED_LIGHTS);
            memset(this->tempLightSortKeys, 0, sizeof(this->tempLightSortKeys[0]) * MAX_CLUSTERED_LIGHTS);
            memset(this->tempLightIndices, 0, sizeof(this->tempLightIndices[0]) * MAX_CLUSTERED_LIGHTS);

            memset(this->clusterLightPointerList, 0, sizeof(this->clusterLightPointerList[0]) * NUM_CLUSTERS);

            this->createLightIndexTextures();

            this->planesX = new util::Plane[NUM_PLANES_X];
            this->planesY = new util::Plane[NUM_PLANES_Y];
            this->planesZ = new util::Plane[NUM_PLANES_Z];

            this->createPlanes();
        }

        //--------------------------------------------------------------
        ClusterGrid::~ClusterGrid()
        {
            delete[] this->planesX;
            delete[] this->planesY;
            delete[] this->planesZ;
        }

        //--------------------------------------------------------------
        void ClusterGrid::createLightIndexTextures()
        {
            // Light index TBO.
            glGenBuffers(1, &this->lightIndexTbo);
            glBindBuffer(GL_TEXTURE_BUFFER, this->lightIndexTbo);

            glGenTextures(1, &this->lightIndexTex);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_BUFFER, this->lightIndexTex);
            glTexBuffer(GL_TEXTURE_BUFFER, GL_R16UI, this->lightIndexTbo);

            glBindBuffer(GL_TEXTURE_BUFFER, this->lightIndexTbo);
            glBufferData(GL_TEXTURE_BUFFER, MAX_CLUSTERED_LIGHTS * sizeof(this->lightIndices[0]), NULL, GL_DYNAMIC_DRAW);

            glBindBuffer(GL_TEXTURE_BUFFER, 0);
            glBindTexture(GL_TEXTURE_BUFFER, 0);
            CheckGLError();

            // Light pointer 3d texture.
            // RG32_UI (R = Offset into light index TBO, G = (PointLightCount << 16 | SpotLightCount)
            glGenTextures(1, &this->lightPointerTableTex3d);
            glBindTexture(GL_TEXTURE_3D, this->lightPointerTableTex3d);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
            CheckGLError();

            glTexImage3D(GL_TEXTURE_3D, 0, GL_RG32UI, NUM_CLUSTERS_X, NUM_CLUSTERS_Y, NUM_CLUSTERS_Z, 0, GL_RG_INTEGER, GL_UNSIGNED_INT, nullptr);
            CheckGLError();
        }

        //--------------------------------------------------------------
        void ClusterGrid::createPlanes()
        {
            this->nearPlane = util::Plane(ofVec3f(0.0f, 0.0f, -this->projInfo.nearZ), ofVec3f(0.0f, 0.0f, 1.0f));
            this->farPlane = util::Plane(ofVec3f(0.0f, 0.0f, -this->projInfo.farZ), ofVec3f(0.0f, 0.0f, 1.0f));

            // Half height and width in normalized form.
            auto halfNormHeight = std::tanf(this->projInfo.fov * 0.5f);
            auto halfNormWidth = halfNormHeight * this->projInfo.aspectRatio;

            // Half height and width at far plane.
            auto halfFarHeight = halfNormHeight * this->projInfo.farZ;
            auto halfFarWidth = halfNormWidth * this->projInfo.farZ;
            auto frustumDepth = this->projInfo.farZ - this->projInfo.nearZ;

            // z depth of an individual cluster.
            auto clusterDepth = frustumDepth / (float)NUM_CLUSTERS_Z;

            auto centerZ = -(this->projInfo.farZ - this->projInfo.nearZ) * 0.5f;

            // Calculate views space far frustum corner points.
            auto farTL = ofVec3f(-halfFarWidth, halfFarHeight, -this->projInfo.farZ);
            auto farTR = ofVec3f(halfFarWidth, halfFarHeight, -this->projInfo.farZ);
            auto farBL = ofVec3f(-halfFarWidth, -halfFarHeight, -this->projInfo.farZ);
            auto farBR = ofVec3f(halfFarWidth, -halfFarHeight, -this->projInfo.farZ);

            // Calculate X planes.
            auto topPoint = farTL;
            auto bottomPoint = farBL;

            auto stepX = (halfFarWidth * 2.0f) / (float)(NUM_PLANES_X - 1);

            for (int x = 0; x < NUM_PLANES_X; ++x)
            {
                // Set plane normal and position.
                ofVec3f normal = topPoint.getCrossed(bottomPoint).getNormalized();
                this->planesX[x] = util::Plane(normal, 0.0f);

                topPoint.x += stepX;
                bottomPoint.x += stepX;
            }

            // Calculate Y planes.
            auto leftPoint = farBL;
            auto rightPoint = farBR;

            auto stepY = (halfFarHeight * 2.0f) / (float)(NUM_PLANES_Y - 1);

            for (int y = 0; y < NUM_PLANES_Y; ++y)
            {
                // Set plane normal and position.
                ofVec3f normal = leftPoint.getCrossed(rightPoint).getNormalized();
                this->planesY[y] = util::Plane(normal, 0.0f);

                leftPoint.y += stepY;
                rightPoint.y += stepY;
            }

            // Calculate Z planes
            auto normal = ofVec3f(0.0f, 0.0f, -1.0f);
            auto distance = -this->projInfo.nearZ;

            for (int z = 0; z < NUM_PLANES_Z; ++z)
            {
                this->planesZ[z] = util::Plane(normal, distance);
                distance -= clusterDepth;
            }

            // Camera frustum planes
            this->frustumPlanes[0] = this->planesZ[0];
            this->frustumPlanes[1] = this->planesZ[NUM_PLANES_Z - 1];

            this->frustumPlanes[2] = this->planesY[0];
            this->frustumPlanes[3] = this->planesY[NUM_PLANES_Y - 1];
            this->frustumPlanes[2].flipNormal();

            this->frustumPlanes[4] = this->planesX[0];
            this->frustumPlanes[4].flipNormal();

            this->frustumPlanes[5] = this->planesX[NUM_PLANES_X - 1];
        }

        //--------------------------------------------------------------
        void ClusterGrid::addPointLightToCluster(uint16_t idx, int x, int y, int z)
        {
            assert(this->numLightIndices < MAX_CLUSTERED_LIGHTS);

            uint16_t clusterId = z * (NUM_CLUSTERS_X * NUM_CLUSTERS_Y) + y * NUM_CLUSTERS_X + x;
            assert(clusterId <= CLUSTER_INDEX_MASK);

            this->clusterLightPointerList[clusterId].pointLightCount++;

            this->lightIndices[this->numLightIndices] = idx; // store light index
            this->lightSortKeys[this->numLightIndices] = (clusterId); // &LIGHT_TYPE_MASK ); // store light key

            ++this->numLightIndices;
        }

        //--------------------------------------------------------------
        void ClusterGrid::cullPointLights(const ofMatrix4x4 & viewMatrix, const std::vector<PointLight> & pointLights)
        {
            // Reset clusters.
            this->numLightIndices = 0;
            this->numAffectedClusters = 0;
            this->numCulledLightIndices = 0;
            this->numVisibleLights = 0;

            memset(this->lightIndices, 0, sizeof(this->lightIndices[0]) * MAX_CLUSTERED_LIGHTS);
            memset(this->lightSortKeys, 0, sizeof(this->lightSortKeys[0]) * MAX_CLUSTERED_LIGHTS);
            memset(this->tempLightSortKeys, 0, sizeof(this->tempLightSortKeys[0]) * MAX_CLUSTERED_LIGHTS);
            memset(this->tempLightIndices, 0, sizeof(this->tempLightIndices[0]) * MAX_CLUSTERED_LIGHTS);
            memset(this->clusterLightPointerList, 0, sizeof(this->clusterLightPointerList[0]) * NUM_CLUSTERS);

            memset(this->culledPointLightIndices, 0, sizeof(this->culledPointLightIndices[0]) * MAX_POINT_LIGHTS);

            float rcpNearFarLength = 1.0f / (this->projInfo.farZ - this->projInfo.nearZ);

            float horizFov = GetHorizontalFov(this->projInfo.fov, this->projInfo.aspectRatio);
            float focalLength = GetFocalLength(horizFov);
            float aspectRatio = 1.0f / this->projInfo.aspectRatio; // height / width

            for (size_t idx = 0; idx < pointLights.size(); ++idx)
            {
                const auto & light = pointLights[idx];
                auto posVS = light.position * viewMatrix;
                auto lightPosVS = ofVec3f(posVS.x, posVS.y, posVS.z);

                if (!util::SphereInFrustum(this->frustumPlanes, lightPosVS, light.radius))
                {
                    this->culledPointLightIndices[this->numCulledLightIndices++] = idx;
                    continue;
                }

                ++this->numVisibleLights;

                // Z bounds of sphere.
                auto linearMinZ = (-lightPosVS.z - light.radius - this->projInfo.nearZ) * rcpNearFarLength;
                auto linearMaxZ = (-lightPosVS.z + light.radius - this->projInfo.nearZ) * rcpNearFarLength;

                auto z0 = std::max(0, std::min((int)(linearMinZ * (float)NUM_CLUSTERS_Z), NUM_CLUSTERS_Z - 1));
                auto z1 = std::max(0, std::min((int)(linearMaxZ * (float)NUM_CLUSTERS_Z), NUM_CLUSTERS_Z - 1));

                ofVec2f rectMin, rectMax;
                const auto result = util::ProjectSphere(lightPosVS, light.radius, focalLength, aspectRatio, &rectMin, &rectMax);

                // Is sphere behind near plane? If so, let's use a full frustrum box vs reduced sphere since it's difficult to calculate tight culling in this case.
                // Morgan McGuire's method (http://jcgt.org/published/0002/02/05/paper.pdf) does achieve this tight culling on near plane intersection, but at a cost.
                // Perhaps look into it to see if it's worth it? Most likely scene dependent.
                if (result == util::SphereProjectionResult::FULL)
                {
                    int x0 = 0;
                    int x1 = NUM_CLUSTERS_X - 1;

                    int y0 = 0;
                    int y1 = NUM_CLUSTERS_Y - 1;

                    for (int z = z0; z <= z1; ++z)
                    {
                        for (int y = y0; y <= y1; ++y)
                        {
                            for (int x = x0; x <= x1; ++x)
                            {
                                this->addPointLightToCluster(idx, x, y, z);
                            }
                        }
                    }

                    continue;
                }

                // Find minimum and maximum AABB cluster indices from clip coords.
                int x0 = std::min((int)((rectMin.x * 0.5f + 0.5f) * (float)NUM_CLUSTERS_X), NUM_CLUSTERS_X - 1);
                int x1 = std::min((int)((rectMax.x * 0.5f + 0.5f) * (float)NUM_CLUSTERS_X), NUM_CLUSTERS_X - 1);

                int y0 = std::min((int)((rectMin.y * 0.5f + 0.5f) * (float)NUM_CLUSTERS_Y), NUM_CLUSTERS_Y - 1);
                int y1 = std::min((int)((rectMax.y * 0.5f + 0.5f) * (float)NUM_CLUSTERS_Y), NUM_CLUSTERS_Y - 1);

                // Note: There's a bug with the cluster refinement when the light size is small, use full AABB until that is fixed.
                for (int z = z0; z <= z1; ++z)
                {
                    for (int y = y0; y <= y1; ++y)
                    {
                        for (int x = x0; x <= x1; ++x)
                        {
                            this->addPointLightToCluster(idx, x, y, z);
                        }
                    }
                }

                continue;

                const auto centerY = (int)((y0 + y1) * 0.5f);
                const auto centerZ = (int)(((linearMaxZ + linearMinZ) * 0.5f) * ((float)NUM_CLUSTERS_Z));

                for (int z = z0; z <= z1; ++z)
                {
                    ofVec3f zLight = lightPosVS;
                    float zRadius = light.radius;

                    if (z != centerZ)
                    {
                        if (z < centerZ)
                        {
                            const auto & plane = this->planesZ[z + 1];
                            plane.intersectsSphereZ(lightPosVS, light.radius, &zLight, &zRadius);
                        }
                        else
                        {
                            const auto & plane = this->planesZ[z];
                            plane.intersectsSphereZ(lightPosVS, light.radius, &zLight, &zRadius);
                        }
                    }

                    for (int y = y0; y <= y1; ++y)
                    {
                        ofVec3f y_light = zLight;
                        float y_radius = zRadius;

                        if (y != centerY)
                        {
                            if (y < centerY)
                            {
                                const auto & plane = this->planesY[y + 1];
                                if (!plane.intersectsSphereY(zLight, zRadius, &y_light, &y_radius))
                                {
                                    continue;
                                }
                            }
                            else
                            {
                                const auto & plane = this->planesY[y];
                                if (!plane.intersectsSphereY(zLight, zRadius, &y_light, &y_radius))
                                {
                                    continue;
                                }
                            }
                        }

                        int x = x0;
                        do {
                            ++x;
                        } while (x < x1 && -this->planesX[x].signedDistanceXPlane(y_light) >= y_radius);

                        int xs = x1 + 1;
                        do {
                            --xs;
                        } while (xs >= x && this->planesX[xs].signedDistanceXPlane(y_light) >= y_radius);

                        for (--x; x <= xs; x++)
                        {
                            this->addPointLightToCluster(idx, x, y, z);
                        }
                    }
                }
            }
        }

        //--------------------------------------------------------------
        void ClusterGrid::sortLightIndexList()
        {
            // Sort the keys and values. This will give us the light indices sorted by cluster type & light type.
            RadixSort16<uint16_t>(this->lightSortKeys, this->tempLightSortKeys, this->lightIndices, this->tempLightIndices, this->numLightIndices);

            // Update the light index "pointer" list
            uint16_t currKey;
            uint16_t lastKey = 65535; // invalid key value

            for (int i = 0; i < this->numLightIndices; ++i)
            {
                currKey = this->lightSortKeys[i];
                if (currKey != lastKey)
                {
                    this->clusterLightPointerList[currKey].indexOffset = i;
                    ++this->numAffectedClusters;
                }

                lastKey = currKey;
            }
        }

        //--------------------------------------------------------------
        void ClusterGrid::updateLightIndexTextures()
        {
            CheckGLError();
            glBindBuffer(GL_TEXTURE_BUFFER, this->lightIndexTbo);
            CheckGLError();

            auto lightIndexPtr = (uint8_t *)glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);
            assert(NULL != lightIndexPtr);
            memcpy(lightIndexPtr, this->lightIndices, sizeof(this->lightIndices[0]) * this->numLightIndices);

            glUnmapBuffer(GL_TEXTURE_BUFFER);
            glBindBuffer(GL_TEXTURE_BUFFER, 0);
            CheckGLError();

            glBindTexture(GL_TEXTURE_3D, this->lightPointerTableTex3d);
            glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, NUM_CLUSTERS_X, NUM_CLUSTERS_Y, NUM_CLUSTERS_Z, GL_RG_INTEGER, GL_UNSIGNED_INT, (void *)this->clusterLightPointerList);
            CheckGLError();
        }

        //--------------------------------------------------------------
        void ClusterGrid::bindLightIndexTextures(GLuint indexTexUnit, GLuint pointerTableTexUnit)
        {
            glActiveTexture(GL_TEXTURE0 + indexTexUnit);
            glBindTexture(GL_TEXTURE_BUFFER, this->lightIndexTex); // per cluster point light index TBO
            CheckGLError();

            glActiveTexture(GL_TEXTURE0 + pointerTableTexUnit);
            glBindTexture(GL_TEXTURE_3D, this->lightPointerTableTex3d);
            CheckGLError();
        }

        //--------------------------------------------------------------
        void ClusterGrid::unbindLightIndexTextures(GLuint indexTexUnit, GLuint pointerTableTexUnit)
        {
            glActiveTexture(GL_TEXTURE0 + indexTexUnit);
            glBindTexture(GL_TEXTURE_BUFFER, 0);
            CheckGLError();

            glActiveTexture(GL_TEXTURE0 + pointerTableTexUnit);
            glBindTexture(GL_TEXTURE_3D, 0);
            CheckGLError();
        }

        //--------------------------------------------------------------
        int ClusterGrid::getNumPlanesX() const 
        { 
            return NUM_PLANES_X; 
        }
        
        //--------------------------------------------------------------
        int ClusterGrid::getNumPlanesY() const 
        { 
            return NUM_PLANES_Y; 
        }
        
        //--------------------------------------------------------------
        int ClusterGrid::getNumPlanesZ() const 
        { 
            return NUM_PLANES_Z; 
        }

        //--------------------------------------------------------------
        int ClusterGrid::getNumClustersX() const 
        { 
            return NUM_CLUSTERS_X; 
        }

        //--------------------------------------------------------------
        int ClusterGrid::getNumClustersY() const 
        { 
            return NUM_CLUSTERS_Y; 
        }

        //--------------------------------------------------------------
        int ClusterGrid::getNumClustersZ() const 
        { 
            return NUM_CLUSTERS_Z; 
        }

        //--------------------------------------------------------------
        int ClusterGrid::getNumClusters()  const 
        { 
            return NUM_CLUSTERS; 
        }

        //--------------------------------------------------------------
        uint16_t ClusterGrid::getNumVisibleLights() const
        {
            return this->numVisibleLights;
        }

        //--------------------------------------------------------------
        uint32_t ClusterGrid::getNumPointLightIndices() const
        {
            return this->numLightIndices;
        }

        //--------------------------------------------------------------
        const uint16_t * ClusterGrid::getPointLightIndices() const
        {
            return this->lightIndices;
        }

        //--------------------------------------------------------------
        uint32_t ClusterGrid::getNumCulledPointLights() const
        {
            return this->numCulledLightIndices;
        }

        //--------------------------------------------------------------
        const uint16_t * ClusterGrid::getCulledPointLightIndices() const
        {
            return this->culledPointLightIndices;
        }

        //--------------------------------------------------------------
        uint32_t ClusterGrid::getNumAffectedClusters() const
        {
            return this->numAffectedClusters;
        }
    }
}

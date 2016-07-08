#include "ClusterGridDebug.h"

namespace ofxRenderToolkit
{
    namespace lighting
    {
        //--------------------------------------------------------------
        ClusterGridDebug::ClusterGridDebug()
        {}

        //--------------------------------------------------------------
        ClusterGridDebug::~ClusterGridDebug()
        {}

        //--------------------------------------------------------------
        void ClusterGridDebug::createClusterMesh(const ClusterGrid & clusterGrid, const ProjInfo & projInfo)
        {
            this->numPlanesX = clusterGrid.getNumPlanesX();
            this->numPlanesY = clusterGrid.getNumPlanesY();
            this->numPlanesZ = clusterGrid.getNumPlanesZ();

            this->numClustersX = clusterGrid.getNumClustersX();
            this->numClustersY = clusterGrid.getNumClustersY();
            this->numClustersZ = clusterGrid.getNumClustersZ();
            this->numClusters = this->numClustersX * this->numClustersY * this->numClustersZ;

            // Half height and width in normalized form.
            auto halfNormHeight = std::tanf(projInfo.fov * 0.5f);
            auto halfNormWidth = halfNormHeight * projInfo.aspectRatio;

            // Half height and width at far plane.
            auto halfFarHeight = halfNormHeight * projInfo.farZ;
            auto halfFarWidth = halfNormWidth * projInfo.farZ;
            auto halfNearHeight = halfNormHeight * projInfo.nearZ;
            auto halfNearWidth = halfNormWidth * projInfo.nearZ;

            auto frustumDepth = projInfo.farZ - projInfo.nearZ;

            // Calculate views space far frustum corner points.
            auto farTL = glm::vec3(-halfFarWidth, halfFarHeight, -projInfo.farZ);
            auto farTR = glm::vec3(halfFarWidth, halfFarHeight, -projInfo.farZ);
            auto farBL = glm::vec3(-halfFarWidth, -halfFarHeight, -projInfo.farZ);
            auto farBR = glm::vec3(halfFarWidth, -halfFarHeight, -projInfo.farZ);

            auto nearTL = glm::vec3(-halfNearWidth, halfNearHeight, -projInfo.nearZ);
            auto nearTR = glm::vec3(halfNearWidth, halfNearHeight, -projInfo.nearZ);
            auto nearBL = glm::vec3(-halfNearWidth, -halfNearHeight, -projInfo.nearZ);
            auto nearBR = glm::vec3(halfNearWidth, -halfNearHeight, -projInfo.nearZ);

            // Calculate X planes.
            auto farStepX = (halfFarWidth * 2.0f) / (this->numPlanesX - 1);
            auto farStepY = (halfFarHeight * 2.0f) / (this->numPlanesY - 1);

            auto nearStepX = (halfNearWidth * 2.0f) / (this->numPlanesX - 1);
            auto nearStepY = (halfNearHeight * 2.0f) / (this->numPlanesY - 1);

            auto stepZ = -frustumDepth / (this->numPlanesZ - 1);

            auto & clusterColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.7f);

            std::vector<glm::vec3> vertices;
            std::vector<glm::vec4> colors;

            for (int z = 0; z < this->numPlanesZ; ++z)
            {
                auto nearPoint = nearBL;
                auto farPoint = farBL;

                auto zPercent = (float)z / (this->numPlanesZ - 1);

                for (int y = 0; y < this->numPlanesY; ++y)
                {
                    auto yPercent = (float)y / this->numPlanesY;

                    for (int x = 0; x < this->numPlanesX; ++x)
                    {
                        auto xPercent = (float)x / this->numPlanesX;

                        auto dir = farPoint - nearPoint;
						auto len = glm::length(dir);

                        auto pt = glm::normalize(dir) * zPercent * len + nearPoint;
                        vertices.push_back(pt);

                        nearPoint.x += nearStepX;
                        farPoint.x += farStepX;
                    }

                    nearPoint.x = nearBL.x;
                    farPoint.x = farBL.x;

                    nearPoint.y += nearStepY;
                    farPoint.y += farStepY;
                }
            }

            std::vector<ofIndexType> clusterIndices;

            for (int z = 0; z < this->numClustersZ; ++z)
            {
                for (int y = 0; y < this->numClustersY; ++y)
                {
                    uint32_t z_offset = this->numPlanesX * this->numPlanesY * z;

                    for (int x = 0; x < this->numClustersX; ++x)
                    {
                        uint32_t y_offset = y * this->numPlanesX;

                        uint32_t tl_front = z_offset + y_offset + x;
                        uint32_t tr_front = tl_front + 1;
                        uint32_t bl_front = z_offset + y_offset + this->numPlanesX + x;
                        uint32_t br_front = bl_front + 1;

                        uint32_t tl_back = tl_front + this->numPlanesX * this->numPlanesY;
                        uint32_t tr_back = tr_front + this->numPlanesX * this->numPlanesY;
                        uint32_t bl_back = bl_front + this->numPlanesX * this->numPlanesY;
                        uint32_t br_back = br_front + this->numPlanesX * this->numPlanesY;

                        // front face
                        clusterIndices.push_back(tl_front); clusterIndices.push_back(tr_front);
                        clusterIndices.push_back(tr_front); clusterIndices.push_back(br_front);
                        clusterIndices.push_back(br_front); clusterIndices.push_back(bl_front);
                        clusterIndices.push_back(bl_front); clusterIndices.push_back(tl_front);

                        // back face
                        clusterIndices.push_back(tl_back); clusterIndices.push_back(tr_back);
                        clusterIndices.push_back(tr_back); clusterIndices.push_back(br_back);
                        clusterIndices.push_back(br_back); clusterIndices.push_back(bl_back);
                        clusterIndices.push_back(bl_back); clusterIndices.push_back(tl_back);

                        // sides 
                        clusterIndices.push_back(tl_front); clusterIndices.push_back(tl_back);
                        clusterIndices.push_back(tr_front); clusterIndices.push_back(tr_back);
                        clusterIndices.push_back(br_front); clusterIndices.push_back(br_back);
                        clusterIndices.push_back(bl_front); clusterIndices.push_back(bl_back);
                    }
                }
            }

            this->clusterVbo.setVertexData(vertices.data(), vertices.size(), GL_STATIC_DRAW);
            this->clusterVbo.setIndexData(clusterIndices.data(), clusterIndices.size(), GL_STATIC_DRAW);
            this->clusterVbo.enableIndices();

            // Frustum lines
            std::vector<glm::vec3> lineVertices;
            std::vector<glm::vec3> lineColors;

            auto & frustumColor = glm::vec3(1.0f, 1.0f, 1.0f);

            // sides
            lineVertices.push_back(nearBL);
            lineVertices.push_back(farBL);
            lineColors.push_back(frustumColor);
            lineColors.push_back(frustumColor);

            lineVertices.push_back(nearBR);
            lineVertices.push_back(farBR);
            lineColors.push_back(frustumColor);
            lineColors.push_back(frustumColor);

            lineVertices.push_back(nearTL);
            lineVertices.push_back(farTL);
            lineColors.push_back(frustumColor);
            lineColors.push_back(frustumColor);

            lineVertices.push_back(nearTR);
            lineVertices.push_back(farTR);
            lineColors.push_back(frustumColor);
            lineColors.push_back(frustumColor);

            // near plane
            lineVertices.push_back(nearTL);
            lineVertices.push_back(nearTR);
            lineColors.push_back(frustumColor);
            lineColors.push_back(frustumColor);

            lineVertices.push_back(nearBL);
            lineVertices.push_back(nearBR);
            lineColors.push_back(frustumColor);
            lineColors.push_back(frustumColor);

            lineVertices.push_back(nearTL);
            lineVertices.push_back(nearBL);
            lineColors.push_back(frustumColor);
            lineColors.push_back(frustumColor);

            lineVertices.push_back(nearTR);
            lineVertices.push_back(nearBR);
            lineColors.push_back(frustumColor);
            lineColors.push_back(frustumColor);

            // far plane
            lineVertices.push_back(farTL);
            lineVertices.push_back(farTR);
            lineColors.push_back(frustumColor);
            lineColors.push_back(frustumColor);

            lineVertices.push_back(farBL);
            lineVertices.push_back(farBR);
            lineColors.push_back(frustumColor);
            lineColors.push_back(frustumColor);

            lineVertices.push_back(farTL);
            lineVertices.push_back(farBL);
            lineColors.push_back(frustumColor);
            lineColors.push_back(frustumColor);

            lineVertices.push_back(farTR);
            lineVertices.push_back(farBR);
            lineColors.push_back(frustumColor);
            lineColors.push_back(frustumColor);

            this->frustumVbo.setVertexData(lineVertices.data(), lineVertices.size(), GL_STATIC_DRAW);
        }

        //--------------------------------------------------------------
        void ClusterGridDebug::drawFrustum(const ofCamera & camera)
        {
            ofSetColor(ofFloatColor(1.0f, 1.0f, 1.0f, 1.0f));

            ofPushMatrix();
            ofMultMatrix(glm::inverse(camera.getModelViewMatrix()));
            {
                this->frustumVbo.draw(GL_LINES, 0, 24);
            }
            ofPopMatrix();
        }

        //--------------------------------------------------------------
        void ClusterGridDebug::drawCluster(uint16_t idx)
        {
            if (idx > this->numClusters)
            {
                ofLogWarning("ClusterGridDebug::drawCluster") << "Index is out of range: " << idx;
                return;
            }

            const int numIndicesPerCluster = 24;
            int startOffset = idx * numIndicesPerCluster;
            this->clusterVbo.drawElements(GL_LINES, numIndicesPerCluster, startOffset);
        }

        //--------------------------------------------------------------
        void ClusterGridDebug::drawCluster(const ofCamera & camera, uint16_t idx)
        {
            if (idx > this->numClusters)
            {
                ofLogWarning("ClusterGridDebug::drawCluster") << "Index is out of range: " << idx;
                return;
            }

            ofPushMatrix();
            ofMultMatrix(glm::inverse(camera.getModelViewMatrix()));
            {
                const int numIndicesPerCluster = 24;
                int startOffset = idx * numIndicesPerCluster;
                this->clusterVbo.drawElements(GL_LINES, numIndicesPerCluster, startOffset);
            }
            ofPopMatrix();
        }

        //--------------------------------------------------------------
        void ClusterGridDebug::drawCluster(const ofCamera & camera, uint16_t x, uint16_t y, uint16_t z)
        {
            if (x > this->numClustersX || x < 0) ofLogWarning("ClusterGridDebug::drawCluster") << "x is out of range: " << x;
            if (y > this->numClustersY || y < 0) ofLogWarning("ClusterGridDebug::drawCluster") << "y is out of range: " << y;
            if (z > this->numClustersZ || z < 0) ofLogWarning("ClusterGridDebug::drawCluster") << "z is out of range: " << z;

            uint16_t idx = (z * this->numClustersX * this->numClustersY + y * this->numClustersX + x);
            this->drawCluster(camera, idx);
        }

        //--------------------------------------------------------------
        void ClusterGridDebug::drawOccupiedClusters(const ofCamera & camera, const ClusterGrid & clusterGrid)
        {
            static uint16_t idx = 0;

            ofSetColor(ofFloatColor(1.0f, 1.0f, 1.0f, 0.3f));

            ofPushMatrix();
            ofMultMatrix(glm::inverse(camera.getModelViewMatrix()));
            for (uint16_t idx = 0; idx < this->numClusters; ++idx)
            {
                if (clusterGrid.clusterLightPointerList[idx].pointLightCount > 0)
                {
                    this->drawCluster(idx);
                }
            }
            ofPopMatrix();
        }
    }
}


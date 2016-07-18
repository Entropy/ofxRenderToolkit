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
        void ClusterGridDebug::setup(const ClusterGrid & clusterGrid)
        {
			this->numPlanesX = clusterGrid.getNumPlanesX();
            this->numPlanesY = clusterGrid.getNumPlanesY();
            this->numPlanesZ = clusterGrid.getNumPlanesZ();

            this->numClustersX = clusterGrid.getNumClustersX();
            this->numClustersY = clusterGrid.getNumClustersY();
            this->numClustersZ = clusterGrid.getNumClustersZ();
            this->numClusters = this->numClustersX * this->numClustersY * this->numClustersZ;

			const auto & projInfo = clusterGrid.getProjInfo();

			// Half height and width in normalized form.
			const auto halfNormHeight = std::tanf(projInfo.fov * 0.5f);
			const auto halfNormWidth = halfNormHeight * projInfo.aspectRatio;

            // Half height and width at far plane.
			const auto halfFarHeight = halfNormHeight * projInfo.farZ;
			const auto halfFarWidth = halfNormWidth * projInfo.farZ;
			const auto halfNearHeight = halfNormHeight * projInfo.nearZ;
			const auto halfNearWidth = halfNormWidth * projInfo.nearZ;

			const auto frustumDepth = projInfo.farZ - projInfo.nearZ;

            // Calculate views space far frustum corner points.
			const auto farTL = glm::vec3(-halfFarWidth, halfFarHeight, -projInfo.farZ);
			const auto farTR = glm::vec3(halfFarWidth, halfFarHeight, -projInfo.farZ);
			const auto farBL = glm::vec3(-halfFarWidth, -halfFarHeight, -projInfo.farZ);
			const auto farBR = glm::vec3(halfFarWidth, -halfFarHeight, -projInfo.farZ);

			const auto nearTL = glm::vec3(-halfNearWidth, halfNearHeight, -projInfo.nearZ);
			const auto nearTR = glm::vec3(halfNearWidth, halfNearHeight, -projInfo.nearZ);
			const auto nearBL = glm::vec3(-halfNearWidth, -halfNearHeight, -projInfo.nearZ);
			const auto nearBR = glm::vec3(halfNearWidth, -halfNearHeight, -projInfo.nearZ);

            // Build cluster VBO mesh.
			{
				const auto farStepX = (halfFarWidth * 2.0f) / (this->numPlanesX - 1);
				const auto farStepY = (halfFarHeight * 2.0f) / (this->numPlanesY - 1);

				const auto nearStepX = (halfNearWidth * 2.0f) / (this->numPlanesX - 1);
				const auto nearStepY = (halfNearHeight * 2.0f) / (this->numPlanesY - 1);

				std::vector<glm::vec3> vertices;

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

				std::vector<ofIndexType> indices;

				for (int z = 0; z < this->numClustersZ; ++z)
				{
					for (int y = 0; y < this->numClustersY; ++y)
					{
						uint32_t zOffset = this->numPlanesX * this->numPlanesY * z;

						for (int x = 0; x < this->numClustersX; ++x)
						{
							uint32_t yOffset = y * this->numPlanesX;

							uint32_t frontTL = zOffset + yOffset + x;
							uint32_t frontTR = frontTL + 1;
							uint32_t frontBL = zOffset + yOffset + this->numPlanesX + x;
							uint32_t frontBR = frontBL + 1;

							uint32_t backTL = frontTL + this->numPlanesX * this->numPlanesY;
							uint32_t backTR = frontTR + this->numPlanesX * this->numPlanesY;
							uint32_t backBL = frontBL + this->numPlanesX * this->numPlanesY;
							uint32_t backBR = frontBR + this->numPlanesX * this->numPlanesY;

							// Front face
							indices.push_back(frontTL); indices.push_back(frontTR);
							indices.push_back(frontTR); indices.push_back(frontBR);
							indices.push_back(frontBR); indices.push_back(frontBL);
							indices.push_back(frontBL); indices.push_back(frontTL);

							// Back face
							indices.push_back(backTL); indices.push_back(backTR);
							indices.push_back(backTR); indices.push_back(backBR);
							indices.push_back(backBR); indices.push_back(backBL);
							indices.push_back(backBL); indices.push_back(backTL);

							// Side faces 
							indices.push_back(frontTL); indices.push_back(backTL);
							indices.push_back(frontTR); indices.push_back(backTR);
							indices.push_back(frontBR); indices.push_back(backBR);
							indices.push_back(frontBL); indices.push_back(backBL);
						}
					}
				}

				this->clusterVbo.setVertexData(vertices.data(), vertices.size(), GL_STATIC_DRAW);
				this->clusterVbo.setIndexData(indices.data(), indices.size(), GL_STATIC_DRAW);
				this->clusterVbo.enableIndices();
			}

            // Build frustum VBO mesh.
			{
				std::vector<glm::vec3> vertices;
				std::vector<glm::vec3> colors;

				const auto & frustumColor = glm::vec3(1.0f, 1.0f, 1.0f);

				// Sides
				vertices.push_back(nearBL);
				vertices.push_back(farBL);
				colors.push_back(frustumColor);
				colors.push_back(frustumColor);

				vertices.push_back(nearBR);
				vertices.push_back(farBR);
				colors.push_back(frustumColor);
				colors.push_back(frustumColor);

				vertices.push_back(nearTL);
				vertices.push_back(farTL);
				colors.push_back(frustumColor);
				colors.push_back(frustumColor);

				vertices.push_back(nearTR);
				vertices.push_back(farTR);
				colors.push_back(frustumColor);
				colors.push_back(frustumColor);

				// Near plane
				vertices.push_back(nearTL);
				vertices.push_back(nearTR);
				colors.push_back(frustumColor);
				colors.push_back(frustumColor);

				vertices.push_back(nearBL);
				vertices.push_back(nearBR);
				colors.push_back(frustumColor);
				colors.push_back(frustumColor);

				vertices.push_back(nearTL);
				vertices.push_back(nearBL);
				colors.push_back(frustumColor);
				colors.push_back(frustumColor);

				vertices.push_back(nearTR);
				vertices.push_back(nearBR);
				colors.push_back(frustumColor);
				colors.push_back(frustumColor);

				// Far plane
				vertices.push_back(farTL);
				vertices.push_back(farTR);
				colors.push_back(frustumColor);
				colors.push_back(frustumColor);

				vertices.push_back(farBL);
				vertices.push_back(farBR);
				colors.push_back(frustumColor);
				colors.push_back(frustumColor);

				vertices.push_back(farTL);
				vertices.push_back(farBL);
				colors.push_back(frustumColor);
				colors.push_back(frustumColor);

				vertices.push_back(farTR);
				vertices.push_back(farBR);
				colors.push_back(frustumColor);
				colors.push_back(frustumColor);

				this->frustumVbo.setVertexData(vertices.data(), vertices.size(), GL_STATIC_DRAW);
			}
        }

        //--------------------------------------------------------------
        void ClusterGridDebug::drawFrustum(const ofCamera & camera)
        {
            ofSetColor(ofColor::white);

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

            static const int kNumIndicesPerCluster = 24;
            int startOffset = idx * kNumIndicesPerCluster;
            this->clusterVbo.drawElements(GL_LINES, kNumIndicesPerCluster, startOffset);
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
				static const int kNumIndicesPerCluster = 24;
				int startOffset = idx * kNumIndicesPerCluster;
                this->clusterVbo.drawElements(GL_LINES, kNumIndicesPerCluster, startOffset);
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


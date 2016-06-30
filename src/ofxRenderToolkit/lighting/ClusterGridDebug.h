#pragma once
#include "ClusterGrid.h"

namespace ofxRenderToolkit
{
    namespace lighting
    {
        class ClusterGridDebug
        {
        public:
            ClusterGridDebug();
            ~ClusterGridDebug();

            void createClusterMesh(const ClusterGrid & clusterGrid, const ProjInfo & projInfo);
            void drawFrustum(const ofCamera & camera);
            void drawCluster(uint16_t idx);
            void drawCluster(const ofCamera & camera, uint16_t idx);
            void drawCluster(const ofCamera & camera, uint16_t x, uint16_t y, uint16_t z);
            void drawOccupiedClusters(const ofCamera & camera, const ClusterGrid & clusterGrid);

        private:

        private:
            uint16_t numPlanesX;
            uint16_t numPlanesY;
            uint16_t numPlanesZ;

            uint16_t numClustersX;
            uint16_t numClustersY;
            uint16_t numClustersZ;
            uint16_t numClusters;

            ofVbo clusterVbo;
            ofVbo frustumVbo;
        };
    }
}
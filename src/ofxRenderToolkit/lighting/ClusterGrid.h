#pragma once

#include "ofMain.h"

#include "PointLight.h"
#include "ProjInfo.h"

#include "ofxRenderToolkit/util/Plane.h"

namespace ofxRenderToolkit
{
    namespace lighting
    {
        // R32G32_UINT
        struct ClusterLightPointer
        {
            uint32_t indexOffset;
            uint32_t pointLightCount;
        }; 
        
        class ClusterGrid
        {
        public:
            static const int NUM_PLANES_X = 21;
            static const int NUM_PLANES_Y = 12;
            static const int NUM_PLANES_Z = 17;

            static const int NUM_CLUSTERS_X = NUM_PLANES_X - 1;
            static const int NUM_CLUSTERS_Y = NUM_PLANES_Y - 1;
            static const int NUM_CLUSTERS_Z = NUM_PLANES_Z - 1;

            static const uint16_t NUM_CLUSTERS = NUM_CLUSTERS_X * NUM_CLUSTERS_Y * NUM_CLUSTERS_Z;
            static const uint16_t MAX_CLUSTERED_LIGHTS = 0xffff;  // 65535 max clustered lights 

            static const uint16_t CLUSTER_INDEX_MASK = 0x0fff; // (0111111111111111) 15 bit mask (0-4096 clusters) 
            static const uint16_t LIGHT_TYPE_MASK = 0xf000; // (1000000000000000) 1 bit mask (point light or spot light) 
            static const uint8_t POINT_LIGHT_TYPE_VALUE = 0;
            static const uint8_t SPOT_LIGHT_TYPE_VALUE = 1;

            static const uint32_t MAX_POINT_LIGHTS = 1024;

        public:
            ClusterGrid();
            ~ClusterGrid();

            void setup(const ProjInfo & projInfo);

            void cullPointLights(const ofMatrix4x4 & viewMatrix, const std::vector<PointLight> & pointLights);

            void sortLightIndexList();

            void updateLightIndexTextures();
            void bindLightIndexTextures(GLuint indexTexUnit, GLuint pointerTableTexUnit);
            void unbindLightIndexTextures(GLuint indexTexUnit, GLuint pointerTableTexUnit);

            int getNumPlanesX() const;
            int getNumPlanesY() const;
            int getNumPlanesZ() const;
                
            int getNumClustersX() const;
            int getNumClustersY() const;
            int getNumClustersZ() const;
            int getNumClusters()  const;

            uint16_t getNumVisibleLights() const;
                     
            uint32_t getNumPointLightIndices() const;
            const uint16_t * getPointLightIndices() const;
                             
            uint32_t getNumCulledPointLights() const;
            const uint16_t * getCulledPointLightIndices() const;
                                
            uint32_t getNumAffectedClusters() const;

        private:
            void createLightIndexTextures();
            void createPlanes();

            void addPointLightToCluster(uint16_t idx, int x, int y, int z);

            ProjInfo projInfo;

            util::Plane * planesX;
            util::Plane * planesY;
            util::Plane * planesZ;

            util::Plane nearPlane;
            util::Plane farPlane;

            util::Plane frustumPlanes[6];

        public:
            GLuint lightIndexTbo;
            GLuint lightIndexTex;
            GLuint lightPointerTableTex3d;

            uint16_t lightIndices[MAX_CLUSTERED_LIGHTS]; // unsorted list of light indices (just light IDs)
            uint16_t lightSortKeys[MAX_CLUSTERED_LIGHTS]; // list of light keys (cluster ID and light type encoded in 16 bits)

            uint16_t tempLightIndices[MAX_CLUSTERED_LIGHTS]; // temp list for radix sort
            uint16_t tempLightSortKeys[MAX_CLUSTERED_LIGHTS]; // temp list for radix sort

            ClusterLightPointer clusterLightPointerList[NUM_CLUSTERS]; // data for 3d texture (cluster's light index offset and light counts)

            uint32_t numLightIndices;
            uint32_t numAffectedClusters;

            uint16_t culledPointLightIndices[MAX_POINT_LIGHTS]; // unsorted list of light indices (just light IDs)
            uint32_t numCulledLightIndices;

            int numVisibleLights;
        };
    }
}

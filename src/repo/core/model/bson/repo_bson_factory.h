/**
*  Copyright (C) 2015 3D Repo Ltd
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Affero General Public License as
*  published by the Free Software Foundation, either version 3 of the
*  License, or (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Affero General Public License for more details.
*
*  You should have received a copy of the GNU Affero General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
*  BSON factory that creates specific BSON objects.
*  This essentially functions as the builder (like Mongo BsonObjBuilder)
*/

#pragma once
#include "../repo_node_utils.h"

#include "repo_bson_project_settings.h"
#include "repo_node.h"
#include "repo_node_camera.h"
#include "repo_node_metadata.h"
#include "repo_node_material.h"
#include "repo_node_mesh.h"
#include "repo_node_revision.h"
#include "repo_node_texture.h"
#include "repo_node_transformation.h"

namespace repo {
	namespace core {
		namespace model {
			namespace bson {
				class RepoBSONFactory
				{
					public:

						/**
						* Create a project setting BSON
						* @param uniqueProjectName a unique name for the project
						* @param owner owner ofthis project
						* @param group group with access of this project
						* @param type  type of project
						* @param description Short description of the project
						* @param ownerPermissionsOctal owner's permission in POSIX
						* @param groupPermissionsOctal group's permission in POSIX
						* @param publicPermissionsOctal other's permission in POSIX
						* @return returns a pointer Project settings
						*/
						static RepoProjectSettings* makeRepoProjectSettings(
							const std::string &uniqueProjectName,
							const std::string &owner,
							const std::string &group = std::string(),
							const std::string &type = REPO_PROJECT_TYPE_ARCHITECTURAL,
							const std::string &description = std::string(),
							const uint8_t     &ownerPermissionsOctal = 7,
							const uint8_t     &groupPermissionsOctal = 0,
							const uint8_t     &publicPermissionsOctal = 0);


						/*
						* -------------------- REPO NODES ------------------------
						*/
						/**
						* Create a RepoNode
						* @param type of node (Optional)
						* @return returns a RepoNode
						*/
						static RepoNode*    makeRepoNode   (std::string type=std::string());

						/**
						* Create a Camera Node
						* @param aspect ratio
						* @param Far clipping plane
						* @param Near clipping plane. Should not be 0 to avoid divis
						* @param Field of view.
						* @param LookAt vector relative to parent transformations.
						* @param Position relative to parent transformations
						* @param Up vector relative to parent transformations.
						* @param API level of the node (optional, default REPO_NODE_API_LEVEL_1)
						* @param name of the node (optional, default empty string)
						* @return returns a pointer Camera node
						*/
						static CameraNode* makeCameraNode(
							const float         &aspectRatio,
							const float         &farClippingPlane,
							const float         &nearClippingPlane,
							const float         &fieldOfView,
							const repo_vector_t &lookAt,
							const repo_vector_t &position,
							const repo_vector_t &up,
							const std::string   &name = std::string(),
							const int           &apiLevel = REPO_NODE_API_LEVEL_1);


						/**
						* Create a Material Node
						* @param a struct that contains the info about the material
						* @param name of the Material
						* @return returns a pointer Material node
						*/
						static MaterialNode* makeMaterialNode(
							const repo_material_t &material,
							const std::string     &name,
							const int             &apiLevel = REPO_NODE_API_LEVEL_1);

						/**
						* Create a Metadata Node
						* @param metadata Metadata itself in RepoBSON format
						* @param mimtype Mime type, the media type of the metadata (optional)
						* @param name Name of Metadata (optional)
						* @param parents 
						* @param apiLevel Repo Node API level (optional)
						* @return returns a pointer metadata node
						*/
						static MetadataNode* makeMetaDataNode(
							RepoBSON			         &metadata,
							const std::string            &mimeType = std::string(),
							const std::string            &name = std::string(),
							const std::vector<repoUUID> &parents = std::vector<repoUUID>(),
							const int                    &apiLevel = REPO_NODE_API_LEVEL_1);
						/**
						* Create a Mesh Node
						* @param vertices vector of vertices
						* @param faces vector of faces
						* @param normals vector of normals
						* @param boundingBox vector of 2 vertex indicating the bounding box
						* @param uvChannels vector of UV Channels
						* @param colors vector of colours
						* @param outline outline generated from bounding box
						* @param name name of the node (optional, default empty string)
						* @param apiLevel API level of the node (optional, default REPO_NODE_API_LEVEL_1)
						* @return returns a pointer mesh node
						*/
						static MeshNode* makeMeshNode(
							std::vector<repo_vector_t>                  &vertices,
							std::vector<repo_face_t>                    &faces,
							std::vector<repo_vector_t>                  &normals,
							std::vector<std::vector<float>>             &boundingBox,
							std::vector<std::vector<repo_vector2d_t>>   &uvChannels,
							std::vector<repo_color4d_t>                 &colors,
							std::vector<std::vector<float>>             &outline,
							const std::string                           &name = std::string(),
							const int                                   &apiLevel = REPO_NODE_API_LEVEL_1);

						/**
						* Create a Revision Node
						* @param user name of the user who is commiting this project
						* @param branch UUID of the branch
						* @param currentNodes vector of current nodes (unique IDs)
						* @param added    vector of what nodes are added    (shared IDs) 
						* @param removed  vector of what nodes are deleted  (shared IDs) 
						* @param modified vector of what nodes are modified (shared IDs) 
						* @param modified vector of what nodes are modified (shared IDs)
						* @param parent   UUID of parent (in a vector)
						* @param message  A message to describe what this commit is for (optional)
						* @param tag      A tag for this specific revision (optional)
						* @param apiLevel API Level(optional)
						* @return returns a texture node
						*/

						static RevisionNode* makeRevisionNode(
							const std::string			 &user,
							const repoUUID              &branch,
							const std::vector<repoUUID> &currentNodes,
							const std::vector<repoUUID> &added,
							const std::vector<repoUUID> &removed,
							const std::vector<repoUUID> &modified,
							const std::vector<repoUUID> &parent = std::vector<repoUUID>(),
							const std::string            &message = std::string(),
							const std::string            &tag = std::string(),
							const int                    &apiLevel = REPO_NODE_API_LEVEL_1
							);


						/**
						* Create a Texture Node
						* @param name name of the texture node, with extension to indicate file format
						* @param memblock binary data to store
						* @param size size of the binary data in bytes
						* @param width width of the texture
						* @param height height of the texture
						* @param apiLevel API Level(optional)
						* @return returns a texture node
						*/
						static TextureNode* makeTextureNode(
							const std::string &name,
							const char        *memblock,
							const uint32_t    &size,
							const uint32_t    &width,
							const uint32_t    &height,
							const int         &apiLevel = REPO_NODE_API_LEVEL_1);

						/**
						* Create a Transformation Node
						* @param transMatrix a 4 by 4 transformation matrix
						* @param name name of the transformation(optional)
						* @param parents vector of parents uuid for this node (optional)
						* @param apiLevel API Level(optional)
						* @return returns a pointer Transformation node
						*/
						static TransformationNode* makeTransformationNode(
						const std::vector<std::vector<float>> &transMatrix,
						const std::string                     &name = std::string(),
						const std::vector<repoUUID>		  &parents = std::vector<repoUUID>(),
						const int                             &apiLevel = REPO_NODE_API_LEVEL_1);

				};
			}//namespace bson
		} //namespace model
	} //namespace core
} //namespace repo
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
* Allows Export functionality from 3D Repo World to SRC
*/


#pragma once

#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "repo_model_export_abstract.h"
#include "../../../core/model/collection/repo_scene.h"

namespace repo{
	namespace manipulator{
		namespace modelconvertor{
			class SRCModelExport : public AbstractModelExport
			{	
			public:
				/**
				* Default Constructor, export model with default settings
				* @param scene repo scene to convert
				*/
				SRCModelExport(const repo::core::model::RepoScene *scene);

				/**
				* Default Destructor
				*/
				virtual ~SRCModelExport();

				/**
				* Convert a Mesh Node into src format
				* @param mesh the mesh to convert
				* @param index a counter indiciating the mesh index
				* @return returns a buffer to the data
				*/
				std::vector<uint8_t> convertMesh(
					const repo::core::model::MeshNode* mesh,
					const size_t &index
					);

				/**
				* Export a repo scene graph to file
				* @param filePath path to destination file
				* @return returns true upon success
				*/
				bool exportToFile(
					const std::string &filePath);

				/**
				* Return the SRC file as raw bytes buffer
				* returns an empty vector if the export has failed
				*/
				std::vector<uint8_t> getFileAsBuffer();

				/**
				* Get supported file formats for this exporter
				*/
				static std::string getSupportedFormats();

				/**
				* @param filePath path to destination file (including file extension)
				* @return returns true upon success
				*/
				bool writeSceneToFile(
					const std::string &filePath);
				
			private:
				const repo::core::model::RepoScene *scene;
				bool convertSuccess;
				boost::property_tree::ptree tree;
				repo::core::model::RepoScene::GraphType gType;
				std::vector<uint8_t> fullDataBuffer;

				/**
				* Create a tree representation for the graph
				* This creates the header of the SRC
				*/
				bool generateTreeRepresentation();
				
				/**
				* Create a property tree with an array of ints
				* @param children 
				*/
				template <typename T>
				boost::property_tree::ptree createPTArray(std::vector<T> children) const
				{					
					boost::property_tree::ptree arrayTree;
					for (const auto &child : children)
					{
						boost::property_tree::ptree childTree;
						childTree.put("", child);
						arrayTree.push_back(std::make_pair("", childTree));
					}


					return arrayTree;		
				}

			};



		} //namespace modelconvertor
	} //namespace manipulator
} //namespace repo


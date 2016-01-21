/**
*  Copyright (C) 2016 3D Repo Ltd
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

#include "repo_property_tree.h"

using namespace repo::lib;

PropertyTree::PropertyTree()
{
}


PropertyTree::~PropertyTree()
{
}


//see http://stackoverflow.com/questions/2855741/why-boost-property-tree-write-json-saves-everything-as-string-is-it-possible-to
struct stringTranslator
{
	typedef std::string internal_type;
	typedef std::string external_type;

	boost::optional<std::string> get_value(const std::string &v) { return  v.substr(1, v.size() - 2); }
	boost::optional<std::string> put_value(const std::string &v) { return '"' + v + '"'; }
};

template <>
void PropertyTree::addToTree<std::string>(
	const std::string           &label,
	const std::string           &value)
{
	if (label.empty())
		tree.put(label, value, stringTranslator());
	else
		tree.add(label, value, stringTranslator());
}

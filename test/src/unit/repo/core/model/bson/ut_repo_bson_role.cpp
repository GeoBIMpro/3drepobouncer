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

#include <cstdlib>

#include <gtest/gtest.h>

#include <repo/core/model/collection/repo_scene.h>
#include <repo/core/model/bson/repo_bson_role.h>
#include <repo/core/model/bson/repo_bson_builder.h>

using namespace repo::core::model;

static RepoBSON buildRoleExample()
{
	RepoBSONBuilder builder;
	std::string database = "admin";
	std::string roleName = "testRole";
	builder << REPO_LABEL_ID << database + "." + roleName;
	builder << REPO_ROLE_LABEL_ROLE << roleName;
	builder << REPO_ROLE_LABEL_DATABASE << database;

	//====== Add Privileges ========
	RepoBSONBuilder privilegesBuilder;

	RepoBSONBuilder innerBsonBuilder, actionBuilder;
	RepoBSON resource = BSON(REPO_ROLE_LABEL_DATABASE << "testdb" << REPO_ROLE_LABEL_COLLECTION << "testCol");
	innerBsonBuilder << REPO_ROLE_LABEL_RESOURCE << resource;

	std::vector<std::string> actions = { "find", "update" };

	for (size_t aCount = 0; aCount < actions.size(); ++aCount)
	{
		actionBuilder << std::to_string(aCount) << actions[aCount];
	}

	innerBsonBuilder.appendArray(REPO_ROLE_LABEL_ACTIONS, actionBuilder.obj());

	privilegesBuilder << "0" << innerBsonBuilder.obj();

	builder.appendArray(REPO_ROLE_LABEL_PRIVILEGES, privilegesBuilder.obj());


	//====== Add Inherited Roles ========

	RepoBSON inheritedRole = BSON( "role" << "readWrite" << "db" << "canarywharf");
	RepoBSONBuilder inheritedRolesBuilder;

	inheritedRolesBuilder << "0" << inheritedRole;
	builder.appendArray(REPO_ROLE_LABEL_INHERITED_ROLES, inheritedRolesBuilder.obj());
	return builder.obj();
}

static RepoBSON buildRoleExample2()
{
	RepoBSONBuilder builder;
	std::string database = "admin";
	std::string roleName = "testRole";
	builder << REPO_LABEL_ID << database + "." + roleName;
	builder << REPO_ROLE_LABEL_ROLE << roleName;
	builder << REPO_ROLE_LABEL_DATABASE << database;

	//====== Add Privileges ========
	RepoBSONBuilder privilegesBuilder;

	RepoBSONBuilder innerBsonBuilder, actionBuilder;
	RepoBSON resource = BSON(REPO_ROLE_LABEL_DATABASE << "testdb" << REPO_ROLE_LABEL_COLLECTION << "testCol");
	innerBsonBuilder << REPO_ROLE_LABEL_RESOURCE << resource;

	std::vector<std::string> actions = { "find", "update" };

	for (size_t aCount = 0; aCount < actions.size(); ++aCount)
	{
		actionBuilder << std::to_string(aCount) << actions[aCount];
	}

	innerBsonBuilder.appendArray(REPO_ROLE_LABEL_ACTIONS, actionBuilder.obj());

	privilegesBuilder << "0" << innerBsonBuilder.obj();

	RepoBSON resource2 = BSON(REPO_ROLE_LABEL_DATABASE << "testdb" << REPO_ROLE_LABEL_COLLECTION << "project.history");
	innerBsonBuilder << REPO_ROLE_LABEL_RESOURCE << resource2;

	std::vector<std::string> actions = {"find"};

	for (size_t aCount = 0; aCount < actions.size(); ++aCount)
	{
		actionBuilder << std::to_string(aCount) << actions[aCount];
	}

	innerBsonBuilder.appendArray(REPO_ROLE_LABEL_ACTIONS, actionBuilder.obj());

	builder.appendArray(REPO_ROLE_LABEL_PRIVILEGES, privilegesBuilder.obj());


	//====== Add Inherited Roles ========

	RepoBSON inheritedRole = BSON("role" << "readWrite" << "db" << "canarywharf");
	RepoBSONBuilder inheritedRolesBuilder;

	inheritedRolesBuilder << "0" << inheritedRole;
	builder.appendArray(REPO_ROLE_LABEL_INHERITED_ROLES, inheritedRolesBuilder.obj());
	return builder.obj();
}

TEST(RepoRoleTest, ConstructorTest)
{
	RepoRole role;

	EXPECT_TRUE(role.isEmpty());

	RepoRole role2(buildRoleExample());
	EXPECT_FALSE(role2.isEmpty());
}

TEST(RepoRoleTest, DBActionToStringTest)
{
	EXPECT_EQ("insert", RepoRole::dbActionToString(DBActions::INSERT));
	EXPECT_EQ("update", RepoRole::dbActionToString(DBActions::UPDATE));
	EXPECT_EQ("remove", RepoRole::dbActionToString(DBActions::REMOVE));
	EXPECT_EQ("find", RepoRole::dbActionToString(DBActions::FIND));
	EXPECT_EQ("createUser", RepoRole::dbActionToString(DBActions::CREATE_USER));
	EXPECT_EQ("createRole", RepoRole::dbActionToString(DBActions::CREATE_ROLE));
	EXPECT_EQ("dropRole", RepoRole::dbActionToString(DBActions::DROP_ROLE));
	EXPECT_EQ("grantRole", RepoRole::dbActionToString(DBActions::GRANT_ROLE));
	EXPECT_EQ("revokeRole", RepoRole::dbActionToString(DBActions::REVOKE_ROLE));
	EXPECT_EQ("viewRole", RepoRole::dbActionToString(DBActions::VIEW_ROLE));
	EXPECT_EQ("", RepoRole::dbActionToString(DBActions::UNKNOWN));
	EXPECT_EQ("", RepoRole::dbActionToString((DBActions)100600));

	//This is a bit hacky, but we want to ensure that every enum is catered for.
	//so loop through until we hit UNKNOWN and make sure every enum returns a valid string

	uint32_t i = 0;
	for (; i < (uint32_t)DBActions::UNKNOWN-1; ++i)
	{
		EXPECT_NE("", RepoRole::dbActionToString((DBActions)i));
	}

	//Also ensure UNKNOWN is the last member in the enum class
	i += 2;
	EXPECT_EQ("", RepoRole::dbActionToString((DBActions)i));
}

TEST(RepoRoleTest, TranslatePermissionsTest_READ)
{
	std::vector<RepoPermission> permissions;

	std::string projectName = "project";
	permissions.push_back({"test", projectName, AccessRight::READ});

	std::vector<RepoPrivilege> privileges = RepoRole::translatePermissions(permissions);

	EXPECT_EQ(RepoScene::getProjectExtensions().size(), privileges.size());

	for (const auto &p : privileges)
	{
		if (p.collection == (projectName + ".scene"))
		{
			EXPECT_EQ(1, p.actions.size());
			EXPECT_EQ(DBActions::FIND, p.actions[0]);
		}
		else if (p.collection == (projectName + ".stash.3drepo"))
		{
			EXPECT_EQ(1, p.actions.size());
			EXPECT_EQ(DBActions::FIND, p.actions[0]);
		}
		else if (p.collection == (projectName + ".stash.src"))
		{
			EXPECT_EQ(1, p.actions.size());
			EXPECT_EQ(DBActions::FIND, p.actions[0]);
		}
		else if (p.collection == (projectName + ".history"))
		{
			EXPECT_EQ(1, p.actions.size());
			EXPECT_EQ(DBActions::FIND, p.actions[0]);
		}
		else if (p.collection == (projectName + ".wayfinder"))
		{
			EXPECT_EQ(1, p.actions.size());
			EXPECT_EQ(DBActions::FIND, p.actions[0]);
		}
		else if (p.collection == (projectName + ".issues"))
		{
			EXPECT_EQ(3, p.actions.size());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::FIND) != p.actions.end());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::UPDATE) != p.actions.end());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::INSERT) != p.actions.end());
		}
		else
		{
			//There is an extension the test doesn't know about. Make sure 
			//this extension is catered for within the roles function
			//and add it as another case here.
			FAIL();
		}
	}

	EXPECT_EQ(0, RepoRole::translatePermissions(std::vector<RepoPermission>()).size());
}

TEST(RepoRoleTest, TranslatePermissionsTest_WRITE)
{
	std::vector<RepoPermission> permissions;

	std::string projectName = "project";
	permissions.push_back({ "test", projectName, AccessRight::WRITE });

	std::vector<RepoPrivilege> privileges = RepoRole::translatePermissions(permissions);

	EXPECT_EQ(RepoScene::getProjectExtensions().size(), privileges.size());

	for (const auto &p : privileges)
	{
		if (p.collection == (projectName + ".scene"))
		{
			EXPECT_EQ(1, p.actions.size());
			EXPECT_EQ(DBActions::INSERT, p.actions[0]);
		}
		else if (p.collection == (projectName + ".stash.3drepo"))
		{
			EXPECT_EQ(1, p.actions.size());
			EXPECT_EQ(DBActions::INSERT, p.actions[0]);
		}
		else if (p.collection == (projectName + ".stash.src"))
		{
			EXPECT_EQ(1, p.actions.size());
			EXPECT_EQ(DBActions::INSERT, p.actions[0]);
		}
		else if (p.collection == (projectName + ".history"))
		{
			EXPECT_EQ(1, p.actions.size());
			EXPECT_EQ(DBActions::INSERT, p.actions[0]);
		}
		else if (p.collection == (projectName + ".wayfinder"))
		{
			EXPECT_EQ(3, p.actions.size());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::REMOVE) != p.actions.end());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::UPDATE) != p.actions.end());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::INSERT) != p.actions.end());
		}
		else if (p.collection == (projectName + ".issues"))
		{
			EXPECT_EQ(2, p.actions.size());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::UPDATE) != p.actions.end());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::INSERT) != p.actions.end());
		}
		else
		{
			//There is an extension the test doesn't know about. Make sure 
			//this extension is catered for within the roles function
			//and add it as another case here.
			FAIL();
		}
	}

	EXPECT_EQ(0, RepoRole::translatePermissions(std::vector<RepoPermission>()).size());
}

TEST(RepoRoleTest, TranslatePermissionsTest_READWRITE)
{
	std::vector<RepoPermission> permissions;

	std::string projectName = "project";
	permissions.push_back({ "test", projectName, AccessRight::READ_WRITE });

	std::vector<RepoPrivilege> privileges = RepoRole::translatePermissions(permissions);

	EXPECT_EQ(RepoScene::getProjectExtensions().size(), privileges.size());

	for (const auto &p : privileges)
	{
		if (p.collection == (projectName + ".scene"))
		{
			EXPECT_EQ(2, p.actions.size());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::INSERT) != p.actions.end());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::FIND) != p.actions.end());
		}
		else if (p.collection == (projectName + ".stash.3drepo"))
		{
			EXPECT_EQ(2, p.actions.size());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::INSERT) != p.actions.end());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::FIND) != p.actions.end());
		}
		else if (p.collection == (projectName + ".stash.src"))
		{
			EXPECT_EQ(2, p.actions.size());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::INSERT) != p.actions.end());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::FIND) != p.actions.end());
		}
		else if (p.collection == (projectName + ".history"))
		{
			EXPECT_EQ(2, p.actions.size());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::INSERT) != p.actions.end());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::FIND) != p.actions.end());
		}
		else if (p.collection == (projectName + ".wayfinder"))
		{
			EXPECT_EQ(4, p.actions.size());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::FIND) != p.actions.end());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::REMOVE) != p.actions.end());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::UPDATE) != p.actions.end());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::INSERT) != p.actions.end());
		}
		else if (p.collection == (projectName + ".issues"))
		{
			EXPECT_EQ(3, p.actions.size());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::FIND) != p.actions.end());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::UPDATE) != p.actions.end());
			EXPECT_TRUE(std::find(p.actions.begin(), p.actions.end(), DBActions::INSERT) != p.actions.end());
		}
		else
		{
			//There is an extension the test doesn't know about. Make sure 
			//this extension is catered for within the roles function
			//and add it as another case here.
			FAIL();
		}
	}

	EXPECT_EQ(0, RepoRole::translatePermissions(std::vector<RepoPermission>()).size());
}

TEST(RepoRoleTest, UpdateActions)
{
	//This is already tested via translatePermissions. If the above passes this function has to run fine.
	std::vector<DBActions> vec;

	RepoRole::updateActions("scene", AccessRight::READ, vec);
	EXPECT_EQ(1, vec.size());
	EXPECT_EQ(DBActions::FIND, vec[0]);

	//Expect no duplicates (check the [A, A] will become [A])
	RepoRole::updateActions("scene", AccessRight::READ, vec);
	EXPECT_EQ(1, vec.size());
	EXPECT_EQ(DBActions::FIND, vec[0]);


	RepoRole::updateActions("scene", AccessRight::WRITE, vec);
	EXPECT_EQ(2, vec.size());
	EXPECT_TRUE(std::find(vec.begin(), vec.end(), DBActions::FIND) != vec.end());
	EXPECT_TRUE(std::find(vec.begin(), vec.end(), DBActions::INSERT) != vec.end());

	//again, expect no duplicates (check the [A, B, A] will become [A, B])
	RepoRole::updateActions("scene", AccessRight::READ, vec);
	EXPECT_EQ(2, vec.size());

	vec.clear();
	
}

TEST(RepoRoleTest, GetDatabaseTest)
{
	RepoRole empty;
	RepoRole role(buildRoleExample());

	EXPECT_EQ("admin", role.getDatabase());
	EXPECT_EQ("", empty.getDatabase());
}

TEST(RepoRoleTest, GetInheritedRolesTest)
{
	RepoRole empty;
	RepoRole role(buildRoleExample());

	std::vector<std::pair<std::string, std::string>> inheritedRoles = role.getInheritedRoles();

	ASSERT_EQ(1, inheritedRoles.size());
	EXPECT_EQ("canarywharf",inheritedRoles[0].first);
	EXPECT_EQ("readWrite", inheritedRoles[0].second);

	EXPECT_EQ(0, empty.getInheritedRoles().size());
}

TEST(RepoRoleTest, GetNameTest)
{
	RepoRole empty;
	RepoRole role(buildRoleExample());

	EXPECT_EQ("testRole", role.getName());
	EXPECT_EQ("", empty.getName());
}

TEST(RepoRoleTest, GetPrivilegesTest)
{

	RepoRole empty;
	RepoRole role(buildRoleExample());

	std::vector<RepoPrivilege> privileges = role.getPrivileges();
	ASSERT_EQ(1, privileges.size());
	EXPECT_EQ("testdb", privileges[0].database);
	EXPECT_EQ("testCol", privileges[0].collection);
	EXPECT_EQ(2, privileges[0].actions.size());
	EXPECT_TRUE(std::find(privileges[0].actions.begin(), privileges[0].actions.end(), DBActions::UPDATE) != privileges[0].actions.end());
	EXPECT_TRUE(std::find(privileges[0].actions.begin(), privileges[0].actions.end(), DBActions::FIND) != privileges[0].actions.end());


	EXPECT_EQ(0, empty.getPrivileges().size());

}

TEST(RepoRoleTest, GetProjectAccessRightsTest)
{
	RepoRole empty;
	RepoRole role(buildRoleExample());
	RepoRole role2(buildRoleExample2());
}
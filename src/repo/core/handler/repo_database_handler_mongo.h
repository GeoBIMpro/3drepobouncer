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
 *  Mongo database handler
 */

#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <boost/log/trivial.hpp>

#if defined(_WIN32) || defined(_WIN64)
	#include <WinSock2.h>
	#include <Windows.h>

	#define strcasecmp _stricmp
#endif

#include <mongo/client/dbclient.h>

#include "repo_database_handler_abstract.h"
#include "connectionpool/repo_connection_pool_mongo.h"
#include "../model/repo_node_utils.h"
#include "../model/bson/repo_bson.h"
#include "../model/bson/repo_bson_builder.h"
#include "../../lib/repo_stack.h"

namespace repo{
	namespace core{
		namespace handler {
			class MongoDatabaseHandler : public AbstractDatabaseHandler{
			public:
				/*
				*	=================================== Public Fields ========================================
				*/
				static const std::string SYSTEM_ROLES_COLLECTION;//! "system.roles"
				static const std::string AUTH_MECH;//! Authentication mechanism. currently MONGO-CR since mongo v2.6
				//! Built in any database roles. See http://docs.mongodb.org/manual/reference/built-in-roles/
				static const std::list<std::string> ANY_DATABASE_ROLES;
				//! Built in admin database roles. See http://docs.mongodb.org/manual/reference/built-in-roles/
				static const std::list<std::string> ADMIN_ONLY_DATABASE_ROLES;

				/*
				 *	=============================================================================================
				 */

				/*
				 *	=================================== Public Functions ========================================
				 */

				/**
				 * A Deconstructor
				 */
				~MongoDatabaseHandler();

				/**
				 * Returns the instance of MongoDatabaseHandler
				 * @param errMsg error message if this fails
				 * @param host hostname of the database
				 * @param port port number
				 * @param number of maximum simultaneous connections
				 * @param username username for authentication
				 * @param password for authentication
				 * @param pwDigested true if password is digested
				 * @return Returns the single instance
				 */
				static MongoDatabaseHandler* MongoDatabaseHandler::getHandler(
					std::string &errMsg,
					const std::string &host,
					const int         &port,
					const uint32_t    &maxConnections,
					const std::string &dbName,
					const std::string &username = std::string(),
					const std::string &password = std::string(),
					const bool        &pwDigested = false);


				/**
				* Generates a BSON object containing user credentials
				* @param username user name for authentication
				* @param password password of the user
				* @param pwDigested true if pw is digested
				* @return returns the constructed BSON object, or 0 if username is empty
				*/
				repo::core::model::bson::RepoBSON* createBSONCredentials(
					const std::string &username,
					const std::string &password,
					const bool        &pwDigested = false);


				/*
				*	------------- Database info lookup --------------
				*/

				/**
				* Get a list of all available collections.
				* Use mongo.nsGetCollection() to remove database from the returned string.
				* @param name of the database
				* @return a list of collection names
				*/
				std::list<std::string> getCollections(const std::string &database);


				/**
				 * Get a list of all available databases, alphabetically sorted by default.
				 * @param sort the database
				 * @return returns a list of database names
				 */
				std::list<std::string> getDatabases(const bool &sorted = true);

				/** get the associated projects for the list of database.
				 * @param list of database
				 * @return returns a map of database -> list of projects
				 */
				std::map<std::string, std::list<std::string> > getDatabasesWithProjects(
					const std::list<std::string> &databases, const std::string &projectExt);

				/**
				 * Get a list of projects associated with a given database (aka company account).
				 * It will filter out any collections without that isn't *.projectExt
				 * @param list of database
				 * @param extension that determines it is a project (scene)
				 * @return list of projects for the database
				 */
				std::list<std::string> getProjects(const std::string &database, const std::string &projectExt);

				/*
				*	------------- Database operations (insert/delete/update) --------------
				*/

				/**
				 * Insert a single document in database.collection
				 * @param database name
				 * @param collection name
				 * @param document to insert
				 * @param errMsg error message should it fail
				 * @return returns true upon success
				 */
				bool insertDocument(
					const std::string &database,
					const std::string &collection,
					const repo::core::model::bson::RepoBSON &obj,
					std::string &errMsg);

				/**
				* Update/insert a single document in database.collection
				* If the document exists, update it, if it doesn't, insert it
				* @param database name
				* @param collection name
				* @param document to insert
				* @param if it is an update, overwrites the document instead of updating the fields it has
				* @param errMsg error message should it fail
				* @return returns true upon success
				*/
				bool upsertDocument(
					const std::string &database,
					const std::string &collection,
					const repo::core::model::bson::RepoBSON &obj,
					const bool        &overwrite,
					std::string &errMsg);

				/*
				*	------------- Query operations --------------
				*/

				/**
				* Given a list of unique IDs, find all the documents associated to them
				* @param name of database
				* @param name of collection
				* @param array of uuids in a BSON object
				* @return a vector of RepoBSON objects associated with the UUIDs given
				*/
				std::vector<repo::core::model::bson::RepoBSON> findAllByUniqueIDs(
					const std::string& database,
					const std::string& collection,
					const repo::core::model::bson::RepoBSON& uuids);

				/**
				*Retrieves the first document matching given Shared ID (SID), sorting is descending
				* (newest first)
				* @param name of database
				* @param name of collectoin
				* @param share id
				* @param field to sort by
				* @param fields to retrieve
				* @return returns a bson object containing those fields
				*/
				repo::core::model::bson::RepoBSON findOneBySharedID(
					const std::string& database,
					const std::string& collection,
					const repoUUID& uuid,
					const std::string& sortField);

				/**
				*Retrieves the document matching given Unique ID (SID), sorting is descending
				* @param name of database
				* @param name of collectoin
				* @param share id
				* @return returns the matching bson object
				*/
				mongo::BSONObj findOneByUniqueID(
					const std::string& database,
					const std::string& collection,
					const repoUUID& uuid);

				/*
				 *	=============================================================================================
				 */


			protected:
				/*
				*	=================================== Protected Fields ========================================
				*/
				static MongoDatabaseHandler *handler; /* !the single instance of this class*/
				/*
				*	=============================================================================================
				*/

			private:
				/*
				 *	=================================== Private Fields ========================================
				 */

				connectionPool::MongoConnectionPool *workerPool;

				/*!
				 * Map holding database name as key and <username, password digest> as a
				 * value. User can be authenticated on any number of databases on a single
				 * connection.
				 */
				std::map<std::string, std::pair<std::string, std::string> > databasesAuthentication;

				mongo::ConnectionString dbAddress; /* !address of the database (host:port)*/

				/*
				 *	=============================================================================================
				 */

				/*
				 *	=================================== Private Functions ========================================
				 */

				/**
				 * Constructor is private because this class follows the singleton pattern
				 * @param dbAddress ConnectionString that holds the address to the mongo database
				 * @param maxConnections max. number of connections to the database
				 * @param username user name for authentication (optional)
				 * @param password password of the user (optional)
				 * @param pwDigested true if pw is digested (optional)
				 */
				MongoDatabaseHandler::MongoDatabaseHandler(
					const mongo::ConnectionString &dbAddress,
					const uint32_t                &maxConnections,
					const std::string             &dbName,
					const std::string             &username = std::string(),
					const std::string             &password = std::string(),
					const bool                    &pwDigested = false);


				/**
				* Generates a mongo BSON object for authentication
				* @param database database to authenticate against
				* @param username user name for authentication
				* @param password password of the user
				* @param pwDigested true if pw is digested
				* @return returns the constructed BSON object, or 0 if username is empty
				*/
				mongo::BSONObj* createAuthBSON(
					const std::string &database,
					const std::string &username,
					const std::string &password,
					const bool        &pwDigested);
				/**
				* Turns a list of fields that needs to be returned by the query into a bson
				* object.
				* @param list of field names to return
				* @param ID field is excluded by default unless explicitly stated in the list.
				*/
				mongo::BSONObj MongoDatabaseHandler::fieldsToReturn(
					const std::list<std::string>& fields,
					bool excludeIdField = false);

				/**
				 * Extract collection name from namespace (db.collection)
				 * @param namespace as string
				 * @return returns a string with just the collection name
				 */
				std::string getCollectionFromNamespace(const std::string &ns);

				/**
				 * Given a database and collection name, returns its namespace
				 * Basically, append database + "." + collection
				 * @param database name
				 * @param collection name
				 * @return namespace
				 */
				std::string MongoDatabaseHandler::getNamespace(
					const std::string &database,
					const std::string &collection);

				/**
				* Extract database name from namespace (db.collection)
				* @param namespace as string
				* @return returns a string with just the database name
				*/
				std::string MongoDatabaseHandler::getProjectFromCollection(const std::string &ns, const std::string &projectExt);

				/**
				* Compares two strings.
				* @param string 1
				* @param string 2
				* @return returns true if string 1 matches (or is greater than) string 2
				*/
				static bool caseInsensitiveStringCompare(const std::string& s1, const std::string& s2);

				/*
				*	=========================================================================================
				*/

			};

		} /* namespace handler */
	}
}


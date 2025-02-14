// Pegasus Frontend
// Copyright (C) 2017-2022  Mátyás Mustoha
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.


#pragma once

#include "GameListModel.h"

#include <QString>

#ifdef Q_CC_MSVC
// MSVC has troubles with forward declared QML model types
#include "model/gaming/Game.h"
#endif

namespace model
{
	class Assets;
}
namespace model
{
	class Game;
}


namespace model
{
	struct CollectionData
	{
		explicit CollectionData(QString name);

		const QString name;
		QString sort_by;

		QString summary;
		QString description;

		QString common_launch_cmd;
		QString common_launch_workdir;
		QString common_relative_basedir;

		void set_short_name(const QString &);

		const QString &short_name() const
		{ return m_short_name; }

	private:
		QString m_short_name;
	};


	class Collection : public QObject
	{
	Q_OBJECT

	public:
#define GETTER(type, name, field) \
    type name() const { return m_data.field; }

		GETTER(const QString&, name, name)

		GETTER(const QString&, sortBy, sort_by)

		GETTER(const QString&, shortName, short_name())

		GETTER(const QString&, summary, summary)

		GETTER(const QString&, description, description)

		GETTER(const QString&, commonLaunchCmd, common_launch_cmd)

		GETTER(const QString&, commonLaunchWorkdir, common_launch_workdir)

		GETTER(const QString&, commonLaunchCmdBasedir, common_relative_basedir)

#undef GETTER


#define SETTER(type, name, field) \
    Collection& set##name(type val) { m_data.field = std::move(val); return *this; }

		SETTER(QString, SortBy, sort_by)

		SETTER(QString, Summary, summary)

		SETTER(QString, Description, description)

		SETTER(QString, CommonLaunchCmd, common_launch_cmd)

		SETTER(QString, CommonLaunchWorkdir, common_launch_workdir)

		SETTER(QString, CommonLaunchCmdBasedir, common_relative_basedir)

		Collection &setShortName(QString val)
		{
			m_data.set_short_name(std::move(val));
			return *this;
		}

#undef SETTER


		Q_PROPERTY(QString name READ name CONSTANT)
		Q_PROPERTY(QString sortBy READ sortBy CONSTANT)
		Q_PROPERTY(QString shortName READ shortName CONSTANT)
		Q_PROPERTY(QString summary READ summary CONSTANT)
		Q_PROPERTY(QString description READ description CONSTANT)

		Q_PROPERTY(QVariantMap extra READ extraMap CONSTANT)

		const QVariantMap &extraMap() const
		{ return m_extra; }

		QVariantMap &extraMapMut()
		{ return m_extra; }


		const Assets &assets() const
		{ return *m_assets; }

		Assets &assetsMut()
		{ return *m_assets; }

		Assets* assetsPtr() const
		{ return m_assets; }

		Q_PROPERTY(model::Assets* assets READ assetsPtr CONSTANT)

		Collection &setGames(std::vector<model::Game*> &&);

		GameListModel* gameList() const
		{ return m_games; }

		Q_PROPERTY(ObjectListModel* games READ gameList CONSTANT)

	public:
		explicit Collection(QString name, QObject* parent = nullptr);

		void finalize();

	private:
		CollectionData m_data;
		QVariantMap m_extra;
		Assets* const m_assets;

		GameListModel* m_games = nullptr;
	};


	bool sort_collections(const model::Collection* const, const model::Collection* const);
} // namespace model

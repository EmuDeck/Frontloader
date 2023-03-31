// Pegasus Frontend
// Copyright (C) 2017-2018  Mátyás Mustoha
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


#include "AutobootGames.h"

#include "AppSettings.h"
#include "Log.h"
#include "model/Api.h"

#include <QCoreApplication>
#include <QDir>
#include <QLocale>


namespace
{
	std::vector<model::AutobootEntry> find_available_games(model::ApiObject* api_object)
	{
		std::vector<model::AutobootEntry> available_games;

        auto games = api_object->allGames();

        for (const auto& game : games->entries())
        {
            available_games.emplace_back(game->title());
        }

		return available_games;
	}
} // namespace


namespace model
{

	AutobootEntry::AutobootEntry(QString name)
			: name(std::move(name))
	{}

	AutobootGames::AutobootGames(QObject* parent)
			: QAbstractListModel(parent), m_role_names({
					                                           {Roles::Name, QByteArrayLiteral("name")},
			                                           }), m_games(), m_current_idx(0)
	{

	}

	void AutobootGames::initialize(model::ApiObject* api_object)
	{
		m_games = find_available_games(api_object);
        m_current_idx = 0;
		select_preferred_game();
	}

	void AutobootGames::select_preferred_game()
	{
		// A. Try to use the saved config value
		if (select_game(AppSettings::general.autobootGame))
			return;

		// B. Fall back to the default
		if (select_game(AppSettings::general.DEFAULT_AUTOBOOT_GAME))
			return;

		Q_UNREACHABLE();
	}

	bool AutobootGames::select_game(const QString &name)
	{
		if (name.isEmpty())
			return false;

		for (size_t idx = 0; idx < m_games.size(); idx++)
		{
			if (m_games.at(idx).name == name)
			{
				m_current_idx = idx;
				return true;
			}
		}

		return false;
	}

	int AutobootGames::rowCount(const QModelIndex &parent) const
	{
		if (parent.isValid())
			return 0;

		return static_cast<int>(m_games.size());
	}

	QVariant AutobootGames::data(const QModelIndex &index, int role) const
	{
		if (!index.isValid() || rowCount() <= index.row())
			return {};

		const auto &game = m_games.at(static_cast<size_t>(index.row()));
		switch (role)
		{
			case Roles::Name:
				return game.name;
			default:
				return {};
		}
	}

	void AutobootGames::setCurrentIndex(int idx_int)
	{
		const auto idx = static_cast<size_t>(idx_int);

		// verify
		if (idx == m_current_idx)
			return;

		if (m_games.size() <= idx)
		{
			Log::warning(LOGMSG("Invalid autoboot index #%1").arg(idx));
			return;
		}

		// load
		m_current_idx = idx;
		emit autobootChanged();

		// remember
		AppSettings::general.autobootGame = m_games.at(idx).name;
		AppSettings::save_config();
	}

} // namespace model

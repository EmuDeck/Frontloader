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


#pragma once

#include "utils/MoveOnly.h"
#include "model/Api.h"

#include <QAbstractListModel>
#include <QTranslator>


namespace model
{
	struct AutobootEntry
	{
		QString name;

		explicit AutobootEntry(QString name);
		MOVE_ONLY(AutobootEntry)
	};


	class AutobootGames : public QAbstractListModel
	{
	Q_OBJECT
		Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY autobootChanged)
		Q_PROPERTY(QString currentName READ currentName NOTIFY autobootChanged)

	public:
		explicit AutobootGames(QObject* parent = nullptr);

		enum Roles
		{
			Name = Qt::UserRole + 1,
		};

		int rowCount(const QModelIndex &parent = QModelIndex()) const override;

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

		QHash<int, QByteArray> roleNames() const override
		{ return m_role_names; }

		int currentIndex() const
		{ return static_cast<int>(m_current_idx); }

		void setCurrentIndex(int);

		QString currentName() const
		{ return m_games.at(m_current_idx).name; }

		void initialize(model::ApiObject* api_object);

	signals:

		void autobootChanged();

	private:
		const QHash<int, QByteArray> m_role_names;
		std::vector<AutobootEntry> m_games;

		size_t m_current_idx;
		QTranslator m_translator;

		void select_preferred_game();

		bool select_game(const QString &name);

	};
} // namespace model

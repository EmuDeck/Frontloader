// Pegasus Frontend
// Copyright (C) 2018  Mátyás Mustoha
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

#include <QString>
#include <QJsonDocument>


namespace providers
{

#ifdef WITH_JSON_CACHE
	void cache_json(const QString& provider_prefix,
					const QString& provider_dir,
					const QString& entryname,
					const QByteArray& bytes);
	QJsonDocument read_json_from_cache(const QString& provider_prefix,
									   const QString& provider_dir,
									   const QString& entryname);
	void delete_cached_json(const QString& provider_prefix,
							const QString& provider_dir,
							const QString& entryname);
#endif // WITH_JSON_CACHE

#ifdef Q_OS_LINUX

	QString steam_flatpak_data_dir();

#endif // Q_OS_LINUX

	QString find_steam_call();

} // namespace providers

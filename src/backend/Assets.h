// Pegasus Frontend
// Copyright (C) 2017  Mátyás Mustoha
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

#include <QHash>
#include <QVector>


enum class AssetType : unsigned char {
    UNKNOWN,

    BOX_FRONT,
    BOX_BACK,
    BOX_SPINE,
    BOX_FULL,
    CARTRIDGE,
    LOGO,
    POSTER,

    ARCADE_MARQUEE,
    ARCADE_BEZEL,
    ARCADE_PANEL,
    ARCADE_CABINET_L,
    ARCADE_CABINET_R,

    UI_TILE,
    UI_BANNER,
    UI_STEAMGRID,
    BACKGROUND,
    MUSIC,

    SCREENSHOTS,
    VIDEOS,
};

/// Definitions of supported asset types and file formats
class Assets {
public:
    static const QVector<AssetType> singleTypes;
    static const QVector<AssetType> multiTypes;
};

inline uint qHash(const AssetType& key, uint seed) {
    return ::qHash(static_cast<unsigned char>(key), seed);
}
inline uint qHash(const AssetType& key) {
    return ::qHash(static_cast<unsigned char>(key));
}
namespace std {
template<> struct hash<AssetType> {
    std::size_t operator()(const AssetType& key) const {
        return static_cast<std::size_t>(key);
    }
};
} // namespace std

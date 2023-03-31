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

// NOTE: This is a regular enum, for Qt::Key compatibility
enum GamepadKeyId
{
	A = 0x100000,
	B, X, Y,
	L1, L2, L3,
	R1, R2, R3,
	SELECT,
	START,
	GUIDE,
};


enum class GamepadButton : unsigned char
{
	INVALID,
	UP, DOWN, LEFT, RIGHT,
	NORTH, SOUTH, EAST, WEST,
	L1, L2, L3,
	R1, R2, R3,
	SELECT,
	START,
	GUIDE,
};
enum class GamepadAxis : unsigned char
{
	INVALID,
	LEFTX, LEFTY,
	RIGHTX, RIGHTY,
};

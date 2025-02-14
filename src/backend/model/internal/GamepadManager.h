// Pegasus Frontend
// Copyright (C) 2017-2019  Mátyás Mustoha
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

#include "CliArgs.h"
#include "Gamepad.h"
#include "GamepadListModel.h"
#include "GamepadManagerBackend.h"
#include "utils/HashMap.h"

#include <QObject>
#include <QString>

#ifndef Q_OS_ANDROID

#  include "GamepadAxisNavigation.h"
#  include "GamepadButtonNavigation.h"

#endif


namespace model
{

	class GamepadManager : public QObject
	{
	Q_OBJECT
		Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

	public:
		explicit GamepadManager(QObject* parent = nullptr);

		void start(const backend::CliArgs &args);

		void stop();

		enum class GMButton
		{
			Invalid,
			Up, Down, Left, Right,
			North, South, East, West,
			L1, L2, L3,
			R1, R2, R3,
			Select,
			Start,
			Guide,
		};

		Q_ENUM(GMButton)

		enum class GMAxis
		{
			Invalid,
			LeftX, LeftY,
			RightX, RightY,
		};

		Q_ENUM(GMAxis)

		Q_PROPERTY(ObjectListModel* devices READ devices CONSTANT)

		Q_INVOKABLE void configureButton(int deviceId, model::GamepadManager::GMButton button);

		Q_INVOKABLE void configureAxis(int deviceId, model::GamepadManager::GMAxis axis);

		Q_INVOKABLE void cancelConfiguration();

		Q_INVOKABLE QString mappingForAxis(int deviceId, model::GamepadManager::GMAxis axis) const;

		Q_INVOKABLE QString mappingForButton(int deviceId, model::GamepadManager::GMButton button) const;

	signals:

		void connected(int deviceId);

		void disconnected(QString deviceId);

		void buttonConfigured(int deviceId, model::GamepadManager::GMButton button);

		void axisConfigured(int deviceId, model::GamepadManager::GMAxis axis);

		void configurationCanceled(int deviceId);

	private slots:

		void bkOnConnected(int, QString);

		void bkOnDisconnected(int);

		void bkOnNameChanged(int, QString);

		void bkOnButtonCfg(int, GamepadButton);

		void bkOnAxisCfg(int, GamepadAxis);

		void bkOnButtonChanged(int, GamepadButton, bool);

		void bkOnAxisChanged(int, GamepadAxis, double);

	private:
		GamepadListModel* devices() const
		{ return m_devices; }

	private:
		const QString m_log_tag;

		GamepadListModel* const m_devices;
		GamepadManagerBackend* const m_backend;

#ifndef Q_OS_ANDROID
		GamepadButtonNavigation padbuttonnav;
		GamepadAxisNavigation padaxisnav;
#endif
	};

} // namespace model


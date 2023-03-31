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


#include "AutobootState.h"
#include "Log.h"

namespace model
{
	AutobootState::AutobootState(QObject* parent)
			: QObject(parent)
	{}

	void AutobootState::reset()
	{
		m_progress = 0.f;
		emit progressChanged();

		m_stage = QString();
		emit stageChanged();
	}

	void AutobootState::cancel()
	{
		Log::info("Autoboot cancelled");
		m_progress = 0.f;
		emit progressChanged();

		m_stage = QString();
		emit stageChanged();

		m_running = false;
		m_cancelled = true;
		emit runningChanged();
		emit cancelledChanged();
	}


	void AutobootState::onAutobootStarted()
	{
		m_running = true;
		m_cancelled = false;
		emit runningChanged();
		emit cancelledChanged();
	}

	void AutobootState::onAutobootProgressChanged(float value, QString stage)
	{
		Q_ASSERT(value <= 1.f);

		if (value > m_progress)
		{
			m_progress = value;
			emit progressChanged();
		}
		if (stage != m_stage)
		{
			m_stage = std::move(stage);
			emit stageChanged();
		}

	}

	void AutobootState::onAutobootFinished()
	{
		m_running = false;
		m_cancelled = false;
		emit runningChanged();
		emit cancelledChanged();
	}
} // namespace model

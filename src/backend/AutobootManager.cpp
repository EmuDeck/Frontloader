// Pegasus Frontend
// Copyright (C) 2017-2020  Mátyás Mustoha
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


#include "AutobootManager.h"

#include "AppSettings.h"
#include "Log.h"

#include <QtConcurrent/QtConcurrent>

AutobootManager::AutobootManager(QObject* parent)
		: QObject(parent)
{
}

void AutobootManager::run(const QString& autobootGame)
{
	Q_ASSERT(!m_future.isRunning());

	m_future = QtConcurrent::run([this, autobootGame]
	                             {
		                             emit autobootStarted();

		                             const qint64 progress_sections = AppSettings::general.autobootTimeout;

		                             m_progress_step = 1.f / std::max<qint64>(progress_sections, 1);
		                             m_current_progress = 0.f;
		                             QElapsedTimer progress_timer;
		                             progress_timer.start();
		                             m_current_stage = QString("Booting %1 in %2 seconds").arg(autobootGame, QString::number(progress_sections - (progress_timer.elapsed() / 1000)));
		                             while (progress_timer.elapsed() / 1000 <= progress_sections && !m_future.isCanceled())
		                             {
			                             m_current_stage = QString("Booting %1 in %2 seconds").arg(autobootGame, QString::number(progress_sections - (progress_timer.elapsed() / 1000)));
			                             emit autobootProgressChanged(m_current_progress, m_current_stage);
			                             Log::info(m_current_stage);

										 QThread::sleep(1);

			                             m_current_progress += m_progress_step;
									 }
		                             m_current_progress = 1.f;
		                             m_current_stage = QString();
		                             emit autobootProgressChanged(m_current_progress, m_current_stage);
		                             emit autobootFinished();
	                             });
}

void AutobootManager::onAutobootCancelled()
{
	m_future.cancel();
	m_future.waitForFinished();
}

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

#include <QObject>

namespace model
{
	class Collection;
}
namespace model
{
	class Game;
}


namespace model
{
	class AutobootState : public QObject
	{
	Q_OBJECT

		Q_PROPERTY(bool running READ running NOTIFY runningChanged)
		Q_PROPERTY(bool cancelled READ cancelled NOTIFY cancelledChanged)
		Q_PROPERTY(QString stage READ stage NOTIFY stageChanged)
		Q_PROPERTY(float progress READ progress NOTIFY progressChanged)

	public:
		explicit AutobootState(QObject* parent = nullptr);

		Q_INVOKABLE void reset();

		Q_INVOKABLE void cancel();

		bool running() const
		{ return m_running; }

		bool cancelled() const
		{ return m_cancelled; }

		QString stage() const
		{ return m_stage; }

		float progress() const
		{ return m_progress; }

	public slots:

		void onAutobootStarted();

		void onAutobootProgressChanged(float value, QString stage);

		void onAutobootFinished();

	signals:

		void runningChanged();

		void stageChanged();

		void progressChanged();

		void cancelledChanged();

		void autobootCancelled();

	private:
		bool m_running = false;
		bool m_cancelled;
		QString m_stage;
		float m_progress;
	};
} // namespace model

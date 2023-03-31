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


#include "Backend.h"

#include "AppSettings.h"
#include "Log.h"
#include "FrontendLayer.h"
#include "ProcessLauncher.h"
#include "ScriptRunner.h"
#include "platform/PowerCommands.h"
#include "types/AppCloseType.h"

// For type registration
#include "model/Api.h"
#include "model/keys/Key.h"
#include "model/gaming/Assets.h"
#include "model/gaming/GameFile.h"
#include "model/internal/Internal.h"
#include "utils/FolderListModel.h"
#include "SortFilterProxyModel/qqmlsortfilterproxymodel.h"
#include "SortFilterProxyModel/filters/filtersqmltypes.h"
#include "SortFilterProxyModel/proxyroles/proxyrolesqmltypes.h"
#include "SortFilterProxyModel/sorters/sortersqmltypes.h"
#include "Paths.h"
#include "model/internal/AutobootState.h"

#include <QGuiApplication>
#include <QQmlEngine>

#if defined(WITH_SDL_GAMEPAD) || defined(WITH_SDL_POWER)

#include <SDL.h>
#include <QDirIterator>

#endif

namespace model
{
	class Key;
}
namespace model
{
	class Keys;
}
class FolderListModel;


namespace
{
	void print_metainfo()
	{
		Log::info(LOGMSG("frontloader " GIT_REVISION " (" GIT_DATE ")"));
		Log::info(LOGMSG("Running on %1 (%2, %3)").arg(
				QSysInfo::prettyProductName(),
				QSysInfo::currentCpuArchitecture(),
				QGuiApplication::platformName()));
		Log::info(LOGMSG("Qt version %1").arg(qVersion()));
	}

	void register_api_classes()
	{
		// register API classes:
		//   this should come before the ApiObject constructor,
		//   as that may produce language change signals

		constexpr auto API_URI = "Pegasus.Model";
		const QString error_msg = LOGMSG("Sorry, you cannot create this type in QML.");

		qmlRegisterUncreatableType<model::Collection>(API_URI, 0, 7, "Collection", error_msg);
		qmlRegisterUncreatableType<model::Game>(API_URI, 0, 2, "Game", error_msg);
		qmlRegisterUncreatableType<model::Assets>(API_URI, 0, 2, "GameAssets", error_msg);
		qmlRegisterUncreatableType<model::Locales>(API_URI, 0, 11, "Locales", error_msg);
		qmlRegisterUncreatableType<model::Themes>(API_URI, 0, 11, "Themes", error_msg);
		qmlRegisterUncreatableType<model::Providers>(API_URI, 0, 11, "Providers", error_msg);
		qmlRegisterUncreatableType<model::Key>(API_URI, 0, 10, "Key", error_msg);
		qmlRegisterUncreatableType<model::Keys>(API_URI, 0, 10, "Keys", error_msg);
		qmlRegisterUncreatableType<model::GamepadManager>(API_URI, 0, 12, "GamepadManager", error_msg);
		qmlRegisterUncreatableType<model::DeviceInfo>(API_URI, 0, 13, "Device", error_msg);

		// QML utilities
		qmlRegisterType<FolderListModel>("Pegasus.FolderListModel", 1, 0, "FolderListModel");

		// third-party
		qqsfpm::registerSorterTypes();
		qqsfpm::registerFiltersTypes();
		qqsfpm::registerProxyRoleTypes();
		qqsfpm::registerQQmlSortFilterProxyModelTypes();
	}

	int get_default_sink_volume()
	{
		QProcess process;
		process.start("/bin/bash", QStringList() << "-c"
		                                         << "pactl get-sink-volume @DEFAULT_SINK@ | grep -Po '\\d+(?=%)' | head -n 1");
		process.waitForFinished();

		return process.readAllStandardOutput().toInt();
	}

	void increase_volume()
	{
		if (get_default_sink_volume() >= 100)
		{
			QProcess::execute("/usr/bin/pactl", QStringList() << "set-sink-volume" << "@DEFAULT_SINK@" << "100%");
		}
		else
		{
			QProcess::execute("/usr/bin/pactl", QStringList() << "set-sink-volume" << "@DEFAULT_SINK@" << "+10%");
		}
	}

	void decrease_volume()
	{
		QProcess::execute("/usr/bin/pactl", QStringList() << "set-sink-volume" << "@DEFAULT_SINK@" << "-10%");
	}

	void on_volume_change(VolumeChangeType type)
	{
		switch (type)
		{
			case VolumeChangeType::UP:
				increase_volume();
				break;
			case VolumeChangeType::DOWN:
				decrease_volume();
				break;
		}
	}

	void on_app_close(AppCloseType type)
	{
		if (type == AppCloseType::SUSPEND)
		{
			return platform::power::suspend();
		}
		ScriptRunner::run(ScriptEvent::QUIT);
		switch (type)
		{
			case AppCloseType::REBOOT:
				ScriptRunner::run(ScriptEvent::REBOOT);
				break;
			case AppCloseType::SHUTDOWN:
				ScriptRunner::run(ScriptEvent::SHUTDOWN);
				break;
			default:
				break;
		}

		Log::info(LOGMSG("Closing frontloader, goodbye!"));
		Log::close();

		QCoreApplication::quit();
		switch (type)
		{
			case AppCloseType::REBOOT:
				platform::power::reboot();
				break;
			case AppCloseType::SHUTDOWN:
				platform::power::shutdown();
				break;
			default:
				break;
		}
	}
} // namespace


namespace backend
{

	Backend::Backend()
			: Backend(CliArgs{})
	{}

	Backend::~Backend()
	{
		delete m_launcher;
		delete m_frontend;
		delete m_providerman;
		delete m_api_private;
		delete m_api_public;
		unloadFrontloader();
#if defined(WITH_SDL_GAMEPAD) || defined(WITH_SDL_POWER)
		SDL_Quit();
#endif
	}

	Backend::Backend(const CliArgs &args)
			: m_args(args)
	{
		// Make sure this comes before any file related operations
//    AppSettings::general.portable = args.portable;

		Log::init(args.silent);
		print_metainfo();
		register_api_classes();

		AppSettings::load_providers();
		AppSettings::load_config();
		loadFrontloader();

		m_api_public = new model::ApiObject(args);
		m_api_private = new model::Internal(args);
		m_frontend = new FrontendLayer(m_api_public, m_api_private);
		m_launcher = new ProcessLauncher();
		m_providerman = new ProviderManager();
		m_autobootman = new AutobootManager();

		// the following communication is required because process handling
		// and destroying/rebuilding the frontend stack are asynchronous tasks;
		// see the relevant classes

		// the Api asks the Launcher to start the game
		QObject::connect(m_api_public, &model::ApiObject::launchGameFile,
		                 m_launcher, &ProcessLauncher::onLaunchRequested);

		// the Launcher tries to start the game, ask the Frontend
		// to tear down the UI, then report back to the Api
		QObject::connect(m_launcher, &ProcessLauncher::processLaunchOk,
		                 m_api_public, &model::ApiObject::onGameLaunchOk);

		QObject::connect(m_api_public, &model::ApiObject::gameFileLaunched,
		                 m_providerman, &ProviderManager::onGameLaunched);

		QObject::connect(m_launcher, &ProcessLauncher::processLaunchError,
		                 m_api_public, &model::ApiObject::onGameLaunchError);

		QObject::connect(m_launcher, &ProcessLauncher::processLaunchOk,
		                 [this]()
		                 { onProcessLaunched(); });

		QObject::connect(m_frontend, &FrontendLayer::teardownComplete,
		                 m_launcher, &ProcessLauncher::onTeardownComplete);

		// when the game ends, the Launcher wakes up the Api and the Frontend
		QObject::connect(m_launcher, &ProcessLauncher::processFinished,
		                 m_api_public, &model::ApiObject::onGameProcessFinished);
		QObject::connect(m_api_public, &model::ApiObject::gameFileFinished,
		                 m_providerman, &ProviderManager::onGameFinished);

		QObject::connect(m_launcher, &ProcessLauncher::processFinished,
		                 [this]()
		                 { onProcessFinished(); });

		// Setting changes
		QObject::connect(m_api_private->settings().localesPtr(), &model::Locales::localeChanged,
		                 m_api_public, &model::ApiObject::onLocaleChanged);
		QObject::connect(m_api_private->settings().themesPtr(), &model::Themes::themeChanged,
		                 m_api_public, &model::ApiObject::onThemeChanged);
		QObject::connect(m_api_private->settings().keyEditorPtr(), &model::KeyEditor::keysChanged,
		                 m_api_public->keysPtr(), &model::Keys::refresh_keys);
		QObject::connect(m_api_private->settingsPtr(), &model::Settings::providerReloadingRequested,
		                 [this]()
		                 { onScanRequested(); });

		QObject::connect(m_api_public, &model::ApiObject::favoritesChanged,
		                 [this]()
		                 { onFavoritesChanged(); });

		// Loading progress
		QObject::connect(m_providerman, &ProviderManager::scanStarted,
		                 m_api_private->scannerPtr(), &model::ScannerState::onScanStarted);
		QObject::connect(m_providerman, &ProviderManager::scanFinished,
		                 m_api_private->scannerPtr(), &model::ScannerState::onScanFinished);
		QObject::connect(m_providerman, &ProviderManager::scanProgressChanged,
		                 m_api_private->scannerPtr(), &model::ScannerState::onScanProgressChanged);
		QObject::connect(m_providerman, &ProviderManager::scanFinished,
		                 [this]()
		                 {
			                 onScanFinished();
							 m_api_private->settings().autobootGames().initialize(m_api_public);
			                 if (AppSettings::general.autoboot)
			                 {
				                 m_autobootman->run(AppSettings::general.autobootGame);
			                 }
		                 });
		QObject::connect(m_api_public, &model::ApiObject::gamedataReady,
		                 m_api_private->scannerPtr(), &model::ScannerState::onUiReady);

		QObject::connect(m_api_private->autobootPtr(), &model::AutobootState::autobootCancelled,
		                 m_autobootman, &AutobootManager::onAutobootCancelled);

		QObject::connect(m_autobootman, &AutobootManager::autobootStarted,
		                 m_api_private->autobootPtr(), &model::AutobootState::onAutobootStarted);
		QObject::connect(m_autobootman, &AutobootManager::autobootFinished,
		                 m_api_private->autobootPtr(), &model::AutobootState::onAutobootFinished);
		QObject::connect(m_autobootman, &AutobootManager::autobootProgressChanged,
		                 m_api_private->autobootPtr(), &model::AutobootState::onAutobootProgressChanged);
		QObject::connect(m_autobootman, &AutobootManager::autobootFinished,
		                 [this]()
		                 {
			                 if (!m_api_private->autobootPtr()->cancelled())
			                 {
				                 auto game = std::find_if(m_api_public->allGames()->entries().begin(),
				                                          m_api_public->allGames()->entries().end(),
				                                          [&](model::Game* game)
				                                          {
					                                          return game->title() == AppSettings::general.autobootGame;
				                                          });
				                 if (game != m_api_public->allGames()->entries().end())
				                 {
					                 (*game)->launch();
				                 }
			                 }
		                 });

		// partial QML reload
		QObject::connect(&m_api_private->meta(), &model::Meta::qmlClearCacheRequested,
		                 m_frontend, &FrontendLayer::clearCache);

		// quit/reboot/shutdown request
		QObject::connect(&m_api_private->system(), &model::System::appCloseRequested, on_app_close);

		// volume buttons
		QObject::connect(&m_api_private->system(), &model::System::volumeChangeRequested, on_volume_change);
	}

	void Backend::copyDirectoryNested(const QString &from, const QString &to)
	{

		QDirIterator it(from, QDirIterator::Subdirectories);

		while (it.hasNext())
		{

			QString file_in = it.next();

			QFileInfo file_info = QFileInfo(file_in);

			QString file_out = file_in;
			file_out.replace(from, to);

			if (file_info.isFile())
			{
				//is file copy
				QFile::copy(file_in, file_out);
				if (QFileInfo(file_out).suffix() == "front" || QFileInfo(file_out).suffix() == "sh" ||
				    QFileInfo(file_out).suffix() == "AppImage")
				{
					QFile::setPermissions(file_out,
					                      QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner |
					                      QFileDevice::ReadGroup | QFileDevice::WriteGroup | QFileDevice::ExeGroup);
				}
			}

			if (file_info.isDir())
			{
				//dir mkdir
				QDir dir(file_out);
				if (!dir.exists())
					dir.mkpath(".");
			}
		}
	}

	void Backend::loadFrontloader()
	{
		if ((!QFile::exists(paths::app_dir_path() + "/scripts")) ||
		    (!QFile::exists(paths::app_dir_path() + "/services")) ||
		    (!QFile::exists(paths::app_dir_path() + "/frontends")) ||
		    (!QFile::exists(paths::app_dir_path() + "/frontends/metadata.frontloader.txt")))
		{
			QDir(paths::app_dir_path()).mkdir("scripts");
			QDir(paths::app_dir_path()).mkdir("services");
			QDir(paths::app_dir_path()).mkdir("frontends");
			copyDirectoryNested(":/frontloader", paths::app_dir_path());
			if (!QFile::exists(QDir::homePath() + "/.local/bin/opengamepadui"))
			{
				QFile::remove(paths::app_dir_path() + "/frontends/OGUI.front");
			}
			if (!QFile::exists(QDir::homePath() + "/Emulation") && !QFile::exists("/var/run/media/mmcblk0p1/Emulation"))
			{
				QFile::remove(paths::app_dir_path() + "/frontends/ES-DE.front");
			}
		}
		else
		{
			copyDirectoryNested(":/frontloader/frontends", paths::app_dir_path() + "/frontends");
			if (!QFile::exists(QDir::homePath() + "/.local/bin/opengamepadui"))
			{
				QFile::remove(paths::app_dir_path() + "/frontends/OGUI.front");
			}
			if (!QFile::exists(QDir::homePath() + "/Emulation") && !QFile::exists("/var/run/media/mmcblk0p1/Emulation"))
			{
				QFile::remove(paths::app_dir_path() + "/frontends/ES-DE.front");
			}
		}
		QProcess::execute("/bin/bash", QStringList() << paths::app_dir_path() + "/scripts/Constructor.sh");
	}

	void Backend::unloadFrontloader()
	{
		QProcess::execute("/bin/bash", QStringList() << paths::app_dir_path() + "/scripts/Destructor.sh");
	}


	void Backend::start()
	{
		m_api_private->settings().postInit();
		onProcessFinished();
		onScanRequested();
	}

	void Backend::onScanRequested()
	{
		m_api_public->clearGameData();
		m_providerman->run();
	}

	void Backend::onScanFinished()
	{
		std::vector<model::Collection*> colls;
		std::swap(m_providerman->foundCollections(), colls);

		std::vector<model::Game*> games;
		std::swap(m_providerman->foundGames(), games);

		m_api_public->setGameData(std::move(colls), std::move(games));
	}

	void Backend::onFavoritesChanged()
	{
		m_providerman->onFavoritesChanged(m_api_public->allGames()->entries());
	}

	void Backend::onProcessLaunched()
	{
		m_frontend->teardown();
		m_api_private->gamepad().stop();
	}

	void Backend::onProcessFinished()
	{
		m_frontend->rebuild();
		m_api_private->gamepad().start(m_args);
	}

} // namespace backend

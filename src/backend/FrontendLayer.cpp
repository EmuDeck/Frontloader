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


#include "FrontendLayer.h"

#include "Paths.h"
#include "imggen/BlurhashProvider.h"
#include "utils/DiskCachedNAM.h"
#include "Log.h"

#ifdef Q_OS_ANDROID
#include "platform/AndroidAppIconProvider.h"
#endif

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlNetworkAccessManagerFactory>
#include <QtX11Extras/QX11Info>
#include <QWindow>
#include <X11/Xproto.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>


namespace {

class DiskCachedNAMFactory : public QQmlNetworkAccessManagerFactory {
public:
    QNetworkAccessManager* create(QObject* parent) override;
};

QNetworkAccessManager* DiskCachedNAMFactory::create(QObject* parent)
{
    return utils::create_disc_cached_nam(parent);
}

} // namespace


FrontendLayer::FrontendLayer(QObject* const api_public, QObject* const api_private, QObject* parent)
    : QObject(parent)
    , m_api_public(api_public)
    , m_api_private(api_private)
    , m_engine(nullptr)
{
    // Note: the pointer to the Api is non-owning and constant during the runtime
}

void set_property(Atom& atom, uint32_t data, Display* display, Window window)
{
	XChangeProperty(display, window, atom, XA_CARDINAL, 32, 1, reinterpret_cast<const unsigned char*>(&data), 1);
	XFlush(display);
}

void FrontendLayer::rebuild()
{
    Q_ASSERT(!m_engine);

    m_engine = new QQmlApplicationEngine(this);
    m_engine->addImportPath(QStringLiteral("lib/qml"));
    m_engine->addImportPath(QStringLiteral("qml"));
    m_engine->setNetworkAccessManagerFactory(new DiskCachedNAMFactory);

    m_engine->addImageProvider(QStringLiteral("blurhash"), new BlurhashProvider);
#ifdef Q_OS_ANDROID
    m_engine->addImageProvider(QStringLiteral("androidicons"), new AndroidAppIconProvider);
#endif
    m_engine->rootContext()->setContextProperty(QStringLiteral("api"), m_api_public);
    m_engine->rootContext()->setContextProperty(QStringLiteral("Api"), m_api_public);
    m_engine->rootContext()->setContextProperty(QStringLiteral("Internal"), m_api_private);
    m_engine->load(QUrl(QStringLiteral("qrc:/frontend/main.qml")));

	Display* display  = QX11Info::display();
	Atom steamAtom = XInternAtom(display, "STEAM_BIGPICTURE", False);
	Atom overlayAtom = XInternAtom(display, "STEAM_OVERLAY", False);
	Atom gameAtom = XInternAtom(display, "STEAM_GAME", False);
	QWindow* qWindow = ((QWindow*)m_engine->rootObjects().first());
	if (qWindow->isWindowType())
	{
		Window window = qWindow->winId();
		set_property(steamAtom, 1, display, window);
		set_property(overlayAtom, 0, display, window);
		set_property(gameAtom, 769, display, window);
	}

    emit rebuildComplete();
}

void FrontendLayer::teardown()
{
    Q_ASSERT(m_engine);

    // signal forwarding
    connect(m_engine, &QQmlApplicationEngine::destroyed,
            this, &FrontendLayer::teardownComplete);

    m_engine->deleteLater();
    m_engine = nullptr;
}

void FrontendLayer::clearCache()
{
    Q_ASSERT(m_engine);
    m_engine->clearComponentCache();
}

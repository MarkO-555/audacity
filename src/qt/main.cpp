/*  SPDX-License-Identifier: GPL-2.0-or-later */

#include <QtCore/QDebug>
#include <QFontDatabase>
#include <QQmlComponent>
#include <QGuiApplication>

#include "AudioIO.h"
#include "BasicSettings.h"
#include "Prefs.h"
#include "Project.h"
#include "QMLEngineFactory.h"
#include "ProjectQMLEnvironment.h"

class StubSettings final : public audacity::BasicSettings
{
public:
   wxString GetGroup() const override { return ""; }
   wxArrayString GetChildGroups() const override { return {}; }
   wxArrayString GetChildKeys() const override { return {}; }
   bool HasEntry(const wxString& key) const override { return false; }
   bool HasGroup(const wxString& key) const override { return false; }
   bool Remove(const wxString& key) override { return false; }
   void Clear() override { }
   bool Read(const wxString& key, bool* value) const override { return false; }
   bool Read(const wxString& key, int* value) const override { return false; }
   bool Read(const wxString& key, long* value) const override { return false; }
   bool Read(const wxString& key, long long* value) const override { return false; }
   bool Read(const wxString& key, double* value) const override { return false; }
   bool Read(const wxString& key, wxString* value) const override { return false; }
   bool Write(const wxString& key, bool value) override { return false; }
   bool Write(const wxString& key, int value) override { return false; }
   bool Write(const wxString& key, long value) override { return false; }
   bool Write(const wxString& key, long long value) override { return false; }
   bool Write(const wxString& key, double value) override { return false; }
   bool Write(const wxString& key, const wxString& value) override { return false; }
   bool Flush() noexcept override { return false; }

protected:
   void DoBeginGroup(const wxString& prefix) override { }
   void DoEndGroup() noexcept override { }
};

audacity::ApplicationSettings::Scope applicationSettings {
   []{ return std::make_unique<StubSettings>(); }
};

static audacity::QMLEngineFactory::Scope qmlEngineFactory {
   [] {
      auto engine = std::make_unique<QQmlEngine>();
      engine->addImportPath(QString(":%1").arg(AUDACITY_QML_RESOURCE_PREFIX));
      return engine;
   }
};

int main(int argc, char *argv[])
{
   QGuiApplication app(argc, argv);

   QFontDatabase::addApplicationFont(":/fonts/MusescoreIcon.ttf");
   QFontDatabase::addApplicationFont(":/fonts/Lato-Bold.ttf");
   QFontDatabase::addApplicationFont(":/fonts/Lato-BoldItalic.ttf");
   QFontDatabase::addApplicationFont(":/fonts/Lato-Italic.ttf");
   QFontDatabase::addApplicationFont(":/fonts/Lato-Regular.ttf");

   InitPreferences(audacity::ApplicationSettings::Call());
   AudioIO::Init();

   auto project = AudacityProject::Create();
   auto& engine = audacity::ProjectQMLEnvironment::Get(*project).GetEngine();

   QQmlComponent applicationWindowComponent (&engine, "qrc:/qml/main.qml");
   auto applicationWindow = std::unique_ptr<QObject>(applicationWindowComponent.create());
   if(applicationWindow == nullptr)
   {
      qDebug() << "Unable to load main.qml: " <<
         applicationWindowComponent.errorString();
      return -1;
   }

   return app.exec();
}

/*=========================================================================

   Program: ParaView
   Module:  pqHelpWindowWebEngine.h

   Copyright (c) 2005,2006 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2.

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

========================================================================*/
#ifndef pqHelpWindowWebEngine_h
#define pqHelpWindowWebEngine_h

/**
 *============================================================================
 * This is an internal header used by pqHelpWindow.
 * This header gets included when PARAVIEW_USE_QTWEBENGINE is ON.
 *============================================================================
 */

#include "vtkParaViewDeprecation.h"

#include <QBuffer>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QTimer>
#include <QWebEngineHistory>
#include <QWebEngineProfile>
#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlSchemeHandler>
#include <QWebEngineView>

namespace
{

class pqUrlSchemeHandler : public QWebEngineUrlSchemeHandler
{
  typedef QWebEngineUrlSchemeHandler Superclass;

public:
  pqUrlSchemeHandler(QHelpEngineCore* engine)
    : Engine(engine)
  {
  }
  ~pqUrlSchemeHandler() = default;

  void requestStarted(QWebEngineUrlRequestJob* request) override
  {
    QMap<QString, QString> extension_type_map;
    extension_type_map["jpg"] = "image/jpeg";
    extension_type_map["jpeg"] = "image/jpeg";
    extension_type_map["png"] = "image/png";
    extension_type_map["gif"] = "image/gif";
    extension_type_map["tiff"] = "image/tiff";
    extension_type_map["htm"] = "text/html";
    extension_type_map["html"] = "text/html";
    extension_type_map["css"] = "text/css";
    extension_type_map["xml"] = "text/xml";

    QUrl url = request->requestUrl();
    QString extension = QFileInfo(url.path()).suffix().toLower();
    QString content_type = extension_type_map.value(extension, "text/plain");

    QByteArray array = this->Engine->fileData(url);
    QBuffer* buffer = new QBuffer;
    buffer->setData(array);
    buffer->open(QIODevice::ReadOnly);
    connect(buffer, &QIODevice::aboutToClose, buffer, &QObject::deleteLater);
    request->reply(content_type.toUtf8(), buffer);
  }

private:
  QHelpEngineCore* Engine;
};
//----------------------------------------------------------------------------------
/**
 * Extend QWebView to support the interface expected in pqBrowserTemplate.
 */
class pqWebView : public QWebEngineView
{
  typedef QWebEngineView Superclass;

public:
  pqWebView(QWidget* parentObject)
    : Superclass(parentObject)
  {
  }
  ~pqWebView() = default;

  static pqWebView* newInstance(QHelpEngine* engine, pqHelpWindow* self)
  {
    pqWebView* instance = new pqWebView(self);
    QWebEngineProfile* profile = QWebEngineProfile::defaultProfile();
    profile->installUrlSchemeHandler("qthelp", new pqUrlSchemeHandler(engine));
    self->connect(instance, &pqWebView::loadFinished, self, &pqHelpWindow::updateHistoryButtons);
    return instance;
  }

  PARAVIEW_DEPRECATED_IN_5_12_0(
    "This constructor is deprecated, please use the specialized one instead: pqWebView* "
    "newInstance(QHelpEngine* engine, pqHelpWindow* self)");
  static pqWebView* newInstance(QHelpEngine* engine, QWidget* parentObject)
  {
    pqWebView* instance = new pqWebView(parentObject);
    QWebEngineProfile* profile = QWebEngineProfile::defaultProfile();
    profile->installUrlSchemeHandler("qthelp", new pqUrlSchemeHandler(engine));
    return instance;
  }

  QUrl url() { return this->history()->currentItem().url(); }

  QUrl goBackward()
  {
    this->history()->back();
    return this->history()->currentItem().url();
  }

  QUrl goForward()
  {
    this->history()->forward();
    return this->history()->currentItem().url();
  }

  bool canGoBackward() { return this->history()->canGoBack(); }

  bool canGoForward() { return this->history()->canGoForward(); }

private:
  Q_DISABLE_COPY(pqWebView)
};

} // end of namespace
#endif

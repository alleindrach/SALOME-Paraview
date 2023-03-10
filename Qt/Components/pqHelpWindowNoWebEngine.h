/*=========================================================================

   Program: ParaView
   Module:  pqHelpWindowNoWebEngine.h

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
#ifndef pqHelpWindowNoWebEngine_h
#define pqHelpWindowNoWebEngine_h

/**
 *============================================================================
 * This is an internal header used by pqHelpWindow.
 * This header gets included when PARAVIEW_USE_QTWEBENGINE is OFF.
 *============================================================================
 */

#include <QTextBrowser>

namespace
{
// ****************************************************************************
//            CLASS pqTextBrowser
// ****************************************************************************
/**
 * Internal class used to add overload the QTextBrowser
 */
class pqTextBrowser : public QTextBrowser
{
public:
  pqTextBrowser(QHelpEngine* helpEngine, QWidget* _parent = nullptr)
  {
    this->HelpEngine = helpEngine;
    this->setParent(_parent);
    this->setOpenLinks(false);
  }

  ~pqTextBrowser() override = default;
  static pqTextBrowser* newInstance(QHelpEngine* engine, pqHelpWindow* self)
  {
    pqTextBrowser* instance = new pqTextBrowser(engine, self);
    self->connect(instance, &pqTextBrowser::anchorClicked, self,
      QOverload<const QUrl&>::of(&pqHelpWindow::showPage));
    self->connect(
      instance, &pqTextBrowser::historyChanged, self, &pqHelpWindow::updateHistoryButtons);
    return instance;
  }

  void setUrl(const QUrl& url) { this->setSource(url); }

  QUrl url() { return this->source(); }

  QUrl goBackward()
  {
    this->backward();
    return this->source();
  }

  QUrl goForward()
  {
    this->forward();
    return this->source();
  }

  bool canGoBackward() { return this->isBackwardAvailable(); }

  bool canGoForward() { return this->isForwardAvailable(); }

protected:
  /**
   * Implementation reference from:
   * http://doc.qt.digia.com/qq/qq28-qthelp.html
   */
  QVariant loadResource(int type, const QUrl& url) override
  {
    if (url.scheme() == "qthelp")
    {
      return QVariant(this->HelpEngine->fileData(url));
    }
    else
    {
      return QTextBrowser::loadResource(type, url);
    }
  }
  QPointer<QHelpEngine> HelpEngine;
};

} // end of namespace

#endif

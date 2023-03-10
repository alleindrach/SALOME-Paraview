/*=========================================================================

  Program:   ParaView
  Plugin:    NodeEditor

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*-------------------------------------------------------------------------
  ParaViewPluginsNodeEditor - BSD 3-Clause License - Copyright (C) 2021 Jonas Lukasczyk

  See the Copyright.txt file provided
  with ParaViewPluginsNodeEditor for license information.
-------------------------------------------------------------------------*/

#ifndef pqNodeEditorView_h
#define pqNodeEditorView_h

#include <QGraphicsView>

class QWheelEvent;
class QKeyEvent;
class QAction;

/**
 * This class extends QGraphicsView to rehandle MouseWheelEvents for zooming,
 * as well as keyboard events.
 */
class pqNodeEditorView : public QGraphicsView
{
  Q_OBJECT
  typedef QGraphicsView Superclass;

public:
  /**
   * Create a view for the specified scene. Also construct a new pqDeleteReaction
   * for whenever a user tries to delete a node using the `Del` key on the keyboard.
   */
  pqNodeEditorView(QGraphicsScene* scene, QWidget* parent = nullptr);

  void triggerDeleteAction() const;

  ~pqNodeEditorView() override = default;

Q_SIGNALS:
  /**
   * Triggered when user ask for creating / deleting annotation nodes. Current key is 'N'.
   */
  void annotate(bool del);

protected:
  void wheelEvent(QWheelEvent* event) override;

private:
  QAction* deleteAction;
};

#endif // pqNodeEditorView_h

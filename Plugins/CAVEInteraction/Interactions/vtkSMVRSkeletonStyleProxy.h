/*=========================================================================
 * NOTE !!!! This is really a test Interactor Style until I learn how to include new ones!

   Program: ParaView
   Module:  vtkSMVRSkeletonStyleProxy.h

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
#ifndef vtkSMVRSkeletonStyleProxy_h
#define vtkSMVRSkeletonStyleProxy_h

#include "vtkInteractionStylesModule.h" // for export macro
#include "vtkNew.h"
#include "vtkSMVRInteractorStyleProxy.h"

class vtkCamera;
class vtkMatrix4x4;
class vtkSMRenderViewProxy;
class vtkSMDoubleVectorProperty;
class vtkSMIntVectorProperty;
struct vtkVREvent;

class VTKINTERACTIONSTYLES_EXPORT vtkSMVRSkeletonStyleProxy : public vtkSMVRInteractorStyleProxy
{
public:
  static vtkSMVRSkeletonStyleProxy* New();
  vtkTypeMacro(vtkSMVRSkeletonStyleProxy, vtkSMVRInteractorStyleProxy);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkSMVRSkeletonStyleProxy();
  ~vtkSMVRSkeletonStyleProxy();

  void HandleButton(const vtkVREvent& event) override;
  void HandleAnalog(const vtkVREvent& event) override;
  void HandleTracker(const vtkVREvent& event) override;

  bool EnableReport;

  bool IsInitialTransRecorded;
  bool IsInitialRotRecorded;

  vtkNew<vtkMatrix4x4> InverseInitialTransMatrix;
  vtkNew<vtkMatrix4x4> InverseInitialRotMatrix;

  vtkNew<vtkMatrix4x4> CachedTransMatrix;
  vtkNew<vtkMatrix4x4> CachedRotMatrix;

private:
  vtkSMVRSkeletonStyleProxy(const vtkSMVRSkeletonStyleProxy&); // Not implemented
  void operator=(const vtkSMVRSkeletonStyleProxy&);            // Not implemented
};

#endif //  vtkSMVRSkeletonStyleProxy_h

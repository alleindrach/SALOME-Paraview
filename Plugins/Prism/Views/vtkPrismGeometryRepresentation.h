/*=========================================================================

  Program:   ParaView
  Module:    vtkPrismGeometryRepresentation.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkPrismGeometryRepresentation
 * @brief   representation for showing any datasets as external shell of polygons in prism view.
 *
 * vtkPrismGeometryRepresentation is a representation for showing polygon geometry in prism view.
 * It handles non-polygonal datasets by extracting external surfaces. One can
 * use this representation to show surface/wireframe/points/surface-with-edges.
 */
#ifndef vtkPrismGeometryRepresentation_h
#define vtkPrismGeometryRepresentation_h

#include "vtkDataObject.h" // needed for vtkDataObject::FieldAssociation
#include "vtkGeometryRepresentation.h"
#include "vtkPrismViewsModule.h" // needed for exports

class vtkExtractPolyDataGeometry;
class vtkPrismGeometryConverter;
class vtkSimulationToPrismFilter;

class VTKPRISMVIEWS_EXPORT vtkPrismGeometryRepresentation : public vtkGeometryRepresentation
{
public:
  static vtkPrismGeometryRepresentation* New();
  vtkTypeMacro(vtkPrismGeometryRepresentation, vtkGeometryRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * vtkAlgorithm::ProcessRequest() equivalent for rendering passes. This is
   * typically called by the vtkView to request meta-data from the
   * representations or ask them to perform certain tasks e.g.
   * PrepareForRendering.
   */
  int ProcessViewRequest(vtkInformationRequestKey* request_type, vtkInformation* inInfo,
    vtkInformation* outInfo) override;

  ///@{
  /**
   * Set If the Data are simulation data or not. If they are, they need to be converted to the prism
   * space.
   */
  void SetIsSimulationData(bool isSimulationData);
  vtkGetMacro(IsSimulationData, bool);
  ///@}

  ///@{
  /**
   * Control which AttributeType the filter operates on (point data or cell data
   * for vtkDataSets). The default value is vtkDataObject::POINT. The input value for
   * this function should be either vtkDataObject::POINT or vtkDataObject::CELL.
   *
   * The default is vtkDataObject::CELL.
   */
  void SetAttributeType(int type);
  int GetAttributeType();
  ///@}

  ///@{
  /**
   * Set the name of the array to use as the X axis.
   */
  void SetXArrayName(const char* name);
  const char* GetXArrayName();
  ///@}

  ///@{
  /**
   * Set the name of the array to use as the Y axis.
   */
  void SetYArrayName(const char* name);
  const char* GetYArrayName();
  ///@}

  ///@{
  /**
   * Set the name of the array to use as the Z axis.
   */
  void SetZArrayName(const char* name);
  const char* GetZArrayName();
  ///@}

  ///@{
  /**
   * Set if thresholding is enabled or not.
   */
  void SetEnableThresholding(bool enableThresholding);
  vtkGetMacro(EnableThresholding, bool);
  ///@}

  /**
   * Get the input bounds of the non simulation data.
   *
   * Note: These values are valid only if it's NOT simulation data.
   */
  vtkGetVector6Macro(NonSimulationDataInputBounds, double);

protected:
  vtkPrismGeometryRepresentation();
  ~vtkPrismGeometryRepresentation() override;

  /**
   * This method is called in the constructor. If the subclasses override any of
   * the iVar vtkObject's of this class e.g. the Mappers, GeometryFilter etc.,
   * they should call this method again in their constructor. It must be totally
   * safe to call this method repeatedly.
   */
  void SetupDefaults() override;

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  bool IsSimulationData = false;
  bool EnableThresholding = false;
  vtkNew<vtkSimulationToPrismFilter> SimulationToPrismFilter;
  vtkNew<vtkExtractPolyDataGeometry> ThresholdFilter;
  vtkNew<vtkPrismGeometryConverter> GeometryConverter;
  // Axis names are extracted from the field data of the input of the representation.
  char* XAxisName = nullptr;
  char* YAxisName = nullptr;
  char* ZAxisName = nullptr;
  vtkSetStringMacro(XAxisName);
  vtkGetStringMacro(XAxisName);
  vtkSetStringMacro(YAxisName);
  vtkGetStringMacro(YAxisName);
  vtkSetStringMacro(ZAxisName);
  vtkGetStringMacro(ZAxisName);
  double NonSimulationDataInputBounds[6];

  friend class vtkPrismView;

private:
  vtkPrismGeometryRepresentation(const vtkPrismGeometryRepresentation&) = delete;
  void operator=(const vtkPrismGeometryRepresentation&) = delete;
};

#endif // vtkPrismGeometryRepresentation_h

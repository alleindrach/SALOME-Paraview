#==========================================================================
#
#     Program: ParaView
#
#     Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
#     All rights reserved.
#
#     ParaView is a free software; you can redistribute it and/or modify it
#     under the terms of the ParaView license version 1.2.
#
#     See License_v1.2.txt for the full ParaView license.
#     A copy of this license can be obtained by contacting
#     Kitware Inc.
#     28 Corporate Drive
#     Clifton Park, NY 12065
#     USA
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#==========================================================================

# HDRi created by Andreas Mischok, shared under CC-0, from: https://polyhaven.com/a/spiaggia_di_mondello
vtk_encode_string(
  INPUT         "ShaderBallSkybox.jpg"
  HEADER_OUTPUT skybox_header
  SOURCE_OUTPUT skybox_source
  BINARY)

vtk_encode_string(
  INPUT         "ShaderBallTexture.png"
  HEADER_OUTPUT shaderBallTexture_header
  SOURCE_OUTPUT shaderBallTexture_source
  BINARY)

vtk_module_add_module(ParaView::VTKExtensionsShaderBall
  NOWRAP_CLASSES vtkShaderBallScene
  CLASSES ${textures}
  SOURCES "${skybox_source}" "${shaderBallTexture_source}"
  PRIVATE_HEADERS "${skybox_header}" "${shaderBallTexture_header}")

if (NOT paraview_use_materialeditor)
  message(FATAL_ERROR "ShadeBall required the MaterialEditor")
endif()

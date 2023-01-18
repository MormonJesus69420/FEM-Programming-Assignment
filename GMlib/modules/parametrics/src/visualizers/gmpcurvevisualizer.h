/**********************************************************************************
**
** Copyright (C) 1994 Narvik University College
** Contact: GMlib Online Portal at http://episteme.hin.no
**
** This file is part of the Geometric Modeling Library, GMlib.
**
** GMlib is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** GMlib is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with GMlib.  If not, see <http://www.gnu.org/licenses/>.
**
**********************************************************************************/





#ifndef GM_PARAMETRICS_VISUALIZERS_PCURVEVISUALIZER_H
#define GM_PARAMETRICS_VISUALIZERS_PCURVEVISUALIZER_H


// gmlib
#include <core/types/gmpoint.h>
#include <core/containers/gmdvector.h>
#include <core/utils/gmcolor.h>
#include <opengl/bufferobjects/gmvertexbufferobject.h>
#include <scene/gmvisualizer.h>


namespace GMlib {

  template <typename T, int n>
  class PCurve;

  template <typename T, int n>
  class PCurveVisualizer : public Visualizer {
  public:
    PCurveVisualizer();
    virtual ~PCurveVisualizer();

    virtual void  replot( const DVector< DVector< Vector<T, n> > >& p,
                          int m, int d, bool closed );

    static void   fillStandardVBO( GL::VertexBufferObject& vbo,
                                   const DVector< DVector< Vector<T, n> > >& p,
                                   int& no_vertices,
                                   int d = 0 );

  }; // END class PCurveVisualizer

} // END namespace GMlib

// Include PCurveVisualizer class function implementations
#include "gmpcurvevisualizer.c"


#endif // GM_PARAMETRICS_VISUALIZERS_PCURVEVISUALIZER_H

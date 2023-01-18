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




#include "gmpcurvevisualizer.h"

// gmlib
#include <opengl/gmopengl.h>

namespace GMlib {

  template <typename T, int n>
  PCurveVisualizer<T,n>::PCurveVisualizer() {}

  template <typename T, int n>
  PCurveVisualizer<T,n>::~PCurveVisualizer() {}

  template <typename T, int n>
  void PCurveVisualizer<T,n>::fillStandardVBO( GL::VertexBufferObject &vbo,
                                               const DVector< DVector< Vector<T, n> > >& p,
                                               int& no_vertices,
                                               int d) {

    no_vertices = p.getDim();
    GLsizeiptr buffer_size = no_vertices * sizeof(GL::GLVertex);

    vbo.bufferData( buffer_size, 0x0, GL_STATIC_DRAW );

    GL::GLVertex *ptr = vbo.mapBuffer<GL::GLVertex>();
    if( ptr ) {
      for( int i = 0; i < p.getDim(); i++ ) {

        ptr->x = p(i)(d)(0);
        ptr->y = p(i)(d)(1);
        ptr->z = p(i)(d)(2);
        ptr++;
      }
    }
    vbo.unmapBuffer();
  }

  template <typename T, int n>
  inline
  void PCurveVisualizer<T,n>::replot( const DVector< DVector< Vector<T, n> > >& /*p*/,
                                      int /*m*/, int /*d*/, bool /*closed*/ ) {}



} // END namespace GMlib

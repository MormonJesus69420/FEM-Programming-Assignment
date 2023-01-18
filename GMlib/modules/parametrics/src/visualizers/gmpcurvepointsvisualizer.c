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




#include "gmpcurvepointsvisualizer.h"


namespace GMlib {

  template <typename T, int n>
  PCurvePointsVisualizer<T,n>::PCurvePointsVisualizer()
    : _no_vertices{0}, _size{T(1)}, _color{GMcolor::blueViolet()} {

    _prog.acquire("color");
    _vbo.create();
  }

  template <typename T, int n>
  PCurvePointsVisualizer<T,n>::~PCurvePointsVisualizer() {}

  template <typename T, int n>
  inline
  void PCurvePointsVisualizer<T,n>::render(const SceneObject* obj, const DefaultRenderer* renderer) const {

    const HqMatrix<float,3> &mvpmat = obj->getModelViewProjectionMatrix(renderer->getCamera());

    GL_CHECK(::glPointSize( _size ));

    _prog.bind(); {

      // Model view and projection matrices
      _prog.uniform( "u_mvpmat", mvpmat );
      _prog.uniform( "u_color", _color );

      // Vertex attribute location
      GL::AttributeLocation vert_loc = _prog.getAttributeLocation( "in_vertex" );

      // Bind and draw
      _vbo.bind();
      _vbo.enable( vert_loc, 3, GL_FLOAT, GL_FALSE, sizeof(GL::GLVertex), static_cast<const GLvoid*>(0x0) );
      GL_CHECK(::glDrawArrays( GL_POINTS, 0, _no_vertices ));
      _vbo.disable( vert_loc );
      _vbo.unbind();

    } _prog.unbind();
  }

  template <typename T, int n>
  const Color& PCurvePointsVisualizer<T,n>::getColor() const {

    return _color;
  }

  template <typename T, int n>
  float PCurvePointsVisualizer<T,n>::getSize() const {

    return _size;
  }

  template <typename T, int n>
  inline
  void PCurvePointsVisualizer<T,n>::replot( const DVector< DVector< Vector<T, n> > >& p,
                                             int /*m*/, int /*d*/, bool /*closed*/ ) {

    this->fillStandardVBO( _vbo, p, _no_vertices );
  }

  template <typename T, int n>
  void PCurvePointsVisualizer<T,n>::setColor( const Color& color ) {

    _color = color;
  }

  template <typename T, int n>
  void PCurvePointsVisualizer<T,n>::setSize( float size ) {

    _size = size;
  }

} // END namespace GMlib

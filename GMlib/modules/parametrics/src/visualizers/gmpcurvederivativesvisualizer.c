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





// include for syntax highlighting ^^
#include "gmpcurvederivativesvisualizer.h"


namespace GMlib {

  template <typename T, int n>
  PCurveDerivativesVisualizer<T,n>::PCurveDerivativesVisualizer() {

    _prog.acquire("color");
    _vbo.create();

    _color = GMcolor::green();
    _t = 1;
    _mode = GM_PCURVE_DERIVATIVESVISUALIZER_RELATIVE;
    _size = 1.0;
  }

  template <typename T, int n>
  PCurveDerivativesVisualizer<T,n>::~PCurveDerivativesVisualizer() { }

  template <typename T, int n>
  inline
  void PCurveDerivativesVisualizer<T,n>::render(const SceneObject *obj, const DefaultRenderer* renderer) const {

    const HqMatrix<float,3> &mvpmat = obj->getModelViewProjectionMatrix(renderer->getCamera());

    _prog.bind(); {

      _prog.uniform( "u_mvpmat", mvpmat );
      _prog.uniform( "u_color", _color );

      // Vertex attribute location
      GL::AttributeLocation vert_loc = _prog.getAttributeLocation( "in_vertex" );

      // Bind & draw
      _vbo.bind();
      _vbo.enable(vert_loc, 3, GL_FLOAT, GL_FALSE, 0, static_cast<const GLvoid*>(0x0) );
      GL_CHECK(glDrawArrays( GL_LINES, 0, _no_elements ));
      _vbo.disable(vert_loc);

    } _prog.unbind();
  }

  template <typename T, int n>
  const Color& PCurveDerivativesVisualizer<T,n>::getColor() const {

    return _color;
  }

  template <typename T, int n>
  int PCurveDerivativesVisualizer<T,n>::getDerivative() const {

    return _t;
  }

  template <typename T, int n>
  GM_PCURVE_DERIVATIVESVISUALIZER_SIZE PCurveDerivativesVisualizer<T,n>::getMode() const {

    return _mode;
  }

  template <typename T, int n>
  double PCurveDerivativesVisualizer<T,n>::getSize() const {

    return _size;
  }

  template <typename T, int n>
  inline
  void PCurveDerivativesVisualizer<T,n>::replot(
    const DVector< DVector< Vector<T, n> > >& p,
    int /*m*/, int /*d*/, bool /*closed*/
  ) {

    int der = 0;
    if( (_t >= 0) || (_t <= p(0).getDim()-1) )
      der = _t;


    int no_derivatives = p.getDim();
    _no_elements = no_derivatives * 2;

    _vbo.bufferData(no_derivatives * 2 * 3 * sizeof(float), 0x0, GL_DYNAMIC_DRAW);

    float *ptr = _vbo.mapBuffer<float>();
    if( ptr ) {

      switch( _mode ) {
      case GM_PCURVE_DERIVATIVESVISUALIZER_RELATIVE: {

          for( int i = 0; i < p.getDim(); i++ ) {

            const Vector<T,3> &pos =p(i)(0);
            for( int j = 0; j < 3; j++ )
              *(ptr++) = pos(j);

            const Vector<T,3> v = p(i)(der) * _size;
            for( int j = 0; j < 3; j++ )
              *(ptr++) = pos(j) + v(j);
          }
        }
        break;

      case GM_PCURVE_DERIVATIVESVISUALIZER_ABSOLUTE: {

          for( int i = 0; i < p.getDim(); i++ ) {

            const Vector<T,3> &pos =p(i)(0);
            for( int j = 0; j < 3; j++ )
              *(ptr++) = pos(j);

            const Vector<T,3> v = p(i)(der).getNormalized() * _size;
            for( int j = 0; j < 3; j++ )
              *(ptr++) = pos(j) + v(j);
          }
        }
        break;

      case GM_PCURVE_DERIVATIVESVISUALIZER_NORMALIZED:
      default: {

          for( int i = 0; i < p.getDim(); i++ ) {

            const Vector<T,3> &pos =p(i)(0);
            for( int j = 0; j < 3; j++ )
              *(ptr++) = pos(j);

            const UnitVector<T,3> uv = p(i)(der);
            for( int j = 0; j < 3; j++ )
              *(ptr++) = pos(j) + uv(j);
          }
        }
        break;
      }
    }

    _vbo.unmapBuffer();
  }

  template <typename T, int n>
  void PCurveDerivativesVisualizer<T,n>::setColor( const Color& color ) {

    _color = color;
  }

  template <typename T, int n>
  void PCurveDerivativesVisualizer<T,n>::setDerivative( int t ) {

    if( _t > 0 )
      _t = t;
  }

  template <typename T, int n>
  void PCurveDerivativesVisualizer<T,n>::setMode( GM_PCURVE_DERIVATIVESVISUALIZER_SIZE mode ) {

    _mode = mode;
  }

  template <typename T, int n>
  void PCurveDerivativesVisualizer<T,n>::setSize( double size ) {

    _size = size;
  }


} // END namespace GMlib

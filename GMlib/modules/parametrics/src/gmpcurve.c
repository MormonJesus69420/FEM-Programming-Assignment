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





// gmlib
#include "visualizers/gmpcurvedefaultvisualizer.h"
#include <core/utils/gmdivideddifferences.h>

// stl
#include <cmath>

namespace GMlib {


  template <typename T, int n>
  inline
  PCurve<T,n>::PCurve( int s ) {

    _no_sam           = s;
    _no_der           = 1;
    _d                = -1;
    _tr               = T(0);
    _sc               = T(1);

    setNoDer(2);
    this->_lighted      = false;
    _default_visualizer = 0x0;
  }


  template <typename T, int n>
  inline
  PCurve<T,n>::PCurve( const PCurve<T,n>& copy ) : Parametrics<T,1,n>( copy ) {

    _no_sam       = copy._no_sam;
    _no_der       = copy._no_der;
    _p            = copy._p;
    _t            = copy._t;
    _d            = copy._d;
    _tr           = copy._tr;
    _sc           = copy._sc;

    setNoDer(2);
    _default_visualizer = 0x0;
  }

  template <typename T, int n>
  PCurve<T,n>::~PCurve() {

    enableDefaultVisualizer( false );
    if( _default_visualizer )
      delete _default_visualizer;
  }



  template <typename T, int n>
  inline
  void	PCurve<T,n>::_eval( T t, int d ) const {

    if( d <= _d && t == _t ) return;

    _t = t; _d = d;
    eval( shift(t), d );
  }

  template <typename T, int n>
  inline
  T PCurve<T,n>::_integral(T a, T b, double eps) const {

    T t = b - a;
    T sum = (getSpeed(a)+getSpeed(b))/2;

    T mat[16][16];
    mat[0][0] = sum * t;

    T s;
    int i,j,k;
    for( i = 1; i < 16; i++ ) {
      s = T(0);
      k = 1 << i;
      t /= 2.0;
      for( j = 1; j < k; j += 2 )
        s += getSpeed(a + t*j);
      mat[0][i] = t * (sum += s);
      for (j=1; j<=i; j++) {
        b = 1 << (j << 1);
        mat[j][i-j] = (b*mat[j-1][i-j+1] - mat[j-1][i-j])/(b-1.0);
      }
      if(std::abs(mat[i][0] - mat[i-1][0]) < eps) return mat[i][0];
    }
    return mat[15][0];
  }

  template <typename T, int n>
  void PCurve<T,n>::enableDefaultVisualizer( bool enable ) {

    if( !enable )
      removeVisualizer( _default_visualizer );
    else {
      if( !_default_visualizer )
        _default_visualizer = new PCurveDefaultVisualizer<T,n>();

      insertVisualizer( _default_visualizer );
    }
  }


  template <typename T, int n>
  DVector<Vector<T,n> >& PCurve<T,n>::evaluate( T t, int d ) const {

    _eval(t,d);
    return _p;
  }


  template <typename T, int n>
  DVector<Vector<T,n> >& PCurve<T,n>::evaluateGlobal( T t, int d ) const {

    static DVector< Vector<T,n> > p;

    _eval(t,d);
    p.setDim(_p.getDim());

    p[0] = this->_present* static_cast< Point<T,n> >(_p[0]);

    for( int i = 1; i <= d; i++ )
      p[i] = this->_present * _p[i];

    return p;
  }


  template <typename T, int n>
  DVector<Vector<T,n> >& PCurve<T,n>::evaluateParent( T t, int d ) const {

    static DVector< Vector<T,n> > p;

    _eval(t,d);
    p.setDim(_p.getDim());

    p[0] = this->_matrix * static_cast< Point<T,n> >(_p[0]);

    for( int i = 1; i <= d; i++ )
      p[i] = this->_matrix * _p[i];

    return p;
  }

  /*!
   * Using a 2nd degree taylor expansion
   * <c-p + dt*c' + .5dt^2*c'' , c' + dt*c''> = 0
   */
  template<typename T, int n>
  bool PCurve<T,n>::getClosestPoint(const Point<T,n>& q, T& t, Point<T,n>& p, double eps, int max_iterations) const {

    DVector<Vector<T,n> > c;
    for (int i=0; i < max_iterations; i++) {
      c = evaluate(t, 2);
      T dt = -( (c[0] - q) * c[1]) / ( (c[0] - q) * c[2] + c[1] * c[1] );

      p = c[0];
      t += dt;

      if (std::abs(dt) <= eps) return true;
    }

    return false;
  }

  template <typename T, int n>
  inline
  T PCurve<T,n>::getCurvature( T t ) const {

    _eval( t, 2 );
    Vector<T,n> d1 = _p[1];
    T a1= d1.getLength();

    if( a1 < T(1.0e-5) ) return T(0);

    return (d1^_p[2]).getLength() / pow(a1,3);
  }


  template <typename T, int n>
  inline
  T PCurve<T,n>::getCurveLength( T a , T b ) const {

    if(b<a)	{

      a = getParStart();
      b = getParEnd();
    }

    return _integral( a, b, 1e-10 );
  }


  template <typename T, int n>
  inline
  int PCurve<T,n>::getDerivatives() const {

    return _no_der;
  }


  template <typename T, int n>
  inline
  Vector<T,n> PCurve<T,n>::getDer1( T t ) const {

    _eval( t, 1 );
    return _p[1];
  }


  template <typename T, int n>
  inline
  Vector<T,n> PCurve<T,n>::getDer2( T t ) const {

    _eval(t,2);
    return _p[2];
  }


  template <typename T, int n>
  inline
  Vector<T,n> PCurve<T,n>::getDer3( T t ) const {

    _eval(t,3);
    return _p[3];
  }


  template <typename T, int n>
  inline
  T PCurve<T,n>::getParDelta() const {

    return _sc*( getEndP() - getStartP() );
  }


  template <typename T, int n>
  inline
  T PCurve<T,n>::getParEnd() const {

    return getParStart() + getParDelta();
  }


  template <typename T, int n>
  inline
  T PCurve<T,n>::getParStart() const {

    return getStartP() + _tr;
  }


  template <typename T, int n>
  inline
  T PCurve<T,n>::getRadius( T t ) const {

    return T(1) / getCurvature( t );
  }


  template <typename T, int n>
  inline
  int PCurve<T,n>::getSamples() const {

    return _no_sam;
  }


  template <typename T, int n>
  inline
  T PCurve<T,n>::getSpeed( T t ) const {

    return getDer1( t ).getLength();
  }

  template <typename T, int n>
  inline
  void PCurve<T,n>::insertVisualizer( Visualizer* visualizer ) {

    Parametrics<T,1,n>::insertVisualizer( visualizer );

    PCurveVisualizer<T,n> *visu = dynamic_cast<PCurveVisualizer<T,n>*>( visualizer );
    if( !visu )
      return;

    if( _pcurve_visualizers.exist( visu ) )
      return;

    _pcurve_visualizers += visu;
  }


  template <typename T, int n>
  inline
  bool PCurve<T,n>::isClosed() const {

    return false;
  }


  template <typename T, int n>
  inline
  void PCurve<T,n>::preSample( int /*m*/, int /*d*/, T /*s*/, T /*e*/ ) {}


  template <typename T, int n>
  void PCurve<T,n>::replot( int m, int d ) {


    // Correct sample domain
    if( m < 2 )       m = _no_sam;
    else        _no_sam = m;

    // Correct derivatives
    if( d < 1 )       d = _no_der;
    else        _no_der = d;


    // pre-sampel / pre evaluate data for a given parametric curve, if wanted/needed
    preSample( m, d, getStartP(), getEndP() );

    // Resample
    DVector< DVector< Vector<T,n> > > p;
    resample( p, m, d, getStartP(), getEndP() );

    // Set The Surrounding Sphere
    setSurroundingSphere( p );


    // Replot Visaulizers
    for( int i = 0; i < this->_pcurve_visualizers.getSize(); i++ )
      this->_pcurve_visualizers[i]->replot( p, m, d, isClosed() );
  }

  template <typename T, int n>
  void PCurve<T,n>::removeVisualizer( Visualizer* visualizer ) {

    PCurveVisualizer<T,n> *visu = dynamic_cast<PCurveVisualizer<T,n>*>( visualizer );
    if( visu )
      _pcurve_visualizers.remove( visu );

    Parametrics<T,1,n>::removeVisualizer( visu );
  }




//  template <typename T, int n>
//  inline
//  void PCurve<T,n>::resample( Array< Point<T,n> >& a, T eps ) {
//
//    _sam = 0; _s_sam = 0; _e_sam = 1;
//    T t, ddt, dp5 = getParDelta()/5;
//    bool go_on=true;
//
//    for( t = getParStart(); go_on; t += ddt ) {
//
//      eval( t, 2 );
//      a += _p[0];
//      if ( a.getSize() > 7000 )
//        break;
//
//      T _r = _p[1] * _p[1];
//      Vector<T,n> r = _p[2]-( (_p[2] * _p[1] ) / _r) * _p[1];
//      _r = sqrt(double(_r));
//      ddt = 2 * _r / _p[1].getLength() * acos( ( _r - eps ) / _r);
//
//      if( ddt > dp5)
//        ddt = dp5;
//
//      if( ddt > getParEnd() - t ) {
//
//        T oddt = ddt;
//        ddt = getParEnd() - t;
//        go_on = false;
//
//        if( ddt < oddt / 5 )
//          a.removeIndex( a.getSize() - 1 );
//      }
//    }
//    eval( t, 0 );
//    a += _p[0];
//  }
//
//
//  template <typename T, int n>
//  inline
//  void PCurve<T,n>::resample( Array<Point<T,n> >& a, int m ) {
//
//    _sam = m; _s_sam = 0; _e_sam = 1;
//    T du = getParDelta() / ( m - 1 );
//    a.setSize( m );
//
//    for( int i = 0; i < m; i++ ) {
//
//      eval( getParStart() + i * du, 0 );
//      a[i] = _p[0];
//    }
//
//  }
//
//
//  template <typename T, int n>
//  inline
//  void PCurve<T,n>::resample( Array<Point<T,n> >& a, int m, T start, T end ) {
//
//    _sam = m; _s_sam = start; _e_sam = end;
//    T du = getParDelta();
//    T st = getParStart() + start * du;
//    T et = getParStart() + end * du;
//    du = ( et - st ) / ( m - 1 );
//    a.setSize( m );
//
//    for(int i=0; i<m; i++) {
//
//      eval( st + i * du, 0 );
//      a[i] = _p[0];
//    }
//  }

  template <typename T, int n>
  inline
  void PCurve<T,n>::resample( DVector< DVector< Vector<T,n> > >& p, int m, int d ) {

    resample( p, m, d, getStartP(), getEndP() );
  }


  template <typename T, int n>
  inline
  void PCurve<T,n>::resample( DVector< DVector< Vector<T,n> > >& p, int m, int d, T start, T end ) {

    T du = (end-start)/(m-1);
    p.setDim(m);

    for( int i = 0; i < m - 1; i++ ) {
      eval( start + i * du, d, true);
      p[i] = _p;
    }
    eval( end, d, false );
    p[m-1] = _p;

    switch( this->_dm ) {
      case GM_DERIVATION_EXPLICIT:
        // Do nothing, evaluator algorithms for explicite calculation of derivatives
        // should be defined in the eval( ... ) function enclosed by
        // if( this->_derivation_method == this->EXPLICIT ) { ... eval algorithms for derivatives ... }
        break;
      case GM_DERIVATION_DD:
      default:
        DD::compute1D(p, du, isClosed(), d );
        break;
    }
  }


  template <typename T, int n>
  inline
  void PCurve<T,n>::setDomain(T start, T end) {

    _sc = ( end - start );
    _tr= getStartP() + start;
  }


  template <typename T, int n>
  inline
  void PCurve<T,n>::setDomainScale( T sc ) {

    _sc = sc;
  }


  template <typename T, int n>
  inline
  void PCurve<T,n>::setDomainTrans( T tr ) {

    _tr = tr;
  }


  template <typename T, int n>
  inline
  void PCurve<T,n>::setNoDer( int d ) {

    _defalt_d = d;
  }


  template <typename T, int n>
  inline
  void PCurve<T,n>::setSurroundingSphere( const DVector< DVector< Vector<T,n> > >& p ) {

    Sphere<T,n>  s;
    s.resetPos( p(0)(0) );
    s += p(p.getDim() - 1)(0);
    for( int i = p.getDim() - 2; i > 0; i-- )
      s += p(i)(0);
    Parametrics<T,1,n>::setSurroundingSphere( s.template toType<float>() );
  }


  template <typename T, int n>
  inline
  T PCurve<T,n>::shift( T t ) const {
    return t;
//    return _tr + _sc * ( t - getStartP() );
  }

  template <typename T, int n>
  void PCurve<T,n>::toggleDefaultVisualizer() {

    if( !_pcurve_visualizers.exist( _default_visualizer ) )
      enableDefaultVisualizer( true );
    else
      enableDefaultVisualizer( false );
  }


  template <typename T, int n>
  inline
  Point<T,n> PCurve<T,n>::operator()( T t )	{

    eval( t, _defalt_d );
    return Point<T,n>(_p[0]);
  }

  template <typename T, int n>
  inline
  const PCurveVisualizer<T, n>* PCurve<T,n>::getDefaultVisualizer() const {

    return _default_visualizer;
  }


} // END namespace GMlib

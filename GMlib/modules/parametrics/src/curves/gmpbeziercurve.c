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



#include "../evaluators/gmevaluatorstatic.h"

// gmlib
#include <scene/visualizers/gmselectorgridvisualizer.h>

namespace GMlib {


  template <typename T>
  inline
  PBezierCurve<T>::PBezierCurve( const DVector< Vector<T, 3> >& c ) {

    init();

    // Set Control Points
    setControlPoints( c );
  }


  template <typename T>
  inline
  PBezierCurve<T>::PBezierCurve( const DVector< Vector<T, 3> >& c, T s, T t, T e ) {


    init();

    // Generate the control points
    DMatrix<T> bhp;
    EvaluatorStatic<T>::evaluateBhp( bhp, c.getDim()-1, (t-s)/(e-s), T(1)/(e-s) );
    bhp.invert();
    _c = bhp * c;

    for( int i = 0; i < c.getDim(); i++ )
      _c[i] -= c(0);
    this->translateParent( c(0) );
  }


  template <typename T>
  inline
  PBezierCurve<T>::PBezierCurve( const PBezierCurve<T>& copy ) : PCurve<T,3>( copy ) {

    init();
    _c = copy._c;
  }


  template <typename T>
  PBezierCurve<T>::~PBezierCurve() {

    if(_sgv) delete _sgv;
  }


  template <typename T>
  inline
  DVector< Vector<T,3> >& PBezierCurve<T>::getControlPoints() {

    return _c;
  }


  template <typename T>
  inline
  int PBezierCurve<T>::getDegree() const {

    return _c.getDim() - 1;
  }


  template <typename T>
  void PBezierCurve<T>::edit( int /*selector_id*/ ) {

    _c_moved = true;

    if( this->_parent )
      this->_parent->edit( this );

    this->replot();
    if(_sgv) _sgv->update();

    _c_moved = false;
  }


  template <typename T>
  inline
  void PBezierCurve<T>::eval( T t, int /*d*/, bool /*l*/ ) const {

    // Send the control to the pre-eval evaluator
//    if( _resamp_mode == GM_RESAMPLE_PREEVAL ) {
//
//      evalPre( t, d, l );
//      return;
//    }

    // Compute the Bernstein-Hermite Polynomials
    DMatrix< T > bhp;
    EvaluatorStatic<T>::evaluateBhp( bhp, getDegree(), t, _scale );

    this->_p = bhp * _c;
  }


  template <typename T>
  inline
  void PBezierCurve<T>::evalPre( T t, int /*d*/, bool /*l*/ ) {

    // Compute the Bernstein-Hermite Polynomials
//    DMatrix< T > bhp;
//    EvaluatorStatic<T>::evaluateBhp( bhp, getDegree(), t, _scale );

    int it = 0;
    findIndex( t, it );

    this->_p = _t[it] * _c;
  }

  template <typename T>
  inline
  void PBezierCurve<T>::findIndex( T t, int& it ) {

    it = (this->_no_samp-1)*(t-this->getParStart())/(this->getParDelta())+0.1;
  }


  template <typename T>
  T PBezierCurve<T>::getStartP() const {
    return T(0);
  }


  template <typename T>
  T PBezierCurve<T>::getEndP() const {
    return T(1);
  }

  template <typename T>
  inline
  void PBezierCurve<T>::hideSelectors() {

    if( !_selectors )
      return;

    // Remove Selector Grid
    if(_sgv) {
      this->removeVisualizer( _sgv );
      _sgv->reset();
    }

    // Remove Selectors
    for( int i = 0; i < _s.getDim(); i++ ) {
      this->remove( _s[i] );
      delete _s[i];
    }

    _selectors = false;
  }


  template <typename T>
  void PBezierCurve<T>::init() {

    this->_type_id = GM_SO_TYPE_CURVE_BEZIER;

    _selectors = false;
    _c_moved = false;

    _scale = T(1);
    _closed = false;
    _pre_eval = true;
    _resamp_mode = GM_RESAMPLE_PREEVAL;

    _sgv = 0x0;
  }


  template <typename T>
  inline
  bool PBezierCurve<T>::isClosed() const {

    return _closed;
  }


  template <typename T>
  bool PBezierCurve<T>::isSelectorsVisible() const {

    return _selectors;
  }


  template <typename T>
  inline
  void PBezierCurve<T>::preSample( int m, int /*d*/, T start, T end ) {

    // break out of the preSample function if no preevalution is to be used
    switch( _resamp_mode ) {
    case GM_RESAMPLE_PREEVAL: break;
    case GM_RESAMPLE_INLINE:
    default:
      return;
    }

    // Check whether to redo the preEvaluation
    if( !_pre_eval && m == _t.getDim() )
      return;

    // dt; sample step value
    const T dt = (end-start) / T(m-1);

    // Set the dimension of the Bernstein-Hermite Polynomial DVector
    _t.setDim(m);

    // For each sample point on the uniform curve calculate the Bernstein-Hermite Polynomials
    for( int i = 0; i < m; i++ )
      EvaluatorStatic<T>::evaluateBhp( _t[i], getDegree(), i*dt, _scale );

    // Disable the pre-evaluation step
    _pre_eval = false;
  }


  template <typename T>
  inline
  void PBezierCurve<T>::setClosed( bool state ) {

    _closed = state;
  }

  template <typename T>
  inline
  void PBezierCurve<T>::setControlPoints( const DVector< Vector<T,3> >& cp ) {


    if( _c.getDim() == cp.getDim() ) {

      bool no_change = true;
      for( int i = 0; i < cp.getDim(); i++ )
        if( _c[i] != cp(i) )
          no_change = false;

      if( no_change )
        return;
    }
    else {

      _pre_eval = true;
    }

    _c = cp;
  }


  template <typename T>
  inline
  void PBezierCurve<T>::setResampleMode( GM_RESAMPLE_MODE mode ) {

    _resamp_mode = mode;
  }


  template <typename T>
  inline
  void PBezierCurve<T>::setScale( T d ) {

    if( d == _scale )
      return;

    _scale = d;
    _pre_eval = true;
  }


  template <typename T>
  inline
  void PBezierCurve<T>::showSelectors( bool grid, const Color& s_color, const Color& /*grid_color*/ ) {

    if( _selectors )
      return;

    _s.setDim( _c.getDim() );
    for( int i = 0, k = 0; i < _c.getDim(); i++ ) {
      Selector<T,3> *sel = new Selector<T,3>( _c[i], k++, this, T(0.3), s_color );
      this->insert( sel );
      _s[i] = sel;
    }


    if( grid ) {
      if(!_sgv) _sgv = new SelectorGridVisualizer<T>;
      _sgv->setSelectors( _c );
      this->insertVisualizer( _sgv );
    }

    _selectors = true;
  }


  template <typename T>
  inline
  void PBezierCurve<T>::updateCoeffs( const Vector<T,3>& d ) {

    if( _c_moved ) {

      HqMatrix<T,3> invmat = this->_matrix;
      invmat.invertOrthoNormal();

      Vector<T,3> diff = invmat*d;
      for( int i = 0; i < _c.getDim(); i++ ) {

        _c[i] += diff;
        _s[i]->translateParent( diff );
      }
      this->translateParent( -d, false );
      this->replot();
    }
  }


} // END namespace GMlib


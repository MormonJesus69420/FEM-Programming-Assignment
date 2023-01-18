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





#include "gmperbscurve.h"


#include "gmparc.h"
#include "gmpbeziercurve.h"
#include "gmpsubcurve.h"


namespace GMlib {

  // Constructors and destructors
  //******************************

  template <typename T>
  inline
  PERBSCurve<T>::PERBSCurve()
  {
    this->_type_id = GM_SO_TYPE_CURVE_ERBS;
    init();
  }


  template <typename T>
  inline
  PERBSCurve<T>::PERBSCurve( CURVE_TYPE type, PCurve<T,3>* g, int n, int d)
  {
    this->_type_id = GM_SO_TYPE_CURVE_ERBS;
    init();

    _closed = g->isClosed();
    if( _closed ) n++;

    generateKnotVector(g, n);

    // Make local curves.
    _c.setDim(n);
    for( int i = 1; i < n; i++ ) {
      _c[i-1] = makeLocal(type, g, _t[i-1], _t[i], _t[i+1], d);
      insertLocal( _c[i-1] );
    }

    // Handle Open/Closed
    if( _closed )
      _c[n-1] = _c[0];
    else {
      _c[n-1] = makeLocal(type, g, _t[n-1], _t[n], _t[n+1], d);
      insertLocal( _c[n-1] );
    }
  }


  template <typename T>
  inline
  PERBSCurve<T>::PERBSCurve( const PERBSCurve<T>& copy ) : PCurve<T,3>( copy )
  {
    init();

    _closed = copy._closed;
    _t = copy._t;

    // sync local patches
    const DVector< PCurve<T,3>* > &c = copy._c;
    _c.setDim( c.getDim() );
    Array< unsigned int > cl;
    Array< int > cli;

    for( int i = 0; i < c.getDim(); ++i ) {

      cli += i;
      cl  += c(i)->getName();
    }


    // Find and save the reference to the local patches
    for( int i = 0; i < this->getChildren().getSize(); ++i ) {

      SceneObject *child = this->getChildren()[i];
      for( int j = cl.getSize() - 1; j >= 0; --j ) {

        if( cl[j] == child->_copy_of->getName() ) {

          int idx = cli[j];

          _c[idx] = static_cast<PCurve<T,3>*>(child);

          cl.removeIndex(j);
          cli.removeIndex(j);
        }
      }
    }
  }


  template <typename T>
  PERBSCurve<T>::~PERBSCurve()
  {
    for( int i = 0; i < _c.getDim(); i++ )
    {
      SceneObject::remove( _c[i] );
      delete _c[i];
    }

    if( _evaluator )
      delete _evaluator;
  }

  template <typename T>
  inline
  void PERBSCurve<T>::edit( SceneObject *obj )
  {
    int i;
    for( i = 0; i < _c.getDim(); i++ )
      if( _c[i] == obj )
        goto edit_loop_break;

    edit_loop_break:

    // If Bezier Patch
    PBezierCurve<T> *bezier = dynamic_cast<PBezierCurve<T>*>(_c[i]);
    if( bezier )
      bezier->updateCoeffs( _c[i]->getPos() - _c[i]->evaluateParent(_t[i],0)[0]);

    replot();
  }


  template <typename T>
  void PERBSCurve<T>::eval( T t, int d, bool l ) const
  {

    int k;
    if( _resamp_mode == GM_RESAMPLE_PREEVAL ){

      // Find knot
      k = findIndex(t);
    }
    else {

      // Find knot
      for( k = 1; k < _t.getDim()-2; ++k ) if( t < _t[k+1] ) break;

      // If right-evaluation, find first knot
      if(!l) while( std::abs( _t[k] - _t[k-1] ) < 1e-5 ) --k;
    }

    // Evaluating first Local Curve @ (t-_t[k-1])/(_t[k+1]-_t[k-1])
    DVector< Vector<T,3> > c0 = _c[k-1]->evaluateParent( mapToLocal(t,k), d);

    // If t == _t[k], the sample is at the knot, set the values to the values of the first local curve.
    if( std::abs(t - _t[k]) < 1e-5 ) { this->_p = c0; return; }


    // Evaluating second Local Curve @ (t-_t[k])/(_t[k+2]-_t[k)
    DVector< Vector<T,3> > c1 = _c[k]->evaluateParent( mapToLocal(t,k+1), d);


    // Blend c0 and c1
    if( _resamp_mode == GM_RESAMPLE_PREEVAL )
      compBlend( d, _B[k], c0, c1 );
    else {

      DVector<T> B;
      getB(B, k, t, d );
      compBlend( d, B, c0, c1 );
    }
  }

  template <typename T>
  void PERBSCurve<T>::compBlend(int d, const DVector<T>& B, DVector<Vector<T,3> >& c0, DVector<Vector<T,3> >& c1) const {

    c0 -= c1;

    DVector<T> a(d+1);
    for( int i = 0; i < B.getDim(); i++ )
    {
      // Compute the pascal triangle numbers
      a[i] = 1;
      for( int j = i-1; j > 0; j-- )
        a[j] += a[j-1];
      // Compute the sample position data
      for( int j = 0; j <= i; j++ )
        c1[i] += (a[j] * B(j)) * c0[i-j];
    }
    this->_p = c1;
  }


  template <typename T>
  inline
  int PERBSCurve<T>::findIndex( T t) const
  {
    return (this->_no_sam-1)*(t-this->getParStart())/(this->getParDelta())+0.1;
  }


  template <typename T>
  inline
  void PERBSCurve<T>::getB( DVector<T>& B, int k, T t, int d ) const {

    B.setDim(d+1);
    _evaluator->set( _t[k], _t[k+1] - _t[k] );
    B[0] = 1 - (*_evaluator)(t);
    switch(d) {
      case 3: B[3] = T(0);
      case 2: B[2] = - _evaluator->getDer2();
      case 1: B[1] = - _evaluator->getDer1();
    }
  }


  template <typename T>
  T PERBSCurve<T>::getEndP() const {
    return _t(_t.getDim()-2);
  }


  template <typename T>
  inline
  DVector< PCurve<T,3>* >& PERBSCurve<T>::getLocalCurves() {

    return _c;
  }


  template <typename T>
  inline
  const DVector< PCurve<T,3>* >& PERBSCurve<T>::getLocalCurves() const {

    return _c;
  }


  template <typename T>
  inline
  int PERBSCurve<T>::getNoLocalCurves() const {

    return _c.getDim();
  }


  template <typename T>
  T PERBSCurve<T>::getStartP() const {
    return _t(1);
  }


  template <typename T>
  inline
  void PERBSCurve<T>::hideLocalCurves() {

    for( int i = 0; i < _c.getDim(); i++ )
      _c[i]->setVisible( false, -1 );
  }

  template <typename T>
  inline
  void PERBSCurve<T>::showLocalCurves() {

    for( int i = 0; i < _c.getDim(); i++ ) {

      if( !_c[i]->getDefaultVisualizer() ) {

        _c[i]->enableDefaultVisualizer();
        _c[i]->replot(10);
      }

      _c[i]->setVisible( true, -1 );
    }
  }


  template <typename T>
  void PERBSCurve<T>::init()
  {

    _no_sam           = 20;
    _no_der           = 1;

    _evaluator = new ERBSEvaluator<long double>();

    _resamp_mode = GM_RESAMPLE_PREEVAL;
    _pre_eval    = true;

//    _resamp_mode = GM_RESAMPLE_INLINE;
//    _pre_eval = false;
//    _B.setDim(1);
  }


  template <typename T>
  inline
  void PERBSCurve<T>::insertLocal( PCurve<T,3>* local ) {

    local->replot( 30 );
    static Color cl= GMcolor::blue();
    local->setColor( cl );
    cl = cl.getInterpolatedHSV( 0.2, GMcolor::yellow() );
    local->setVisible( false );
    local->setCollapsed( true );
    this->insert( local );
  }


  template <typename T>
  inline
  bool PERBSCurve<T>::isClosed() const {

    return _closed;
  }


  template <typename T>
  inline
  bool PERBSCurve<T>::isLocalCurvesVisible() const {

    for( int i = 0; i < _c.getDim(); i++ )
      if( _c(i)->isVisible() ) return true;
    return false;
  }



  template <typename T>
  inline
  void PERBSCurve<T>::preSample( int m, int d, T start, T end ) {

    // break out of the preSample function if no preevalution is to be used
    switch( _resamp_mode ) {
    case GM_RESAMPLE_PREEVAL:   break;
    case GM_RESAMPLE_INLINE :
    default:                    return;
    }

    // break out and return if preevaluation isn't necessary.
    if( !_pre_eval && m == _tk.getDim() )   return;


    // Set dimension for B and index value tables.
    _B.setDim(m);
    _tk.setDim(m);

    int     tk = 1;
    const T dt = ( end - start ) / T(m-1);  // sample step value

    for( int i = 0; i < m; i++ )
    {
      const T t = getStartP() + T(i) * dt;  // Compute the "current" t value
      // Calculate knot-index of sample nr. i
      for( ; tk < _t.getDim()-2; tk++ ) if(t < _t[tk+1]) break;
      _tk[i] = tk;

      // Find the complementary B-Vector coherent with the current index.
      if( !(std::abs(t - _t[tk]) < 1e-5) )
        getB( _B[i], tk, t, d );
    }

    _pre_eval = false;
  }


  template <typename T>
  inline
  void PERBSCurve<T>::setResampleMode( GM_RESAMPLE_MODE mode ) {

    _resamp_mode = mode;
  }


  template <typename T>
  inline
  void PERBSCurve<T>::toggleLocalCurves() {

    for( int i = 0; i < _c.getDim(); i++ )
      _c[i]->toggleVisible();
  }


  template <typename T>
  inline
  PCurve<T,3>* PERBSCurve<T>::makeLocal(CURVE_TYPE type, PCurve<T,3>* g, T s, T t, T e, int d) {

    switch(type) {
    case SUB_CURVE:
      return new PSubCurve<T>( g, s, e, t );
    case ARC_CURVE:
      return new PArc<T>(g->evaluateParent(t, 2), s, t, e);
    case BEZIERCURVE:
      return new PBezierCurve<T>(g->evaluateParent(t, d), s, t, e);
    }

    return NULL;
  }

  template <typename T>
  inline
  T PERBSCurve<T>::mapToLocal(T t, int tk) const {

//    return getParStart() + (t-s)/(e-s) * getParDelta();

    PCurve<T,3> *c = _c(tk-1);

    const T cs = c->getParStart();
    const T cd = c->getParDelta();

    // Fill in when support for multiple knots has been implemented and supported.
    // See PERBSSurf

    return cs + (t - _t(tk-1)) / ( _t(tk+1) - _t(tk-1) ) * cd;
  }


  template <typename T>
  inline
  void PERBSCurve<T>::generateKnotVector( PCurve<T,3>* g, int n ) {

    const T  sp = g->getParStart();
    const T  dt = g->getParDelta()/( n-1 );

    _t.setDim(n+2);

    for( int i = 0; i < n; i++ )
      _t[i+1] = sp + i * dt;

    if( isClosed() ) {
      _t[0]   = _t[1] - ( _t[n] - _t[n-1]);
      _t[n+1] = _t[n] + ( _t[2] - _t[1]  );
    }
    else {
      _t[0]   = _t[1];
      _t[n+1] = _t[n];
    }
  }

  template <typename T>
  inline
  void PERBSCurve<T>::splitKnot(int tk) {

    // Return if knot does not exist
    if( tk >= _c.getDim() )
      return;

    // expand knot vector
    _t.insert( tk+1, _t(tk+1) );

    // expand local curves
    PCurve<T,3> *split_curve = static_cast<PCurve<T,3>*>(_c(tk)->makeCopy());
    _c.insert( tk, split_curve );
    insertLocal( split_curve );

    /*!
     *  Scale domain of splited curves
     *
     *  \todo Does this scaling handle multiple knot splits to the "same" knot ?
     */
    split_curve->setDomain( T(0), T(0.5) );
    _c(tk)->setDomain( T(0.5), T(1) );
  }

  template <typename T>
  void PERBSCurve<T>::insertVisualizer(Visualizer* visualizer)  {

    PCurveVisualizer<T,3> *visu = dynamic_cast<PCurveVisualizer<T,3>*>( visualizer );
    if( !visu ) {

      PCurve<T,3>::insertVisualizer( visualizer );
      return;
    }

    if( _pv.exist( visu ) )
      return;

    _pv += visu;
  }

  template <typename T>
  void PERBSCurve<T>::removeVisualizer(Visualizer* visualizer) {

    PCurveVisualizer<T,3> *visu = dynamic_cast<PCurveVisualizer<T,3>*>( visualizer );
    if( !visu ) {

      PCurve<T,3>::removeVisualizer( visualizer );
      return;
    }

    if( !_pv.exist( visu ) )
      return;

    _pv.remove(visu);
  }

  template <typename T>
  void PERBSCurve<T>::replot(int m, int d)  {



    // Correct sample domain
    if( m < 2 )       m = _no_sam;
    else        _no_sam = m;

    // Correct derivatives
    if( d < 1 )       d = _no_der;
    else        _no_der = d;


    // pre-sampel / pre evaluate data for a given parametric curve, if wanted/needed
    preSample( m, d, getStartP(), getEndP() );

    // Predict number of "visualizer"-segments
    DVector< Vector<float,2> > ps(0);
    float prev_ppos = _t[1];
    for( int i = 2; i < _t.getDim()-2; ++i ) {
      if( std::abs( _t[i] - _t[i-1] ) <  1e-5 ) {

        ps.append( Vector<float,2>( prev_ppos, _t[i-1]) );
        prev_ppos = _t[i-1];
      }
    }
    ps.append( Vector<float,2>(prev_ppos,_t[_t.getDim()-2]) );

    // Clean up
    for( int i = 0; i < _pvi.getDim(); ++i )
      for( int j = 0; j < _pvi[i].visus.getSize(); ++j )
        PCurve<T,3>::removeVisualizer( _pvi[i].visus[j] );

    if( _pvi.getDim() != ps.getDim() )
      _pvi.resetDim( ps.getDim() );

    // Insert new visualizers and replot
    Sphere<T,3>  s;
    DVector< DVector< Vector<T,3> > > p;
    for( int i = 0; i < _pvi.getDim(); ++i ) {

      // Update sub-visualizer set
      _pvi[i].updateVisualizerSet(_pv);
      _pvi[i].segment = ps[i];

      // Get visualizer and domain of i-th sub-visualizer set
      const Array< PCurveVisualizer<T,3>* > &sub_visus  = _pvi[i].visus;
      const Vector<float,2>                 &segment     = _pvi[i].segment;

      for( int j = 0; j < sub_visus.getSize(); ++j )
        PCurve<T,3>::insertVisualizer( sub_visus(j) );

      // Resample i-th segment
      this->resample( p, m, d, segment(0), segment(1) );

      // Replot visualizers of i-th segment
      for( int j = 0; j < sub_visus.getSize(); ++j )
        sub_visus(j)->replot( p, m, d, (_pvi.getDim() == 1) && isClosed() );

      // Surrounding sphere
      if( i == 0 )
        s.resetPos( p(0)(0) );
      else
        s += p(0)(0);
      s += p(p.getDim() - 1)(0);
      for( int j = p.getDim() - 2; j > 0; j-- )
        s += p(j)(0);
    }

    // Set surrounding sphere
    Parametrics<T,1,3>::setSurroundingSphere( s.template toType<float>() );
  }



} // END namespace GMlib



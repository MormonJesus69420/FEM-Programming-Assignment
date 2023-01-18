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




#ifndef GM_PARAMETRICS_PCURVE_H
#define GM_PARAMETRICS_PCURVE_H



#include "gmparametrics.h"

// gmlib
#include <core/containers/gmarray.h>
#include <core/containers/gmdvector.h>



namespace GMlib {

  template <typename T, int n>
  class PCurveVisualizer;

  template <typename T, int n>
  class PCurveDefaultVisualizer;


  template <typename T,int n>
  class PCurve : public Parametrics<T,1,n> {
  public:
    PCurve( int s = 20 );
    PCurve( const PCurve<T,n>& copy );
    ~PCurve();

    DVector<Vector<T,n> >&        evaluate( T t, int d ) const;
    DVector<Vector<T,n> >&        evaluateGlobal( T t, int d ) const;
    DVector<Vector<T,n> >&        evaluateParent( T t, int d ) const;

    bool                          getClosestPoint(const Point<T,n>& q, T& t, Point<T,n>& p, double eps = 10e-6, int max_iterations = 20) const;
    T                             getCurvature( T t ) const;
    T                             getCurveLength( T a = 0, T b = -1 ) const;
    int                           getDerivatives() const;
    Vector<T,n>                   getDer1( T t ) const;
    Vector<T,n>                   getDer2( T t ) const;
    Vector<T,n>                   getDer3( T t ) const;

    T                             getParStart() const;
    T                             getParDelta() const;
    T                             getParEnd()   const;
    T                             getRadius( T t ) const;
    int                           getSamples() const;
    T                             getSpeed( T t ) const;
    virtual bool                  isClosed() const;
    virtual void                  preSample( int m, int d, T s = T(0), T e = T(0) );
    virtual void                  replot( int m = 0, int d = 2 );
//    virtual void              resample( Array<Point<T,n> >& a, T eps );	// Always smooth, requires derivatives
    void                          resample( DVector< DVector< Vector<T,n> > >& p, int m, int d );
    virtual void                  resample( DVector< DVector< Vector<T,n> > >& p, int m, int d, T start, T end );
    void                          setDomain( T start, T end );
    void                          setDomainScale( T sc );
    void                          setDomainTrans( T tr );
    void                          setNoDer( int d );
    virtual void                  setSurroundingSphere( const DVector< DVector< Vector<T,n> > >& p );

    void                          enableDefaultVisualizer( bool enable = true );
    const PCurveVisualizer<T,n>*  getDefaultVisualizer() const;
    void                          toggleDefaultVisualizer();
    void                          insertVisualizer( Visualizer* visualizer );
    void                          removeVisualizer( Visualizer* visualizer );

    Point<T,n>                operator()( T t );

  protected:
    Array<PCurveVisualizer<T,n>*> _pcurve_visualizers;
    PCurveVisualizer<T,n>*        _default_visualizer;

    int                           _no_sam;      // Number of samples for single sampling
    int                           _no_der;      // Number of derivatives


    DVector< Vector<T,2> >        _sam_p;       // Sampling partition (start/stop)
    DVector< int >                _no_sam_p;    // Number of samples for each partition
    int                           _defalt_d;    // used by operator() for number of derivative to evaluate.


    // The result of the previous evaluation
    mutable DVector<Vector<T,n> >  _p;           // Position and belonging derivatives
    DVector<Vector<T,n> >&         _q = _p;
    mutable T                      _t;           // The parameter value used for last evaluation
    mutable int                    _d;           // Number of derivatives computed last time

    // Shift of parameter.
    T                             _tr;          // translate
    T                             _sc;          // scale
    T                             shift( T t ) const;


    virtual void                  eval(T t, int d, bool l = true ) const = 0;
    virtual T                     getEndP()     const = 0;
    virtual T                     getStartP()   const = 0;

  private:
    void                          _eval( T t, int d ) const;
    T                             _integral(T a, T b, double eps) const;

  }; // END class PCurve

} // END namespace GMlib

// Include PCurve class function implementations
#include "gmpcurve.c"


#endif  // GM_PARAMETRICS_PCURVE_H


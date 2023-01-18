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




#ifndef GM_PARAMETRICS_CURVES_PBSPLINECURVE_H
#define GM_PARAMETRICS_CURVES_PBSPLINECURVE_H


#include "../gmpcurve.h"


namespace GMlib {


  template <typename T>
  class PBSplineCurve : public PCurve<T,3> {
    GM_SCENEOBJECT(PBSplineCurve)
  public:
    PBSplineCurve( const DVector< Vector<T, 3> >& c, int d );
    PBSplineCurve( const PBSplineCurve<T>& curve );
    virtual ~PBSplineCurve();

    void                      setDegree( int d );
    int                       getDegree() const;
    void                      setScale( T d );
    void                      setControlPoints( const DVector< Vector<T,3> >& cv, bool gen_t = true );
    void                      setKnotVector( const DVector<T>& t );
    void                      setResampleMode( GM_RESAMPLE_MODE mode );
    void                      generateKnotVector();
    void                      resample( DVector< DVector< Vector<T,3> > >& p, int m, int d, T start, T end );

    //****************************************
    //****** Virtual public functions   ******
    //****************************************

    // from PCurve
    bool                      isClosed() const override;

  protected:
    // Virtual function from PCurve that has to be implemented locally
    void                      eval(T t, int d = 0, bool l = false) const override;
    T                         getStartP() const override;
    T                         getEndP()   const override;

    // Help function
//    void                      evalBernsteinHermite( DMatrix<T>& bp_mat, T t, int idx ) const;
//    T                         getW( T t, int idx, int d ) const;
//    T                         getWder( T t, int idx, int d ) const;
//    int                       getIndex( T t, int m ) const;
    void                      resampleInline( DVector< DVector< Vector<T,3> > >& p, int m, T dt );
    void                      resamplePreEval( DVector< DVector< Vector<T,3> > >& p, int m, T dt );
    void                      multEval(DMatrix<Vector<T,3>>& p, const DMatrix<T>& bsh, const DVector<int>& ii, int d);

    // Protected data for the curve
    DVector< Vector<T,3> >    _c; // control points (controlpolygon)
    int                       _d; // polynomial degree
    DVector<T>                _t; // knot vector

    T                         _scale;

    DVector< DMatrix< T > >   _bhps;
    DVector< int >            _indices;

    GM_RESAMPLE_MODE          _resamp_mode;
    bool                      _pre_eval;

  }; // END class PBSplineCurve


} // END namepace GMlib

// Include PBSplineCurve class function implementations
#include "gmpbsplinecurve.c"


#endif // GM_PARAMETRICS_CURVES_PBSPLINECURVE_H



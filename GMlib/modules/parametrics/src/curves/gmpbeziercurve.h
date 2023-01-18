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



#ifndef GM_PARAMETRICS_CURVES_PBEZIERCURVE_H
#define GM_PARAMETRICS_CURVES_PBEZIERCURVE_H


#include "../gmpcurve.h"



namespace GMlib {

  template <typename T, int n>
  class Selector;

  template <typename T>
  class SelectorGridVisualizer;

  template <typename T>
  class PBezierCurve : public PCurve<T,3> {
    GM_SCENEOBJECT(PBezierCurve)
  public:
    PBezierCurve( const DVector< Vector<T,3> >& c );
    PBezierCurve( const DVector< Vector<T,3> >& c, T s, T t, T e );
    PBezierCurve( const PBezierCurve<T>& copy );
    virtual ~PBezierCurve();

    void                            edit( int selector ) override;
    DVector< Vector<T,3> >&         getControlPoints();
    int                             getDegree() const;
    virtual void                    hideSelectors();
    bool                            isSelectorsVisible() const;
    void                            setClosed( bool state );
    void                            setControlPoints( const DVector< Vector<T,3> >& cv );
    void                            setResampleMode( GM_RESAMPLE_MODE mode );
    void                            setScale( T d );
    virtual void                    showSelectors( bool grid = false, const Color& selector_color = GMcolor::darkBlue(), const Color& grid_color = GMcolor::lightGreen() );
    void                            updateCoeffs( const Vector<T,3>& d );

    // virtual from PCurve
    bool                            isClosed() const override;
    void                            preSample( int m, int d, T start, T end ) override;



  protected:
    void                            eval( T t, int d = 0, bool l = false ) const override;
    T                               getEndP()   const override;
    T                               getStartP() const override;

    DVector< Vector<T, 3> >         _c;
    T                               _scale;
    DVector< DMatrix< T > >         _t;
    bool                            _closed;

    GM_RESAMPLE_MODE                _resamp_mode;
    bool                            _pre_eval;

    bool                            _selectors;
    SelectorGridVisualizer<T>*      _sgv;
    DVector< Selector<T,3>* >       _s;
    bool                            _c_moved;

    void                            evalPre( T t, int d = 0, bool l = false );
    void                            findIndex( T t, int& it );
    virtual void                    init();
  }; // END class PBezierCurve

} // END namepace GMlib


// Include PBezierCurve class function implementations
#include "gmpbeziercurve.c"


#endif // GM_PARAMETRICS_CURVES_PBEZIERCURVE_H


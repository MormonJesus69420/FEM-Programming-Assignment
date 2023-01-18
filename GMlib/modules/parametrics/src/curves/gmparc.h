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



#ifndef GM_PARAMETRICS_CURVES_PARC_H
#define GM_PARAMETRICS_CURVES_PARC_H


#include "../gmpcurve.h"


namespace GMlib {


  template <typename T>
  class PArc : public PCurve<T,3> {
    GM_SCENEOBJECT(PArc)
  public:
    PArc( T speed, T curvature );
    PArc( T speed, T curvature, T start, T end );
    PArc( DVector< Vector<T,3> >& p, T s, T t, T e );
    PArc( const PArc<T>& copy );

    virtual ~PArc();

    // Public local functions
    void                setCurvature( T curvature );
    T                   getCurvature() const;
    void                setSpeed( T speed );
    T                   getSpeed() const;

    void                setStart( T start );
    void                setEnd( T end );

    //****************************************
    //****** Virtual public functions   ******
    //****************************************

    // from PCurve
    bool                isClosed() const override;

  protected:
    // Virtual function from PCurve that has to be implemented locally
    void                eval(T t, int d, bool l) const override;
    T                   getStartP() const override;
    T                   getEndP()   const override;

    // Help function
    T                   getCurvature( DVector< Vector<T,3> >& p );

    // Protected data for the curve
    T                   _d;       // Delta
    T                   _k;       // Kappa
    T                   _start;   // Start
    T                   _end;     // End

  }; // END class PArc

} // END namepace GMlib

// Include PArc class function implementations
#include "gmparc.c"


#endif // GM_PARAMETRICS_CURVES_PARC_H


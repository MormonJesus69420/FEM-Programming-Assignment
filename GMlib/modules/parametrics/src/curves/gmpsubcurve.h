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




#ifndef GM_PARAMETRICS_CURVES_PSUBCURVE_H
#define GM_PARAMETRICS_CURVES_PSUBCURVE_H


#include "../gmpcurve.h"


namespace GMlib {


  template <typename T>
  class PSubCurve : public PCurve<T,3> {
    GM_SCENEOBJECT(PSubCurve)
  public:
    PSubCurve( PCurve<T,3>* c, T s, T e);
    PSubCurve( PCurve<T,3>* c, T s, T e, T t);
    PSubCurve( const PSubCurve<T>& copy );

    virtual ~PSubCurve();

    // Public local functions
    void                    togglePlot() {} // Not implemented yet

    //****************************************
    //****** Virtual public functions   ******
    //****************************************

    // from PCurve
    virtual bool        isClosed() const;

  protected:
    // Virtual function from PCurve that has to be implemented locally
    void                eval(T t, int d, bool l) const override;
    T                   getStartP() const override;
    T                   getEndP()   const override;

    // Protected data for the curve
    PCurve<T,3>*        _c;
    T                   _s;
    T                   _t;
    T                   _e;
    Vector<float,3>     _trans;

  private:

    // Private help functions
    void set(PCurve<T,3>* c, T s, T e, T t);

  }; // END class PSubCurve

} // END namepace GMlib


// Include PSubCurve class function implementations
#include "gmpsubcurve.c"


#endif // GM_PARAMETRICS_CURVES_PSUBCURVE_H


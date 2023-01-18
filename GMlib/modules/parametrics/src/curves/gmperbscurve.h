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




#ifndef GM_PARAMETRICS_CURVES_PERBSCURVE_H
#define GM_PARAMETRICS_CURVES_PERBSCURVE_H


#include "../gmpcurve.h"

#include "../evaluators/gmerbsevaluator.h"


namespace GMlib {


  template <typename T>
  class PCurveVisualizerSet {
  public:
    PCurveVisualizerSet() {}
    PCurveVisualizerSet( const Array<PCurveVisualizer<T,3>*>& skel ) { updateVisualizerSet(skel); }

    void    updateVisualizerSet( const Array<PCurveVisualizer<T,3>*>& skel ) {

      // Delete visualizers
      for( int i = 0; i < visus.getSize(); ++i )
        delete visus[i];
      visus.clear();

      // Create new ones
      for( int i = 0; i < skel.getSize(); ++i )
        visus += static_cast<PCurveVisualizer<T,3>*>(skel(i)->makeCopy());
    }

    Array< PCurveVisualizer<T,3>* >    visus;
    Vector<float,2>                    segment;
  };


  template <typename T>
  class PERBSCurve : public PCurve<T,3> {
    GM_SCENEOBJECT(PERBSCurve)
  public:

    enum CURVE_TYPE {
      SUB_CURVE   = 0,
      ARC_CURVE   = 1,
      BEZIERCURVE = 2
    };

    PERBSCurve(); // Dummy
    PERBSCurve( CURVE_TYPE type, PCurve<T,3>* g, int n, int d = 2 );
    PERBSCurve( const PERBSCurve<T>& copy );
    virtual ~PERBSCurve();

    void                            setResampleMode( GM_RESAMPLE_MODE mode );

    // Local curves
    DVector< PCurve<T,3>* >&        getLocalCurves();
    const DVector< PCurve<T,3>* >&  getLocalCurves() const;
    int                             getNoLocalCurves() const;
    virtual void                    hideLocalCurves();
    virtual void                    showLocalCurves();
    virtual void                    toggleLocalCurves();
    bool                            isLocalCurvesVisible() const;

    // Knot insertion
    void                            splitKnot( int tk );


    // virtual functions from DO/PCurve
    void                            insertVisualizer(Visualizer* visualizer ) override;
    void                            removeVisualizer(Visualizer *visualizer) override;

    // virtual functions from PCurve
    void                            edit( SceneObject *obj ) override;
    bool                            isClosed() const override;
    void                            preSample( int m, int d, T start, T end ) override;
    void                            replot(int m = 0, int d = 0) override;

  protected:
    bool                            _closed;
    DVector<T>                      _t;
    DVector<PCurve<T,3>*>           _c;

    BasisEvaluator<long double>*    _evaluator;

    // Using pre evaulating of GERBS-basis functions
    GM_RESAMPLE_MODE                _resamp_mode;
    bool                            _pre_eval; // To mark that pre eval is done
    DVector< int >                  _tk;       // pre evaluation vector
    DVector< DVector<T> >           _B;        // Storing sample values - GERBS

    // virual functions from PSurf
    void                            eval( T t, int d = 0, bool l = false ) const override;
    T                               getEndP()   const override;
    T                               getStartP() const override;

    // Local help functions
    int                             findIndex( T t) const;
    void                            getB( DVector<T>& B, int tk, T t, int d ) const;

  private:
    Array<PCurveVisualizer<T,3>*>   _pv;
    DVector<PCurveVisualizerSet<T>> _pvi;

    int                             _no_sam;
    int                             _no_der;

    // Local help functions
    void                            compBlend( int d, const DVector<T>& B,
                                               DVector< Vector<T,3> >& c0, DVector< Vector<T,3> >& c1) const;
    void                            generateKnotVector( PCurve<T,3>* g, int n );
    virtual void                    init();
    void                            insertLocal( PCurve<T,3> *patch );
    PCurve<T,3>*                    makeLocal( CURVE_TYPE type, PCurve<T,3>* g, T s, T t, T e, int d=2);

    T                               mapToLocal( T t, int tk ) const;

  }; // END class PERBSCurve

} // END namepace GMlib

// Include PERBSCurve class function implementations
#include "gmperbscurve.c"


#endif // GM_PARAMETRICS_CURVES_PERBSCURVE_H



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


#include "../evaluators/gmerbsevaluator.h"

namespace GMlib {


//*****************************************
// Constructors and destructor           **
//*****************************************


  template <typename T, typename G>
  PBasisCurve<T,G>::PBasisCurve() {
    _B = new ERBSEvaluator<G>();
    _B->set( 0.0f, 1.0f );
    _d_no = 0;
  }

  template <typename T, typename G>
  PBasisCurve<T,G>::~PBasisCurve() {
    delete _B;
  }



  //**************************************
  //        Public local functons       **
  //**************************************

  template <typename T, typename G>
  void PBasisCurve<T,G>::setDisplay( int display ) {
    _d_no = display;
  }

  template <typename T, typename G>
  void PBasisCurve<T,G>::setEvaluator( BasisEvaluator<G> *e ) {
    if( !e ) return;
    if( _B ) delete _B;
    _B = e;
  }


  //***************************************************
  // Overrided (public) virtual functons from PCurve **
  //***************************************************

  template <typename T, typename G>
  bool PBasisCurve<T,G>::isClosed() const {
    return false;
  }


  //******************************************************
  // Overrided (protected) virtual functons from PCurve **
  //******************************************************

  template <typename T, typename G>
  void PBasisCurve<T,G>::eval( T t, int /*d*/, bool /*l*/ ) const {

    this->_p.setDim(1);
    float value = _B->operator()(t);

    switch( _d_no ) {
    case 1: value = _B->getDer1();  break;
    case 2: value = _B->getDer2();  break;
    }


    this->_p[0][0] = t;
    this->_p[0][1] = value;
    this->_p[0][2] = 0.0f;
  }



  template <typename T, typename G>
  T PBasisCurve<T,G>::getStartP() const {
    return 0.0f;
  }


  template <typename T, typename G>
  T PBasisCurve<T,G>::getEndP() const {
    return 1.0f;
  }

} // END namespace GMlib

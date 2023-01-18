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





namespace GMlib {


//*****************************************
// Constructors and destructor           **
//*****************************************

  template <typename T>
  inline
  PCircle<T>::PCircle( T radius ) {
    _r = radius;
  }


  template <typename T>
  inline
  PCircle<T>::PCircle( const PCircle<T>& copy ) : PCurve<T,3>(copy) {}


  template <typename T>
  PCircle<T>::~PCircle() {}


  //**************************************
  //        Public local functons       **
  //**************************************

  template <typename T>
  inline
  T PCircle<T>::getRadius() const {
    return _r;
  }


  template <typename T>
  inline
  void PCircle<T>::setRadius( T radius ) {
      _r = radius;
  }


  //***************************************************
  // Overrided (public) virtual functons from PCurve **
  //***************************************************

  template <typename T>
  bool PCircle<T>::isClosed() const {
    return true;
  }


  //******************************************************
  // Overrided (protected) virtual functons from PCurve **
  //******************************************************

  template <typename T>
  void PCircle<T>::eval( T t, int d, bool /*l*/ ) const {

    this->_p.setDim( d + 1 );

    const T ct = _r * cos(t);
    const T st = _r * sin(t);

    this->_p[0][0] = ct;
    this->_p[0][1] = st;
    this->_p[0][2] = T(0);

    if( this->_dm == GM_DERIVATION_EXPLICIT ) {

      if( d > 0 ) {
        this->_p[1][0] = -st;
        this->_p[1][1] =  ct;
        this->_p[1][2] =  T(0);
      }
      if( d > 1 ) this->_p[2] = -this->_p[0];
      if( d > 2 ) this->_p[3] = -this->_p[1];
      if( d > 3 ) this->_p[4] = this->_p[0];
      if( d > 4 ) this->_p[5] = this->_p[1];
      if( d > 5 ) this->_p[6] = this->_p[2];
      if( d > 6 ) this->_p[7] = this->_p[3];
    }
  }


  template <typename T>
  T PCircle<T>::getStartP() const {
    return T(0);
  }


  template <typename T>
  T PCircle<T>::getEndP()const {
    return T( M_2PI );
  }


} // END namespace GMlib

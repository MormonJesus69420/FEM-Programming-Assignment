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
  PRoseCurve<T>::PRoseCurve( int n, int m, T radius ) {

    _r = radius;
    _n = n;
    _m = m;
    _k = T(_n)/T(_m);
    _k2 = T((_n*_n)+(_m*_m))/T(_m*_m);

    if((_n*_m) % 2 == 0)
        _l = 2;
    else
        _l = 1;
  }


  template <typename T>
  inline
  PRoseCurve<T>::PRoseCurve( const PRoseCurve<T>& copy ) : PCurve<T,3>( copy ) {}


  template <typename T>
  PRoseCurve<T>::~PRoseCurve() {}



  //***************************************************
  // Overrided (public) virtual functons from PCurve **
  //***************************************************

  template <typename T>
  bool PRoseCurve<T>::isClosed() const {
    return true;
  }


  //******************************************************
  // Overrided (protected) virtual functons from PCurve **
  //******************************************************

  template <typename T>
  void PRoseCurve<T>::eval( T t, int d, bool /*l*/ ) const {

      this->_p.setDim(d + 1);

      this->_p[0][0] = _r * T(cos(t)*cos(_k*t));
      this->_p[0][1] = _r * T(sin(t)*cos(_k*t));
      this->_p[0][2] = T(0);

      if( this->_dm == GM_DERIVATION_EXPLICIT ) {
          if(d > 0) {
              this->_p[1][0] = _r * T(-_k*cos(t) * sin(_k*t) - sin(t)*cos(_k*t));
              this->_p[1][1] = _r * T(-_k*sin(t) * sin(_k*t) + cos(t)*cos(_k*t));
              this->_p[1][2] = T(0);
              if(d > 1) {
                  this->_p[2][0]= _r * T( 2*_k*sin(t)*sin(_k*t) - _k2*cos(t)*cos(_k*t));
                  this->_p[2][1]= _r * T(-2*_k*cos(t)*sin(_k*t) - _k2*sin(t)*cos(_k*t));
                  this->_p[2][2]= T(0);
              }
          }
      }
  }


  template <typename T>
  T PRoseCurve<T>::getStartP() const {
    return T(0);
  }


  template <typename T>
  T PRoseCurve<T>::getEndP() const {
    return T( _m * _l * M_PI );
  }


}


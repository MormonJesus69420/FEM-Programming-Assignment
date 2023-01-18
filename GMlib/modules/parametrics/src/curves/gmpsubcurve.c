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
  PSubCurve<T>::PSubCurve( PCurve<T,3>* c, T s, T e )
  {
    set(c, s, e, (e+s)/2);

    DVector<Vector<T,3> > tr = _c->evaluateParent(_t, 0);
    _trans = tr[0];
    this->translateParent( _trans );
  }


  template <typename T>
  inline
  PSubCurve<T>::PSubCurve( PCurve<T,3>* c, T s, T e, T t )
  {
    set(c, s, e, t);

    DVector<Vector<T,3> > tr = _c->evaluateParent(_t, 0);
    _trans = tr[0];
    this->translateParent( _trans );
  }


  template <typename T>
  inline
  PSubCurve<T>::PSubCurve( const PSubCurve<T>& copy ) : PCurve<T,3>( copy )
  {
    set(copy._c, copy._s, copy._e, copy._t);

    _trans = copy._trans;
  }


  template <typename T>
  PSubCurve<T>::~PSubCurve() {}



  //***************************************************
  // Overrided (public) virtual functons from PCurve **
  //***************************************************

  template <typename T>
  bool PSubCurve<T>::isClosed() const {
    return false;
  }



  //******************************************************
  // Overrided (protected) virtual functons from PCurve **
  //******************************************************


  template <typename T>
  void PSubCurve<T>::eval( T t, int d, bool /*l*/ ) const
  {
    this->_p     = _c->evaluateParent(t , d);
    this->_p[0] -= _trans;
  }


  template <typename T>
  T PSubCurve<T>::getStartP() const {
    return _s;
  }


  template <typename T>
  T PSubCurve<T>::getEndP() const {
    return _e;
  }


  //***************************
  // Private help functions  **
  //***************************

  template <typename T>
  inline
  void PSubCurve<T>::set(PCurve<T,3>* c, T s, T e, T t)
  {
    _c = c;
    _s = s;
    _t = t;
    _e = e;
  }


} // END namespace GMlib


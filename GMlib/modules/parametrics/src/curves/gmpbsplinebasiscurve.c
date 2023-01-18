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
  PBSplineBasisCurve<T>::PBSplineBasisCurve( const DVector<T>& t ) {
    int k = t.getDim()-1;
    _t.setDim(3*k-1);
    for(int i=0; i<k-1; i++)
      _t[i] = t(0)- T(k-i-1);
    for(int i=k-1; i < 2*k; i++)
      _t[i] = t(i-k+1);
    for(int i=2*k; i<_t.getDim(); i++)
      _t[i] = t(k)+T(i-2*k+1);
  }


  template <typename T>
  inline
  PBSplineBasisCurve<T>::PBSplineBasisCurve( const PBSplineBasisCurve<T>& copy ) : PCurve<T,3>( copy ) {
      _t = copy._t;
  }


  template <typename T>
  PBSplineBasisCurve<T>::~PBSplineBasisCurve() {}


  //***************************************************
  // Overrided (public) virtual functons from PCurve **
  //***************************************************

  template <typename T>
  bool PBSplineBasisCurve<T>::isClosed() const {
    return false;
  }



  //******************************************************
  // Overrided (protected) virtual functons from PCurve **
  //******************************************************

  template <typename T>
  void PBSplineBasisCurve<T>::eval( T t, int d, bool /*l*/ ) const {
    this->_p.setDim( d + 1 );

    int k = (_t.getDim()+1)/3;
    int i = findIndex(t,k-1);

    DMatrix<T> B(k,k);
    makeMat(B, i, k-1, t, 1);

    i = 2*(k-1)-i;

    this->_p[0][0] = B[0][i];
    this->_p[0][1] = t;
    this->_p[0][2] = 0;

    if(d>0)
    {
      this->_p[1][0] = B[1][i];
      this->_p[1][1] = 1;
      this->_p[1][2] = 0;
      if(d>1)
      {
        this->_p[1][0] = B[2][i];
        this->_p[1][1] = 0;
        this->_p[1][2] = 0;
        if(d>2)
        {
          this->_p[1][0] = B[3][i];
          this->_p[1][1] = 0;
          this->_p[1][2] = 0;
        }
      }
    }
  }



  template <typename T>
  T PBSplineBasisCurve<T>::getStartP() const {
    int k = (_t.getDim()+1)/3;
    return _t(k-1);
  }


  template <typename T>
  T PBSplineBasisCurve<T>::getEndP() const {
    int k = (_t.getDim()+1)/3;
    return _t(_t.getDim()-k);
  }




  //*****************************************
  //     Local (protected) functons        **
  //*****************************************


  template <typename T>
  inline
  T PBSplineBasisCurve<T>::W( T t, int d,  int i) const {
    return (t-_t(i))/(_t(i+d)-_t(i));
  }


  template <typename T>
  inline
  T PBSplineBasisCurve<T>::delta(T s, int d,  int i) const {
    return s/(_t(i+d)-_t(i));
  }


  template <typename T>
  void PBSplineBasisCurve<T>::makeMat( DMatrix<T>& mat, int ii, int d, T t, T scale ) const {
    mat.setDim( d+1, d+1 );
    DVector<T> w(d);

    // Compute the Bernstein-Hermite polynomials 1 -> d.
    // One for each row, starting from the bottom up.
    w[0] = W(t,1,ii);
    mat[d-1][0] = 1 - w[0];
    mat[d-1][1] = w[0];
    for( int i = d-2; i >= 0; i-- )
    {
      int di  = d-i;
      int di1 = di-1;
      int i1 = i+1;
      for( int j = 0; j < di; j++ ) w[j] = W(t,di,ii-di1+j);
      mat[i][0] = ( 1 - w[0]) * mat[i1][0];
      for( int j = 1; j < di; j++ )
        mat[i][j] = w[j-1] * mat[i1][j-1] + (1 - w[j]) * mat[i1][j];
      mat[i][di] = w[di1]* mat[i1][di1];
    }
    // Compute all the deriatives :P
    w[0] = delta(scale,1,ii);
    mat[d][0] = -w[0];
    mat[d][1] =  w[0];

    for( int k = 2; k <= d; k++ )
    {
      const double s = k * scale;
      for( int i = d; i > d - k; i-- )
      {
        int di  = d-i;
        int di1 = di-1;
        for( int j = 0; j < di; j++ ) w[j] = delta(s,di,ii-di1+j);

        mat[i][k] = w[di1] * mat[i][k-1];
        for( int j = k - 1; j > 0; j-- )
          mat[i][j] = w[j-1] * mat[i][j-1] - w[j] * mat[i][j] ;
        mat[i][0] = - w[0]* mat[i][0];
      }
    }
  }



  template <typename T>
  inline
  int PBSplineBasisCurve<T>::findIndex(T t, int j) const {
    int i=j;
    for( ; i<_t.getDim()-j-1;i++)
      if(_t[i+1]>t) break;
    if(i == _t.getDim()-j-1) i--;
    return i;
  }


} // END namespace GMlib


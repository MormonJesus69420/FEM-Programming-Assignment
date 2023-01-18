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



// local
#include "../utils/gmutils.h"

// stl
#include <cstring>
#include <cmath>

namespace GMlib {

//******************************
//** Static members of APoint **
//******************************

  template <typename T, int n>
  Arrow<T, n>* APoint<T, n>::_arrow = new Arrow<T, n>();

  template <typename T, int n>
  int APoint<T, n>::_sortType = 1;



  //******************************
  //**     Members of APoint    **
  //******************************


  /*! APoint::APoint()
   *  \brief  Default Constructor
   *
   *  Takes no paramters, and will not initalize any values.
   *  This has been done intentionaly, especially to speedup creation of matrices.
   *  If point should be used by itself, use one of the other constructors.
   *  \see APoint( T t )
   *  \see APoint( const T *t )
   */
  template <typename T, int n>
  inline
  APoint<T, n>::APoint() {
    // SHOULD NOT INITIALIZE ANYTHING
    // Intentinonal speed enhancement.
  }


  template <typename T, int n>
  inline
  APoint<T, n>::APoint( const APoint<T, n> &p ) {
    _cpy(p);
  }


  /*! APoint::APoint( T t )
   *  \brief  Constructuer taking an init value
   *
   *  Initializing each component of the point to the init value
   *
   * \param t An initial value
   */
  template <typename T, int n>
  inline
  APoint<T, n>::APoint( T t ) {
    _cpy(t);
  }


  /*! APoint::APoint( const T *t )
   *  \brief  Constructuer taking an array of n T values.
   *
   *  This constructor has been created to make the point class
   *  compatible with other librarys and generel datasets.
   *  The constructor presupposes that the input array has n T values.
   *
   * \param t A const T pointer of n values.
   */
  template <typename T, int n>
  inline
  APoint<T, n>::APoint( const T *t ) {
    _cpy(t);
  }


  /*! Angle APoint<T, n>::getAngle( APoint<T, n> v ) const
   *  \brief Returns the angle between two points
   *
   *  Returns the angle between two vectors
   *  Has been moved down into the point class from the vector class,
   *  for convenience.
   *
   *  \param v The point to which the angle should be calculated.
   *  \return The angle between this point and the given point.
   */
  template <typename T, int n>
  Angle APoint<T, n>::getAngle( APoint<T, n> v ) const {

     APoint<T, n> u = (*this);
     u /= u.getLength();
     v /= v.getLength();
     double ang = u*v;
     if (ang > 1.0) ang = 1.0;
     else if (ang < -1.0) ang = -1.0;
     ang = acos(ang);
     if (n ==2)
     {
        double orient = u[0]*v[1]-u[1]*v[0];
        if (orient >= 0.0) 	return Angle(ang);
        else 		            return Angle(2*M_PI - ang);
     }
     else                     return Angle(ang);
  }


  /*! T APoint<T, n>::getLength() const
   *  \brief Returns the length of the point (vector)
   *
   *  Returns the length of the point (vector)
   *  Has been moved down into the point class from the vector class for convenience.
   *
   *  \return The length of the point (vector)
   */
  template <typename T, int n>
  inline
  T APoint<T, n>::getLength() const {
    return T( sqrt( double( (*this) * (*this) ) ) );
  }


  /*! int APoint<T, n>::getMaxIndex() const
   *  \brief Returns the index of the first point element with the max value.
   *
   *  Returns the index of the first point element with the max value.
   *  Dependent on the > (greater then) operator
   *
   *  \return The index of the first max value
   */
  template <typename T, int n>
  inline
  int APoint<T, n>::getMaxIndex() const {
    // return GM_Static_<T,n>::maxIndex(pt);
    int j=0;
    for(int i=1; i<n; i++)
      if( _pt[i] > _pt[j] ) j = i;
    return j;
  }


  /*! int APoint<T, n>::getMaxAbsIndex() const
   *  \brief Returns the index of the first point element with the max absolute value.
   *
   *  Returns the index of the first point element with the max absolute value.
   *  Dependent on the > (greater then) operator.
   *
   *  \return The index of the first max absolute value.
   */
  template <typename T, int n>
  inline
  int APoint<T, n>::getMaxAbsIndex() const {
    // return GM_Static_<T,n>::maxIndex(pt);
    int j=0;
    for(int i=1; i<n; i++)
      if( std::fabs( _pt[i] ) > std::fabs( _pt[j] ) ) j = i;
    return j;
  }


  /*! \brief Returns a non-const pointer to the point element dataset. */
  template <typename T, int n>
  inline
  T* APoint<T, n>::getPtr() {
    return reinterpret_cast<T*>(this);
  }

  /*! \brief Returns a const pointer to the point element dataset. */
  template <typename T, int n>
  inline
  const T* APoint<T, n>::getPtr() const {
    return reinterpret_cast<const T*>(this);
  }


  /*! void APoint<T, n>::setTestType( int t, const APoint<T, n> &p, const Vector<T, n> &v )
   *  \brief Set how a point should be sorted in accordance to other points.
   *
   *  Set how a point should be sorted in accordance to other points.
   *  When set to type 1 the point will be sorted as closest to a point.
   *  When set to type 2 the point will be sorted as closes to a directional vector.
   *  When set to type 3 the point will be sorted as closes to a radar vector rotating 360 degreas from the offset point and vector.
   *
   *  \param[in] t The sorting type/method
   *  \param[in] p The point to sort after
   *  \param[in] v A complementary directional vector, when sorting according to degreas (radar)
   */
  template <typename T, int n>
  inline
  void APoint<T, n>::setTestType( int t, const APoint<T, n> &p, const Vector<T, n> &v ) {

    _sortType = t;
    if (t==1)
      _arrow->setPos(p);
    else if (t==2)
      _arrow->setDir(p);
    else if (t==3){
      _arrow->setPos(p);
      _arrow->setDir(v);
    }
    else {
      // Use a debug error message
    }
  }


  template <typename T, int n>
  inline
  const APoint<T, n>& APoint<T, n>::operator = ( const T t ) {
    _cpy(t);
    return *this;
  }


  template <typename T, int n>
  inline
  const APoint<T, n>& APoint<T, n>::operator = ( const T *t ) {
    _cpy(t);
    return (*this);
  }


  template <typename T, int n>
  inline
  const APoint<T, n>& APoint<T, n>::operator = ( const APoint<T, n>& p) {
    _cpy(p);
    return (*this);
  }


  template <typename T, int n>
  inline
  const APoint<T, n>& APoint<T, n>::operator += ( const APoint<T, n>& p ) {
    GM_Static_<T, n>::peq( getPtr(), p.getPtr() );
    return *this;
  }


  template <typename T, int n>
  inline
  const APoint<T, n>& APoint<T, n>::operator -= ( const APoint<T, n>& p ) {
    GM_Static_<T, n>::meq( getPtr(), p.getPtr() );
    return *this;
  }


  template <typename T, int n>
  inline
  APoint<T, n> APoint<T, n>::operator - () const {
    APoint<T, n> r;
    GM_Static_<T, n>::eqm( r.getPtr(), getPtr());
    return r;
  }


  template <typename T, int n>
  inline
  APoint<T, n> APoint<T, n>::operator + ( const APoint<T, n> &p ) const {
    APoint<T, n> r;
    GM_Static_<T, n>::eq_p( r.getPtr(), getPtr(), p.getPtr());
    return r;
  }


  template <typename T, int n>
  inline
  APoint<T, n> APoint<T, n>::operator - ( const APoint<T, n> &p ) const {
    APoint<T, n> r;
    GM_Static_<T, n>::eq_m( r.getPtr(), getPtr(), p.getPtr() );
    return r;
  }

  // Inner product
  template <typename T, int n>
  inline
  T APoint<T, n>::operator * ( const APoint<T, n> &p ) const {
    return GM_Static_<T, n>::dpr( getPtr(), p.getPtr() );
  }

  template <typename T, int n>
  inline
  T& APoint<T, n>::operator [] ( int i) {
    return _pt[i];   //return given element.
  }

  template <typename T, int n>
  inline
  T const& APoint<T, n>::operator [] ( int i) const {
    return _pt[i]; //return copy.
  }

  template <typename T, int n>
  inline
  T const& APoint<T, n>::operator () ( int i) const {
    return _pt[i]; //return copy.
  }


  template <typename T, int n>
  inline
  const APoint<T, n>& APoint<T, n>::operator *= ( double d ) {
    GM_Static_<T, n>::sc( getPtr(), d );
    return *this;
  }


  template <typename T, int n>
  inline
  APoint<T, n> APoint<T, n>::operator * ( double d ) const {
    APoint<T, n> r;
    GM_Static_<T, n>::sc_r( r.getPtr(), getPtr(), d);
    return r;
  }

  template <typename T, int n>
  inline
  const APoint<T, n>& APoint<T, n>::operator %= ( const APoint<T, n> &p ) {
    GM_Static_<T, n>::xeq( getPtr(), p.getPtr());
    return *this;
  }

  template <typename T, int n>
  inline
  APoint<T, n> APoint<T, n>::operator % ( const APoint<T, n> &p ) const {
    APoint<T, n> r;
    GM_Static_<T, n>::eq_x( r.getPtr(), getPtr(), p.getPtr());
    return r;
  }

  template <typename T, int n>
  inline
  const APoint<T, n>& APoint<T, n>::operator /= ( double d ) {
    d = 1/d;
    *this *= d;
    return *this;
  }

  template <typename T, int n>
  inline
  APoint<T, n> APoint<T, n>::operator /  ( double d ) const {
    APoint<T,n> r;
    d = 1/d;
    r = (*this) * d;
    return r;
  }



  template <typename T, int n>
  inline
  bool APoint<T, n>::operator == ( const APoint<T, n> &p ) const {
    APoint<T,n> d=(*this)-p;
    return bool((d*d)<POS_TOLERANCE);
  }


  template <typename T, int n>
  inline
  bool APoint<T, n>::operator != ( const APoint<T, n> &pt ) const {
    return !((*this) == pt);
  }

  template <typename T, int n>
  inline
  bool APoint<T, n>::operator <  ( const APoint<T, n> &v ) const {
     switch(_sortType){
     case 1: return bool((_arrow->getPos()-(*this)).getLength() < (_arrow->getPos()-v).getLength());
     case 2: return bool((_arrow->getDir()*(*this)) <  (_arrow->getDir()*v));
     case 3: return bool(_arrow->getDir().getAngle((*this)-_arrow->getPos()) < _arrow->getDir().getAngle(v-_arrow->getPos()));
     default: return false;// Use a debug error-message
     }
  }

  template <typename T, int n>
  inline
  bool APoint<T, n>::operator >  ( const APoint<T, n> &v ) const {
     switch(_sortType){
     case 1: return bool((_arrow->getPos()-(*this)).getLength() > (_arrow->getPos()-v).getLength());
     case 2: return bool((_arrow->getDir()*(*this)) >  (_arrow->getDir()*v));
     case 3: return bool(_arrow->getDir().getAngle((*this)-_arrow->getPos()) > _arrow->getDir().getAngle(v-_arrow->getPos()));
     default: return false;// Use a debug error-message
     }
  }

  template <typename T, int n>
  inline
  bool APoint<T, n>::operator <= ( const APoint<T, n> &v ) const {
    return !((*this) > v);
  }

  template <typename T, int n>
  inline
  bool APoint<T, n>::operator >= ( const APoint<T, n> &v ) const {
    return !((*this) < v);
  }


  template <typename T, int n>
  template <typename G, int m>
  inline
  APoint<T, n>::operator APoint<G,m> () const {
    APoint<G,m> v;
    GM_Static1_<G,T,(n<m?n:m)>::eq( v.getPtr(), getPtr());
    return v;
  }


  /*!
   *  Returns a point converted to dimension n where all point elements is casted to G type.
   *
   *  \tparam G Convert-to Type
   *  \tparam m Convert-to Dimension
   *
   *  \return A point where all point elemets is of G type
   */
  template <typename T, int n>
  template <typename G, int m>
  inline
  APoint<G,m> APoint<T, n>::to() const {
    APoint<G,m> v;
    v = *this;
    return v;
  }


  /*!
   *  Returns a point where all point elements is casted to G type.
   *
   *  \tparam G Convert-to Type
   *
   *  \return A point where all point elemets is of G type
   */
  template <typename T, int n>
  template <typename G>
  inline
  APoint<G,n> APoint<T, n>::toType() const {
    return to<G,n>();
  }


  /*! void APoint<T, n>::_cpy( const APoint<T, n> &v )
   *  \brief Copys a point element data set.
   *
   *  Copys a point element data set.
   *  Takes a point-object as parameter,
   *  and uses memcpy to copy the data raw from the src point to the dest point (this).
   *
   *  \param[in] v The src APoint, from where the data is collected.
   */
  template <typename T, int n>
  inline
  void APoint<T, n>::_cpy( const APoint<T, n> &v ) {
    std::memcpy( _pt, v._pt, sizeof( APoint<T,n> ) );
  }


  /*! void APoint<T, n>::_cpy( const T p[n] )
   *  \brief Copys a point element data set.
   *
   *  Copys a point element data set.
   *  Takes a point element array
   *  and uses memcpy to copy the data raw from the src point to the dest point (this).
   *
   *  \param[in] p The src point element data set, from where the data is collected.
   */
  template <typename T, int n>
  inline
  void APoint<T, n>::_cpy( const T p[n] ) {
    memcpy( _pt, p, sizeof( APoint<T,n> ) );
  }


  /*! void APoint<T, n>::_cpy( const T &d )
   *  \brief Copys a point element data set.
   *
   *  Copys a point element data set.
   *  Takes a T value, and copys the value into every point element in the data set of the point object.
   *
   *  \param[in] d The value to be set in every point element.
   */
  template <typename T, int n>
  inline
  void APoint<T, n>::_cpy( const T &d ) {
    GM_Static_<T,n>::eq( getPtr(), d );
  }






  //*************************************
  //**  Special members of Point<T,2>  **
  //*************************************



  /*! Point<T,2>::Point<T,2>(const T& x,const T& y)
   *  \brief  Default Constructor
   *
   *  Detailed description of
   *  the default constructor
   */
  template <typename T>
  inline
  Point<T,2>::Point(const T& x,const T& y) {
    this->_pt[0]=x; this->_pt[1]=y;
  }


  /*! Point<T,2>::Point(const Point<T,3>& p)
   *  \brief  Default Constructor
   *
   *  Detailed description of
   *  the default constructor
   */
  template <typename T>
  inline
  Point<T,2>::Point(const APoint<T,3>& p) {
    this->_pt[0]=p(0); this->_pt[1]=p(1);
  }


  /*! int Point<T,2>::isInside(const Point<T,2>& v1,const Point<T,2>& v2,const Point<T,2>& v3) const
   *  \brief  Check if point is inside polygon
   *
   * See if this point is inside a closed polygon
   * If it is inside the return value is 1.
   * If it is on the edges the return value is -n,
   * where n is the nuber on the actual edge.
   * If it is outside the polygon the retun value is 0.
   */
  template <typename T>
  int Point<T,2>::isInside(const Array<Point<T,2> >& a) const{
    Array<int> v;
    double r;

    for (int i=0,j=1; i<a.size(); i++,j++) {
      if (j == a.size()) j = 0;
      UnitVector<T,2> b = a(j) - a(i);
      r = b^((*this) - a(i));
      if (r < - POS_TOLERANCE) return 0;
      if (std::abs(r) < POS_TOLERANCE) v += -i-1;
    }

    if (v.size() == 0)	return 1;
    else				return v[0];
  }


  /*! int Point<T,2>::isInside(const APoint<T,2>& v1,const APoint<T,2>& v2,const APoint<T,2>& v3) const
   *  \brief  Check if point is inside circle
   *
   * See if this point is inside a circle defined
   * by tree points on the circle.
   * If it is inside the circle the return value is true
   * else it is false.
   */
  template <typename T>
  int  Point<T,2>::isInsideCircle(const APoint<T,2>& p1,const APoint<T,2>& p2, const APoint<T,2>& p3) const {
    T b1 = p1*p1;
    T b2 = p2*p2;
    T b3 = p3*p3;

    Point<T,2> b(b2-b1,b3-b2);
    Point<T,2> a1 = p2 - p1;
    Point<T,2> a2 = p3 - p2;

    Point<T,2> c = (0.5/(a1^a2))*Point<T,2>(Point<T,2>(a2[1],-a1[1])*b,Point<T,2>(-a2[0],a1[0])*b);
    a1 = (*this) - c;
    a2 = p1 - c;

    T r = a1*a1;
    T s = a2*a2;

    if (std::fabs(r-s) < POS_TOLERANCE)	return -1;
    else if (r < s)					return  1;
    else							return  0;
  }


  /*! T Point2D<T>::operator^(const Point<T,2>& v) const
   *  \brief  Operator ^ Semi vector product.
   *
   *  Detailed description of
   *  the operator
   */
  template <typename T>
  inline
  T Point<T,2>::operator^(const APoint<T,2>& v) const {
    return this->_pt[0]*v(1) - this->_pt[1]*v(0);
  }

  /*! \brief  Return a vector 90 deg. to this. */
  template <typename T>
  inline
  APoint<T,2> Point<T,2>::getNormal() {
    return Point<T,2>(-this->_pt[1], this->_pt[0]);
  }

  template <typename T>
  inline
  int Point<T,2>::isInside(const APoint<T,2>& v1,const APoint<T,2>& v2,const APoint<T,2>& v3) const {
    Array<Point<T,2> > arr;
    arr+=v1;
    arr+=v2;
    arr+=v3;
    return isInside(arr);
  }


  //*************************************
  //**  Special members of Point<T,3>  **
  //*************************************


  /*! Point<T,3>::Point(const T& x,const T& y,const T& z)
   *  \brief  Default Constructor
   *
   *  Detailed description of
   *  the default constructor
   */
  template <typename T>
  inline
  Point<T,3>::Point(const T& x,const T& y,const T& z) {
    this->_pt[0]=x; this->_pt[1]=y; this->_pt[2]=z;
  }


  /*! Point<T,3>::Point(const Point<T,2>& p)
   *  \brief  Default Constructor
   *
   *  Detailed description of
   *  the default constructor
   */
  template <typename T>
  inline
  Point<T,3>::Point(const APoint<T,2>& p) {
    this->_pt[0]=p(0); this->_pt[1]=p(1); this->_pt[2]=T(0);
  }

  /*! Point<T,3> Point<T,3> ::operator^(const Point<T,3>& v)  const
   *  \brief  Vector product
   *
   *  Detailed description of
   *  the operator
   */
  template <typename T>
  inline
  APoint<T,3> Point<T,3>::operator^(const APoint<T,3>& v)  const {
    return Point<T,3>( this->_pt[1]*v(2) - this->_pt[2]*v(1),
                       this->_pt[2]*v(0) - this->_pt[0]*v(2),
                       this->_pt[0]*v(1) - this->_pt[1]*v(0) );
  }



  //*******************************
  //**     Members of Vector     **
  //*******************************


 /*! Vector<T,n>::Vector() : APoint<T, n>()
   *  \brief Default constructor, does not initalize the vector
   *
   *  Default constructor, does not initalize the vector.
   *  If the default constructor should be used, the vector needs to be initalized at a later point.
   */
//  template <typename T, int n>
//  inline
//  Vector<T,n>::Vector() : APoint<T,n>() {}

  /*! Vector<T, n>::Vector( T d ) : APoint<T, n>(d)
   *  \brief Constructor taking a value t of T
   *
   *  Constructor taking a value t of T.
   *  The vector will be initialzied with t in each axis.
   *
   *  \param[in] t The initialization value.
   */
//  template <typename T, int n>
//  inline
//  Vector<T, n>::Vector( T t ) : APoint<T,n>(t) {}

  /*! Vector<T, n>::Vector( const T *t ) : APoint<T, n>(t)
   *  \brief  Constructuer taking an array of n T values.
   *
   *  This constructor has been created to make the vector class
   *  compatible with other librarys and general datasets.
   *  The constructor presupposes that the input array has n T values.
   *
   * \param[in] t A const T pointer of n values.
   */
//  template <typename T, int n>
//  inline
//  Vector<T,n>::Vector( const T *t ) : APoint<T,n>(t) {}

  /*! Vector<T, n>::Vector( const APoint<T, n> &p ) : APoint<T, n>(p)
   *  \brief Default copy constructor
   *
   *  Default copy constructor.
   *  Taking a APoint or Vector as parameter.
   *
   *  \param[in] p APoint/Vector to copy from.
   */
//  template <typename T, int n>
//  inline
//  Vector<T,n>::Vector( const APoint<T,n> &p ) : APoint<T, n>(p) {}

  /*! APoint<T,n>& Vector<T, n>::getNormalized() const
   *  \brief Get the vector normalized.
   *
   *  Get the vector normalized.
   *
   *  \return A normalized Vector
   */

#define V_getnorm(n) inline\
  APoint<T,n> Vector<T,n>::getNormalized() const {\
    return (*this)/APoint<T,n>::getLength();\
  }

  template <typename T, int n>
  V_getnorm(n)

  template <typename T>
  V_getnorm(2)

  template <typename T>
  V_getnorm(3)

  template <typename T>
  V_getnorm(4)

  /*! Vector<T,n> Vector<T, n>::getLinIndVec() const
   *  \brief Get a linear independent vector to the vector
   *
   *  Get a linear independent vector to the vector
   *
   *  \return A linear independent vector
   */

#define V_getliniv(n) \
  Vector<T,n> Vector<T,n>::getLinIndVec() const {\
    if(n==1|| GMutils::compValueF(APoint<T,n>::getLength(),T(0))) return Vector<T,n>(T(0));\
    else\
    {\
      int i,j=0;\
      for(i=1; i < n;i++) if(( this->_pt[i]*this->_pt[i])>(this->_pt[j]*this->_pt[j])) j=i;\
      if(j==0) i = 1;\
      else	 i = j-1;\
      Vector<T,n> r = *this;\
      T tmp = -r[j];\
      r[j]  = r[i];\
      r[i]  = tmp;\
      r    -= ((r*(*this))/((*this)*(*this)))*(*this);\
      return r;\
    }\
  }

  template <typename T, int n>
  V_getliniv(n)

  template <typename T>
  V_getliniv(2)

  template <typename T>
  V_getliniv(3)

  template <typename T>
  V_getliniv(4)


  /*! APoint<T,n>& Vector<T, n>::normalize()
   *  \brief Normalize the vector
   *
   *  Normalize the vector
   */

#define V_getnormalize(n) inline\
  const APoint<T,n>& Vector<T,n>::normalize() {\
  (*this)/=APoint<T,n>::getLength();\
  return *this;\
  }

  template <typename T, int n>
  V_getnormalize(n)

  template <typename T>
  V_getnormalize(2)

  template <typename T>
  V_getnormalize(3)

  template <typename T>
  V_getnormalize(4)

  /*! void Vector<T, n>::setLength( T length )
   *  \brief Set the length of the vector
   *
   *  Set the length of the vector and adjust it accordingly
   *
   *  \param[in] length The new vector length
   */

#define V_setlength(n) inline\
  void Vector<T,n>::setLength( T length ) {\
    length /= APoint<T,n>::getLength();\
    (*this) *= length;\
  }

  template <typename T, int n>
  V_setlength(n)

  template <typename T>
  V_setlength(2)

  template <typename T>
  V_setlength(3)

  template <typename T>
  V_setlength(4)



  //*************************************
  //** Specific members of Vector<T,2> **
  //*************************************



  /*! Vector<T,2>::Vector<T,2>( const APoint<T,3> &p)
   *  \brief  Default Constructor
   *
   *  Detailed description of
   *  the default constructor
   */
  template <typename T>
  inline
  Vector<T,2>::Vector( const APoint<T,3> &p) {
      this->_pt[0]=p(0); this->_pt[1]=p(1);
  }

  template <typename T>
  inline
  Vector<T,2>::Vector( const APoint<T,4> &p) {
      this->_pt[0]=p(0); this->_pt[1]=p(1);
  }

  /*! Vector<T,2>::Vector<T,2>(const T& x,const T& y)
   *  \brief  Default Constructor
   *
   *  Detailed description of
   *  the default constructor
   */
  template <typename T>
  inline
  Vector<T,2>::Vector( const T& x, const T& y) {
      this->_pt[0]=x; this->_pt[1]=y;
  }



  /*! T Vector<T,2>::operator^(const Point<T,2>& v) const
   *  \brief  Semi vector product.
   *
   *  Detailed description of
   *  the default constructor
   */
  template <typename T>
  T Vector<T,2>::operator^(const APoint<T,2>& v) const {
    return this->_pt[0]*v(1) - this->_pt[1]*v(0);
  }


  /*! APoint<T,2>  Vector<T,2>:getNormal()
   *  \brief  Return a vector 90 deg. to this.
   *
   *  Detailed description of
   *  the default constructor
   */
  template <typename T>
  inline
  APoint<T,2>  Vector<T,2>::getNormal() const {
      return Point<T,2>( - this->_pt[1], this->_pt[0] );
  }



  //*************************************
  //** Specific members of Vector<T,3> **
  //*************************************


  /*! Vector<T,3>::Vector<T,3>( const APoint<T,2> &p)
   *  \brief  Default Constructor
   *
   *  Detailed description of
   *  the default constructor
   */
  template <typename T>
  inline
  Vector<T,3>::Vector( const APoint<T,2> &p) {
      this->_pt[0]=p(0); this->_pt[1]=p(1); this->_pt[2]=T(0);
  }

  template <typename T>
  inline
  Vector<T,3>::Vector( const APoint<T,4> &p) {
      this->_pt[0]=p(0); this->_pt[1]=p(1); this->_pt[2]=p(2);
  }


  /*! Vector<T,3>::Vector<T,3>(const T& x,const T& y,const T& z)
   *  \brief  Default Constructor
   *
   *  Detailed description of
   *  the default constructor
   */
  template <typename T>
  inline
  Vector<T,3>::Vector( const T& x, const T& y, const T& z) {
      this->_pt[0]=x; this->_pt[1]=y; this->_pt[2]=z;
  }



  /*! APoint<T,3> Vector<T,3>::operator^(const APoint<T,3>& v) const
   *  \brief  Vector product
   *
   *  Detailed description of
   *  the operator
   */
  template <typename T>
  inline
  APoint<T,3> Vector<T,3>::operator^(const APoint<T,3>& v) const {
    return Vector<T,3>(
      this->_pt[1]*v(2) - this->_pt[2]*v(1),
      this->_pt[2]*v(0) - this->_pt[0]*v(2),
      this->_pt[0]*v(1) - this->_pt[1]*v(0)
    );
  }



  //*************************************
  //** Specific members of Vector<T,4> **
  //*************************************


  template <typename T>
  inline
  Vector<T,4>::Vector( const APoint<T,2> &p) {
      this->_pt[0]=p(0); this->_pt[1]=p(1); this->_pt[2]=T(0); this->_pt[3]=T(0);
  }

  template <typename T>
  inline
  Vector<T,4>::Vector( const APoint<T,3> &p) {
      this->_pt[0]=p(0); this->_pt[1]=p(1); this->_pt[2]=p(2); this->_pt[3]=T(0);
  }

  template <typename T>
  inline
  Vector<T,4>::Vector( const T& x1, const T& x2, const T& x3, const T& x4) {
      this->_pt[0]=x1; this->_pt[1]=x2; this->_pt[2]=x3; this->_pt[3]=x4;
  }


  //****************************************************************
  //******  The UnitVector class, constructors and functions  ******
  //****************************************************************
  //****************************************************************


#define UV_conc(n) inline\
  UnitVector<T,n>::UnitVector( T t ) : Vector<T,n>(t) {\
    APoint<T,n>::operator/=(APoint<T,n>::getLength());\
  }

  template <typename T, int n>
  UV_conc(n)

  template <typename T>
  UV_conc(2)

  template <typename T>
  UV_conc(3)


#define UV_conc0(n) inline\
  UnitVector<T,n>::UnitVector( const T t[n] ) : Vector<T,n>(t) {\
    APoint<T,n>::operator/=( APoint<T,n>::getLength() );\
  }

  template <typename T, int n>
  UV_conc0(n)

  template <typename T>
  UV_conc0(2)

  template <typename T>
  UV_conc0(3)


#define UV_conc1(n) inline\
  UnitVector<T,n>::UnitVector( const APoint<T,n> &p ) : Vector<T,n>(p) {\
    APoint<T,n>::operator/=( APoint<T,n>::getLength() );\
  }

  template <typename T, int n>
  UV_conc1(n)

  template <typename T>
  UV_conc1(2)

  template <typename T>
  UV_conc1(3)


  /*! UnitVector<T,2>::UnitVector<T,2>(const T& x,const T& y)
   *  \brief  Default Constructor
   *
   *  Detailed description of
   *  the default constructor
   */
  template <typename T>
  inline
  UnitVector<T,2>::UnitVector( const T& x, const T& y): Vector<T,2>(x,y) {}


  /*! UnitVector<T,3>::UnitVector<T,3>(const T& x,const T& y,const T& z)
   *  \brief  Default Constructor
   *
   *  Detailed description of
   *  the default constructor
   */
  template <typename T>
  inline
  UnitVector<T,3>::UnitVector( const T& x, const T& y, const T& z): Vector<T,3>(x,y,z) {}


#define UV_conc2(n) inline\
  UnitVector<T, n>::UnitVector( const UnitVector<T, n> &uv ) : Vector<T, n>(uv) {}

  template <typename T, int n>
  UV_conc2(n)

  template <typename T>
  UV_conc2(2)

  template <typename T>
  UV_conc2(3)


#define UV_eq0(n) inline\
  const APoint<T, n>& UnitVector<T, n>::operator = ( const T t ) {\
    _cpy(t);\
    return APoint<T,n>::operator/=( APoint<T,n>::getLength() );\
  }

  template <typename T, int n>
  UV_eq0(n)

  template <typename T>
  UV_eq0(2)

  template <typename T>
  UV_eq0(3)


#define UV_eq1(n) inline\
  const APoint<T, n>& UnitVector<T, n>::operator = ( const T t[n] ) {\
    _cpy(t);\
    return APoint<T,n>::operator/=( APoint<T,n>::getLength() );\
  }

  template <typename T, int n>
  UV_eq1(n)

  template <typename T>
  UV_eq1(2)

  template <typename T>
  UV_eq1(3)


#define UV_eq2(n) inline\
  const APoint<T, n>& UnitVector<T, n>::operator = ( const APoint<T, n> &p ) {\
    this->_cpy(p);\
    return APoint<T,n>::operator/=( APoint<T,n>::getLength() );\
  }

  template <typename T, int n>
  UV_eq2(n)

  template <typename T>
  UV_eq2(2)

  template <typename T>
  UV_eq2(3)


#define UV_eq3(n) inline\
  const APoint<T, n>& UnitVector<T, n>::operator = ( const UnitVector<T, n>& uv )	{\
    this->_cpy(uv);\
    return *this;\
  }

  template <typename T, int n>
  UV_eq3(n)

  template <typename T>
  UV_eq3(2)

  template <typename T>
  UV_eq3(3)


#define UV_op1(n) inline\
  const T& UnitVector<T, n>::operator [] ( int i ) {\
    return APoint<T,n>::_pt[i];\
  }

  template <typename T, int n>
  UV_op1(n)

  template <typename T>
  UV_op1(2)

  template <typename T>
  UV_op1(3)


#define UV_pe(n) inline\
  const APoint<T,n>& UnitVector<T, n>::operator += ( const APoint<T, n> &p ) {\
    APoint<T,n>::operator += (p);\
    return APoint<T,n>::operator/=( APoint<T,n>::getLength() );\
  }

  template <typename T, int n>
  UV_pe(n)

  template <typename T>
  UV_pe(2)

  template <typename T>
  UV_pe(3)


#define UV_me(n) inline\
  const APoint<T,n>& UnitVector<T, n>::operator -= ( const APoint<T, n> &p ) {\
    APoint<T,n>::operator-=(p);\
    return APoint<T,n>::operator/=( APoint<T,n>::getLength() );\
  }

  template <typename T, int n>
  UV_me(n)

  template <typename T>
  UV_me(2)

  template <typename T>
  UV_me(3)


#define UV_pre(n) inline\
  const APoint<T,n>& UnitVector<T, n>::operator %= ( const APoint<T, n> &p ) {\
    APoint<T,n>::operator%=(p); return APoint<T,n>::operator/=( APoint<T,n>::getLength() );\
  }

  template <typename T, int n>
  UV_pre(n)

  template <typename T>
  UV_pre(2)

  template <typename T>
  UV_pre(3)


#define UV_ge(n) inline\
  const APoint<T,n>& UnitVector<T, n>::operator *= ( const double /*d*/ ) {\
    return *this;\
  }

  template <typename T, int n>
  UV_ge(n)

  template <typename T>
  UV_ge(2)

  template <typename T>
  UV_ge(3)


#define UV_de(n) inline\
  const APoint<T,n>& UnitVector<T, n>::operator /= ( double /*d*/ ) {\
    return *this;\
  }

  template <typename T, int n>
  UV_de(n)

  template <typename T>
  UV_de(2)

  template <typename T>
  UV_de(3)


  //********************************************************************
  //******  The UnitQuaternion class, constructors and functions  ******
  //********************************************************************
  //********************************************************************

  template <typename T>
  inline
  UnitQuaternion<T>::UnitQuaternion() : Quaternion<T>() {}

  template <typename T>
  inline
  UnitQuaternion<T>::UnitQuaternion( const T t[4] ) : Quaternion<T>(t) {
    APoint<T,4>::operator /= ( APoint<T,4>::getLength() );
  }

  template <typename T>
  inline
  UnitQuaternion<T>::UnitQuaternion(const T& q0, const T& q1, const T& q2, const T& q3) :
    Quaternion<T>( q0, q1, q2, q3 ) {

    APoint<T,4>::operator /= ( APoint<T,4>::getLength() );
  }

  template <typename T>
  inline
  UnitQuaternion<T>::UnitQuaternion( const UnitQuaternion& uq ) : Quaternion<T>(uq) {}

  template <typename T>
  inline
  UnitQuaternion<T>::UnitQuaternion( const APoint<T,4> &p ) : Quaternion<T>(p) {
    APoint<T,4>::operator /= ( APoint<T,4>::getLength() );
  }

  template <typename T>
  inline
  const APoint<T,4>& UnitQuaternion<T>::operator =  ( const T t ) {
    _cpy(t);
    return APoint<T,4>::operator/=( APoint<T,4>::getLength() );
  }

  template <typename T>
  inline
  const APoint<T,4>& UnitQuaternion<T>::operator =  ( const T t[3] ) {
    _cpy(t);
    return APoint<T,4>::operator/=( APoint<T,4>::getLength() );
  }

  template <typename T>
  inline
  const APoint<T,4>& UnitQuaternion<T>::operator =  ( const APoint<T,3> &p ) {
    _cpy(p);
    return APoint<T,4>::operator/=( APoint<T,4>::getLength() );
  }

  template <typename T>
  inline
  const APoint<T,4>& UnitQuaternion<T>::operator =  ( const UnitQuaternion<T>& uv ) {
    _cpy(uv);
    return *this;
  }

  template <typename T>
  inline
  const T& UnitQuaternion<T>::operator [] ( int i ) {
    return APoint<T,4>::_pt[i];
  }

  template <typename T>
  inline
  const APoint<T,4>& UnitQuaternion<T>::operator += ( const APoint<T,4> &p ) {
    _cpy(p);
    return APoint<T,4>::operator/=( APoint<T,4>::getLength() );
  }

  template <typename T>
  inline
  const APoint<T,4>& UnitQuaternion<T>::operator -= ( const APoint<T,4> &p ) {
    _cpy(p);
    return APoint<T,4>::operator/=( APoint<T,4>::getLength() );
  }

  template <typename T>
  inline
  const APoint<T,4>& UnitQuaternion<T>::operator %= ( const APoint<T,4> &p ) {
    _cpy(p);
    return APoint<T,4>::operator/=( APoint<T,4>::getLength() );
  }

  template <typename T>
  inline
  const APoint<T,4>& UnitQuaternion<T>::operator *= ( const double /*d*/ ) {
    return *this;
  }

  template <typename T>
  inline
  const APoint<T,4>& UnitQuaternion<T>::operator /= ( double /*d*/ ) {
    return *this;
  }



  //***********************************************************
  //******  The Arrow class, constructors and functions  ******
  //***********************************************************
  //***********************************************************


  template <typename T, int n>
  inline
  Arrow<T, n>::Arrow() : Point<T, n>()	{
    _dir = Vector<T,n>(1);
  }

  template <typename T, int n>
  inline
  Arrow<T, n>::Arrow( const APoint<T, n> &p ) : Point<T, n>(p) {
    _dir = Vector<T,n>(1);
  }

  template <typename T, int n>
  inline
  Arrow<T, n>::Arrow( const APoint<T, n> &p, const Vector<T, n> &v ) : Point<T,n>(p) {
    _dir = v;
  }

  template <typename T, int n>
  inline
  Arrow<T, n>::Arrow( const Arrow<T, n> &a ) : Point<T, n>(a) {
    _dir = a._dir;
  }

  template <typename T, int n>
  inline
  const Vector<T, n>& Arrow<T, n>::getDir() const {
    return _dir;
  }

  template <typename T, int n>
  inline
  const APoint<T, n>&  Arrow<T, n>::getPos() const {
    return (*this);
  }

  template <typename T, int n>
  inline
  void Arrow<T, n>::setDir( const Vector<T, n> &v ) {
    _dir = v;
  }

  template <typename T, int n>
  inline
  void Arrow<T, n>::setPos( const APoint<T, n> &v ) {
    this->_cpy(v);
  }

  template <typename T, int n>
  inline
  const Arrow<T, n>& Arrow<T, n>::operator = ( const Arrow<T, n> &a ) {
    memcpy( APoint<T, n>::getPtr(), a.getPtr(), sizeof( Arrow<T, n> ) );
    return *this;
  } // setPos(v.pos()); setDir(v.dir()); return *this;}

  template <typename T, int n>
  inline
  const Arrow<T, n>& Arrow<T, n>::operator += ( const Point<T, n> &p ) {
    APoint<T, n>::operator += (p);
    return *this;
  }

  template <typename T, int n>
  inline
  const Arrow<T, n>& Arrow<T, n>::operator -= ( const Point<T, n> &p ) {
    APoint<T,n>::operator-=(p);
    return *this;
  }

  template <typename T, int n>
  inline
  Arrow<T, n> Arrow<T, n>::operator + ( const Point<T, n> &p ) const {
    //APoint<T, n> r = (*this);
    Arrow<T, n> r = (*this);
    return r += p;
  }

  template <typename T, int n>
  inline
  Arrow<T, n> Arrow<T, n>::operator - ( const Point<T, n> &p ) const {
    //APoint<T,n> r = (*this);
    Arrow<T,n> r = (*this);
    return r -= p;
  }

  template <typename T, int n>
  inline
  const Arrow<T, n>& Arrow<T, n>::operator += ( const Vector<T, n> &v ) {
    _dir += v;
    return (*this);
  }

  template <typename T, int n>
  inline
  const Arrow<T, n>& Arrow<T, n>::operator -= ( const Vector<T, n> &v ) {
    _dir -= v;
    return (*this);
  }

  template <typename T, int n>
  inline
  Arrow<T, n> Arrow<T, n>::operator + ( const Vector<T, n> &v ) const {
    //APoint<T, n> r = (*this);
    Arrow<T, n> r = (*this);
    return r += v;
  }

  template <typename T, int n>
  inline
  Arrow<T, n> Arrow<T, n>::operator - ( const Vector<T, n> &v ) const {
    //APoint<T, n> r = (*this);
    Arrow<T, n> r = (*this);
    return r -= v;
  }

  template <typename T, int n>
  inline
  Arrow<T, n> Arrow<T, n>::operator - () const {
    APoint<T, n> p = -(*this);
    return p;
  }

  template <typename T, int n>
  template <class G, int m>
  inline
  Arrow<T, n>::operator Arrow<G, m> () const {
    Arrow<G,m> v;
    GM_Static1_<G,T,(n<m?n:m)>::eq( v.getPtr(), APoint<T, n>::getPtr());
    GM_Static1_<G,T,(n<m?n:m)>::eq( v.getDir().getPtr(), _dir.getPtr());
    return v;
  }

  template <typename T, int n>
  inline
  Arrow<float, n> Arrow<T, n>::toFloat() const {
    Arrow<float,n> v;
    GM_Static1_<float,T,n>::eq( v.getPtr(), APoint<T, n>::getPtr() );
    GM_Static1_<float,T,n>::eq( v.getDir().getPtr(), APoint<T, n>::getPtr() );
    return v;
  }

  template <typename T, int n>
  inline
  Arrow<double, n> Arrow<T, n>::toDouble() const {
    Arrow<double,n> v;
    GM_Static1_<double,T,n>::eq( v.getPtr(), APoint<T, n>::getPtr());
    GM_Static1_<double,T,n>::eq( v.getDir().getPtr(), APoint<T, n>::getPtr());
    return v;
  }




  //******************************************************************
  //******  The ScalarAPoint class, constructors and functions  ******
  //******************************************************************
  //******************************************************************

  template <typename T, int n>
  inline
  ScalarPoint<T, n>::ScalarPoint() : _pos( T(0) ), _value( T(0) ) {}


  template <typename T, int n>
  inline
  ScalarPoint<T, n>::ScalarPoint( const APoint<T, n>& p, T v )	{
    _pos = p;
    _value = v;
  }

  template <typename T, int n>
  inline
  ScalarPoint<T, n>::ScalarPoint( const ScalarPoint<T, n>& s )			{
    _pos = s._pos;
    _value = s._value;
  }

  template <typename T, int n>
  inline
  const APoint<T, n>& ScalarPoint<T, n>::getPos() const {
    return _pos;
  }

  template <typename T, int n>
  inline
  T* ScalarPoint<T, n>::getPtr() {
    return reinterpret_cast<T*>(this);
  }

  template <typename T, int n>
  inline
  const T* ScalarPoint<T, n>::getPtr() const {
    return reinterpret_cast<const T*>(this);
  }

  template <typename T, int n>
  inline
  T ScalarPoint<T, n>::getValue() const {
    return _value;
  }

  template <typename T, int n>
  inline
  void ScalarPoint<T, n>::reset( const APoint<T, n>& p , T v )	{
    _pos = p;
    _value = v;
  }

  template <typename T, int n>
  inline
  void ScalarPoint<T, n>::reset() {
    _pos = T(0);
    _value = T(0);
  }

  template <typename T, int n>
  inline
  void ScalarPoint<T, n>::resetValue( T t ) {
    _value = t;
  }

  template <typename T, int n>
  inline
  void ScalarPoint<T, n>::resetPos( const APoint<T, n>& p ) {
    _pos = p;
  }

  template <typename T, int n>
  inline
  const ScalarPoint<T, n>& ScalarPoint<T, n>::operator += ( const APoint<T, n>& p ) {
    _pos += p;
    return *this;
  }

  template <typename T, int n>
  inline
  ScalarPoint<T, n>  ScalarPoint<T, n>::operator +  ( const APoint<T, n>& p ) const	{
    ScalarPoint<T,n> a=(*this);
    a+=p;
    return a;
  }

  template <typename T, int n>
  inline
  const ScalarPoint<T, n>& ScalarPoint<T, n>::operator += ( T p ) {
    _value += p;
    return *this;
  }

  template <typename T, int n>
  inline
  ScalarPoint<T, n>  ScalarPoint<T, n>::operator +  ( T p ) const {
    ScalarPoint<T,n> a=(*this);
    a+=p;
    return a;
  }

  template <typename T, int n>
  inline
  const ScalarPoint<T, n>& ScalarPoint<T, n>::operator += ( const ScalarPoint<T, n>& p ) {
    _pos += p._pos;
    _pos /= 2.0;
    _value += p._value;
    _value /= 2.0;
    return *this;
  }

  template <typename T, int n>
  inline
  ScalarPoint<T, n>  ScalarPoint<T, n>::operator +  ( const ScalarPoint<T, n>& p ) const {
    ScalarPoint<T,n> a=(*this);
    a+=p;
    return a;
  }

  template <typename T, int n>
  inline
  const ScalarPoint<T, n>& ScalarPoint<T, n>::operator *= ( double d ) {
    _value *= d;
    return *this;
  }

  template <typename T, int n>
  inline
  ScalarPoint<T, n>  ScalarPoint<T, n>::operator *  ( double d ) const {
    ScalarPoint<T,n> a=(*this);
    a*=d;
    return a;
  }

  template <typename T, int n>
  inline
  const ScalarPoint<T, n>& ScalarPoint<T, n>::operator /= ( double d ) {
    _value /= d;
    return *this;
  }

  template <typename T, int n>
  inline
  ScalarPoint<T, n>  ScalarPoint<T, n>::operator /  ( double d ) const {
    ScalarPoint<T,n> a=(*this);
    a/=d;
    return a;
  }

  template <typename T, int n>
  inline
  const ScalarPoint<T, n>& ScalarPoint<T, n>::operator %= ( const APoint<T, n>& p ) {
    _pos %= p;
    return *this;
  }

  template <typename T, int n>
  inline
  ScalarPoint<T, n>  ScalarPoint<T, n>::operator %  ( const APoint<T, n>& p ) const	{
    ScalarPoint<T,n> a=(*this);
    a%=p;
    return a;
  }

  template <typename T, int n>
  template <typename G, int m>
  inline
  ScalarPoint<T,n>::operator ScalarPoint<G,m> () const {
    ScalarPoint<G,m> v;
    v.resetPos(this->_pos);
    v.resetValue(this->_value);
    return v;
  }

  template <typename T, int n>
  template <typename G, int m>
  inline
  ScalarPoint<G,m> ScalarPoint<T, n>::to() const {
    ScalarPoint<G,m> v;
    v = *this;
    return v;
  }

  template <typename T, int n>
  template <typename G>
  inline
  ScalarPoint<G,n> ScalarPoint<T, n>::toType() const {
    return to<G,n>();
  }





  //*********************************************************
  //*****  The Sphere class, constructors and functions  ****
  //*********************************************************
  //*********************************************************

  template <typename T, int n>
  inline
  Sphere<T, n>::Sphere( bool valid ) : ScalarPoint<T,n>() {
    _valid = valid;
  }

  template <typename T, int n>
  inline
  Sphere<T, n>::Sphere( const APoint<T, n>& p, T v ) : ScalarPoint<T,n>(p,v)	{
    _valid = true;
  }

  template <typename T, int n>
  inline
  Sphere<T, n>::Sphere( const ScalarPoint<T, n>& s ) : ScalarPoint<T,n>(s) {
    _valid = true;
  }

  template <typename T, int n>
  inline
  Sphere<T, n>::Sphere( const Sphere<T, n>& s ) : ScalarPoint<T,n>(s) {
    _valid = s._valid;
  }

  template <typename T, int n>
  inline
  T	Sphere<T, n>::getRadius() const	{
    return this->_value;
  }

  template <typename T, int n>
  inline
  bool Sphere<T, n>::isValid() const {
    return _valid;
  }

  template <typename T, int n>
  bool Sphere<T, n>::isIntersecting(const Sphere<T,n>& p) const {
    if(ScalarPoint<T, n>::_valid && p._valid)
    {
      T d = (this->_pos - p._pos).getLength();
      return (this->_value + p._value) > d;
    }
    return false;
  }

  template <typename T, int n>
  inline
  void Sphere<T, n>::resetPos( const APoint<T, n>& p )	{
    _valid = true;
    ScalarPoint<T,n>::resetPos(p);
  }

  template <typename T, int n>
  inline
  void Sphere<T, n>::resetRadius( T t ) {
    this->resetValue(t);
  }

  template <typename T, int n>
  inline
  void Sphere<T, n>::reset() {
    _valid = false;
    ScalarPoint<T,n>::reset();
  }

  template <typename T, int n>
  const Sphere<T, n>& Sphere<T, n>::operator += ( const APoint<T, n>& p ) {
    if(_valid)
    {
      Vector<T,n> v = p - this->_pos;
      T r, d = v.getLength();

      if (d > this->_value)
      {
        r		      = (d+this->_value)/2;
        this->_pos   += ((r-this->_value)/d)*v;
        this->_value  = r;
      }
    }
    else
    {
      this->_pos   = p;
      this->_value = T(0);
      _valid = true;
    }

    return *this;
  }

  template <typename T, int n>
  inline
  Sphere<T, n> Sphere<T, n>::operator +  ( const APoint<T, n>& p ) const {

    Sphere<T, n> a = (*this);
    a += p;
    return a;
  }

  template <typename T, int n>
  const Sphere<T, n>& Sphere<T, n>::operator += ( const Sphere<T, n>& p ) {

    if(p._valid)
    {
      if(_valid)
      {
        Vector<T,n> v =  p._pos - this->_pos;
        T r, d = v.getLength();
        if(this->_value > p._value)
        {
          if (d > this->_value - p._value)
          {
            r		     = (d + this->_value + p._value)/2;
            this->_pos  += ((r-this->_value)/d)*v;
            this->_value = r;
          }
        }
        else
        {
          if (d > p._value - this->_value)
          {
            r		= (d + this->_value + p._value)/2;
            this->_pos   += ((r - this->_value)/d)*v;
            this->_value	= r;
          }
          else
            *this = p;
        }
      }
      else
        *this  = p;
    }
    return *this;
  }

  template <typename T, int n>
  inline
  Sphere<T, n>  Sphere<T, n>::operator +  ( const Sphere<T, n>& p ) const {
    Sphere<T,n> a=(*this);
    a+=p;
    return a;
  }

  template <typename T, int n>
  template <typename G, int m>
  inline
  Sphere<T,n>::operator Sphere<G,m> () const {
    return ScalarPoint<T,n>::operator ScalarPoint<G,m>& ();
  }


  //*********************************************************
  //******  The Box class, constructors and functions  ******
  //*********************************************************
  //*********************************************************

  template <typename T, int n>
  inline
  Box<T, n>::Box() : _min{T(0)}, _max{T(0)} {}

  template <typename T, int n>
  inline
  Box<T, n>::Box( const APoint<T, n>& p ) : _min{p}, _max{p} {}

  template <typename T, int n>
  inline
  Box<T, n>::Box( const Box<T, n>& b ) : _min{b._min}, _max{b._max} {}

  template <typename T, int n>
  inline
  Box<T, n>::Box( const APoint<T, n>& p1, const APoint<T, n>& p2 )	{
    _min = _max = p1;
    insert(p2);
  }

  template <typename T, int n>
  inline
  Box<T, n>::Box( const APoint<T, n>& p1, const APoint<T, n>& p2, const APoint<T,n>& p3 ) {
    _min = _max = p1;
    insert(p2);
    insert(p3);
  }

  template <typename T, int n>
  inline
  APoint<T, n>	Box<T, n>::getPointMin() const {
    return _min;
  }

  template <typename T, int n>
  inline
  APoint<T, n>	Box<T, n>::getPointMax() const {
    return _max;
  }

  template <typename T, int n>
  inline
  APoint<T, n>	Box<T, n>::getPointCenter() const {
    return (_max + _min)*0.5;
  }

  template <typename T, int n>
  inline
  Vector<T, n>	Box<T, n>::getPointDelta() const {
    return _max - _min;
  }

  template <typename T, int n>
  inline
  T* Box<T, n>::getPtr() const {
    return static_cast<T*>(this);
  }

  template <typename T, int n>
  inline
  T& Box<T, n>::getValueAt( int i, int j ) {
    return i == 0 ? _min[j] : _max[j];
  }

  template <typename T, int n>
  inline
  T Box<T, n>::getValueMin( int i ) const	{
    return _min(i);
  }

  template <typename T, int n>
  inline
  T Box<T, n>::getValueMax( int i ) const	{
    return _max(i);
  }

  template <typename T, int n>
  inline
  T Box<T, n>::getValueCenter( int i ) const {
    return (_max(i) + _min(i))*0.5;
  }

  template <typename T, int n>
  inline
  T Box<T, n>::getValueDelta( int i ) const {
    return _max(i) - _min(i);
  }

  template <typename T, int n>
  void Box<T, n>::insert( const APoint<T, n>& p) {
    for(int i=0;i<n;i++)
    {
      if(p(i) < _min[i]) _min[i] = p(i);
      if(p(i) > _max[i]) _max[i] = p(i);
    }
  }


  template <typename T, int n>
  void Box<T, n>::insert( const Box<T, n>& b) {
    for(int i=0;i<n;i++)
    {
      if(b._min(i) < _min[i]) _min[i] = b._min(i);
      if(b._max(i) > _max[i]) _max[i] = b._max(i);
    }
  }

  template <typename T, int n>
  bool Box<T, n>::isIntersecting( const Box<T,n>& b ) const {
    for(int i=0;i<n;i++)
    {
      if(b._min(i) > _max(i)) return false;
      if(b._max(i) < _min(i)) return false;
    }
    return true;
  }

  template <typename T, int n>
  bool Box<T, n>::isSurrounding( const APoint<T,n>& p ) const {
    for(int i=0;i<n;i++)
    {
      if(p(i) < _min(i)) return false;
      if(p(i) > _max(i)) return false;
    }
    return true;
  }

  template <typename T, int n>
  bool Box<T, n>::isSurrounding( const Box<T,n>& b )  const {
    for(int i=0;i<n;i++)
    {
      if(b._min(i) < _min(i)) return false;
      if(b._max(i) > _max(i)) return false;
    }
    return true;
  }

  template <typename T, int n>
  inline
  void Box<T, n>::reset() {
    reset( APoint<T, n>( T(0) ) );
  }

  template <typename T, int n>
  inline
  void Box<T, n>::reset( const APoint<T, n>& p ) {
    _min = _max = p;
  }

  template <typename T, int n>
  inline
  const Box<T,n>&	Box<T, n>::operator += ( const APoint<T, n>& p ) {
    insert(p);
    return *this;
  }

  template <typename T, int n>
  inline
  Box<T,n> Box<T, n>::operator +  ( const APoint<T, n>& p ) {
    Box<T,n> a=(*this);
    a+=p;
    return a;
  }

  template <typename T, int n>
  inline
  const Box<T,n>&	Box<T, n>::operator += ( const Box<T, n>& b ) {
    insert(b);
    return *this;
  }

  template <typename T, int n>
  inline
  Box<T,n> Box<T, n>::operator +  ( const Box<T, n>& b ) {
    Box<T,n> a=(*this);
    a+=b;
    return a;
  }




  //*********************************************************
  //*** The PlaneArrow class, constructors and functions  ***
  //*********************************************************
  //*********************************************************


  /*! PlaneArrow<T,n>:: PlaneArrow()
   *  \brief  Default Constructor
   *
   *  Detailed description of
   *  the default constructor
   */
  template <typename T, int n>
  inline
  PlaneArrow<T,n>::PlaneArrow() : Arrow<T,n>() {
    this->_dir.normalize();
  }

  /*! PlaneArrow<T,n>:: PlaneArrow()
   *  \brief  Default Constructor
   *
   *  Detailed description of
   *  the default constructor
   */
  template <typename T, int n>
  inline
  PlaneArrow<T,n>::PlaneArrow(const Point<T,n>& p) : Arrow<T,n>(p) {
    this->_dir.normalize();
  }

  /*! PlaneArrow<T,n>:: PlaneArrow()
   *  \brief  Default Constructor
   *
   *  Detailed description of
   *  the default constructor
   */
  template <typename T, int n>
  inline
  PlaneArrow<T,n>::PlaneArrow(const Point<T,n>& p ,const Vector<T,n>& v) : Arrow<T,n>(p,UnitVector<T,n>(v)) {
  }

  /*! PlaneArrow<T,n>:: PlaneArrow()
   *  \brief  Default Constructor
   *
   *  Detailed description of
   *  the default constructor
   */
  template <typename T, int n>
  inline
  PlaneArrow<T,n>::PlaneArrow(const Arrow<T,n>& a) : Arrow<T,n>(a) {
  }

  /*! void PlaneArrow<T,n>::setNormal(const Vector<T,n>& v)
   *  \brief  setNormal
   *
   *  Detailed description of
   *  the function
   */
  template <typename T, int n>
  inline
  void PlaneArrow<T,n>::setNormal(const Vector<T,n>& v) {
    this->setDir(UnitVector<T,n>(v));
  }

  /*! const Vector<T,3>&	PlaneArrow<T,n>::getNormal()
   *  \brief  getNormal
   *
   *  \return normal
   *  Detailed description of
   *  the function
   */
  template <typename T, int n>
  inline
  const Vector<T,n>&	PlaneArrow<T,n>::getNormal() const {
    return this->getDir();
  }

  /*! \brief Computes closest point
   *
   *  \param[in] p The point one computes the closest point
   *  \return The closest point
   */
  template <typename T, int n>
  inline
  APoint<T,n> PlaneArrow<T,n>::getClosestPoint(const Point<T,n>& p) const {

    Vector<T,n> d = p - this->getPos();
    return this->getPos() + d - (d*this->getNormal())*this->getNormal();
  }

  /*! Vector<T,n> PlaneArrow<T,n>::getDistanceVector(const Point<T,n>& p) const
   *  \brief  getDistanceVector
   *
   *  \return distanceVector
   *  Detailed description of
   *  the function
   */
  template <typename T, int n>
  inline
  Vector<T,n> PlaneArrow<T,n>::getDistanceVector(const Point<T,n>& p) const {

    Vector<T,n> d = p - this->getPos();
    return (d*this->getNormal())*this->getNormal();
  }

  /*! T PlaneArrow<T,n>::getDistanceTo(const Point<T,3>& p) const
   *  \brief  getDistanceTo
   *
   *  \return distanceTo
   *  Detailed description of
   *  the function
   */
  template <typename T, int n>
  inline
  T PlaneArrow<T,n>::getDistanceTo(const Point<T,n>& p) const {

    return (p - this->getPos())*this->getNormal();
  }




  //*********************************************************
  //********  M_I_ is a help class for I-matrix init ********
  //********   This is a constructor and a function  ********
  //************   NOT FOR EXTERNAL USE !!!!!!   ************
  //*********************************************************


  template <typename T, int n, int m>
  M_I_<T, n, m>::M_I_() {
    for(int k=0,i=0; i<n; i++)
      for(int j=0; j<m; j++)
        if(i==j)	_p[k++] = T(1);
        else _p[k++] = T(0);
  }

  template <typename T, int n, int m>
  inline
  const T* M_I_<T, n, m>::getPtr() const {
    return reinterpret_cast<const T*>(this);
  }

} // END namespace GMlib


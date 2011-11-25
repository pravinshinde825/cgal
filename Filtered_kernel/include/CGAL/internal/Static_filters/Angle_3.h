// Copyright (c) 2011 GeometryFactory Sarl (France)
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
// See the file LICENSE.LGPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
//
//
// Author(s)     : Laurent Rineau


#ifndef CGAL_INTERNAL_STATIC_FILTERS_ANGLE_3_H
#define CGAL_INTERNAL_STATIC_FILTERS_ANGLE_3_H

#include <CGAL/Bbox_3.h>
#include <CGAL/Profile_counter.h>
#include <CGAL/internal/Static_filters/Static_filter_error.h>
#include <CGAL/internal/Static_filters/tools.h>
#include <cmath>
#include <iostream>

// inspired from http://cag.csail.mit.edu/~amy/papers/box-jgt.pdf

namespace CGAL {

namespace internal {

namespace Static_filters_predicates {


template < typename K_base >
class Angle_3
  : public K_base::Angle_3
{
  typedef typename K_base::Point_3   Point_3;
  typedef typename K_base::Angle_3 Base;

public:

  typedef typename Base::result_type  result_type;


#ifndef CGAL_CFG_MATCHING_BUG_6
  using Base::operator();
#else 

  template <typename T1, typename T2>
  result_type
  operator()(const T1& t1, const T2& t2) const
  {
    return Base()(t1,t2);
  }
#endif


  Sign sign_with_error(const double x, const double error) const {
    if(x > error) return POSITIVE;
    else if( x < - error) return NEGATIVE;
    else return ZERO;
  }

  result_type operator()(const Point_3 &p, const Point_3& q, const Point_3& r) const
  {
    CGAL_BRANCH_PROFILER_3("semi-static failures/attempts/calls to   : Angle_3", tmp);

    Get_approx<Point_3> get_approx; // Identity functor for all points
                                    // but lazy points
    double px, py, pz, qx, qy, qz, rx, ry, rz;

    if (fit_in_double(get_approx(p).x(), px) && fit_in_double(get_approx(p).y(), py) &&
        fit_in_double(get_approx(p).z(), pz) &&
        fit_in_double(get_approx(q).x(), qx) && fit_in_double(get_approx(q).y(), qy) &&
        fit_in_double(get_approx(q).z(), qz) &&
        fit_in_double(get_approx(r).x(), rx) && fit_in_double(get_approx(r).y(), ry) &&
        fit_in_double(get_approx(r).z(), rz) )      
    {
      CGAL_BRANCH_PROFILER_BRANCH_1(tmp);

      double pqx = px-qx;
      double rqx = rx-qx;
      double pqy = py-qy;
      double rqy = ry-qy;
      double pqz = pz-qz;
      double rqz = rz-qz;

      double maxpq = CGAL::abs(pqx);
      double maxrq = CGAL::abs(rqx);
      double apqy  = CGAL::abs(pqy);
      double arqy  = CGAL::abs(rqy);
      double apqz  = CGAL::abs(pqz);
      double arqz  = CGAL::abs(rqz);

      if(maxpq < apqy) maxpq = apqy;
      if(maxrq < arqy) maxrq = arqy;
      if(maxpq < apqz) maxpq = apqz;
      if(maxrq < arqz) maxrq = arqz;

      if(maxrq < maxpq) std::swap(maxrq, maxpq);
      // then maxrq >= maxpq

      // TODO: OVERFLOW and UNDERFLOW;

      double scal = pqx * rqx + pqy * rqy + pqz * rqz;

      double eps = 7e-15 * maxpq * maxrq;

      if(scal > eps) return ACUTE;
      if(scal < -eps) return OBTUSE;

      CGAL_BRANCH_PROFILER_BRANCH_2(tmp);
    }
    return Base::operator()(p, q, r);
  }


  // Computes the epsilon for Bbox_3_Segment_3_do_intersect.
  static double compute_epsilon()
  {
    typedef Static_filter_error F;
    F t1 = F(1);
    F f = ((t1 - t1) * (t1 - t1)) +
      ((t1 - t1) * (t1 - t1)) +
      ((t1 - t1) * (t1 - t1));
      
    double err = f.error();
    err += err * 2 * F::ulp(); // Correction due to "eps * m * m ".
    std::cerr << "*** epsilon for Angle_3(Point_3, Point_3, Point_3) = "
              << err << std::endl;
    return err;
  }

}; // class Angle_3

}  // namespace Static_filters_predicates

} // namespace internal


} //namespace CGAL

#endif  // CGAL_INTERNAL_STATIC_FILTERS_ANGLE_3_H

// Copyright (c) 2005 Rijksuniversiteit Groningen (Netherlands)
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
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
// Author(s)     : Nico Kruithof <Nico@cs.rug.nl>

#ifndef CGAL_TRIANGULATED_MIXED_COMPLEX_OBSERVER_3
#define CGAL_TRIANGULATED_MIXED_COMPLEX_OBSERVER_3

#include <CGAL/Triangulation_simplex_3.h>
#include <CGAL/Skin_surface_quadratic_surface_3.h>

CGAL_BEGIN_NAMESPACE

template <class TriangulatedMixedComplex_3,
	  class RegularTriangulation_3>
class Triangulated_mixed_complex_observer_3 {
public:
  typedef typename RegularTriangulation_3::Geom_traits     Regular_traits;
  typedef typename TriangulatedMixedComplex_3::Geom_traits Triangulated_mixed_complex_traits;
  typedef RegularTriangulation_3                     Regular;
  typedef TriangulatedMixedComplex_3                 Triangulated_mixed_complex;
  typedef typename Triangulated_mixed_complex::Triangulation_data_structure
                                                     TMC_TDS;
  typedef typename TMC_TDS::Cell::Quadratic_surface  Quadr_surface; 

  typedef typename Regular_traits::FT                FT;

  typedef typename Regular::Vertex_handle            Rt_Vertex_handle;
  typedef typename Regular::Edge                     Rt_Edge;
  typedef typename Regular::Facet                    Rt_Facet;
  typedef typename Regular::Cell_handle              Rt_Cell_handle;
  typedef Triangulation_simplex_3<Regular>           Rt_Simplex;

  typedef typename Regular::Bare_point               Rt_Point;
  typedef typename Regular::Geom_traits              Rt_Geom_traits;
  typedef typename Rt_Geom_traits::RT                Rt_RT;
  typedef typename Regular::Weighted_point           Rt_Weighted_point;

  typedef typename Triangulated_mixed_complex::Vertex_handle TMC_Vertex_handle;
//   typedef typename Triangulated_mixed_complex::Edge          TMC_Edge;
//   typedef typename Triangulated_mixed_complex::Facet         TMC_Facet;
  typedef typename Triangulated_mixed_complex::Cell_handle   TMC_Cell_handle;
  

  typedef typename Quadr_surface::K                   Surface_traits;
  typedef Regular_triangulation_euclidean_traits_3<Surface_traits> Surface_regular_traits;
  typedef typename Quadr_surface::Point               Surface_point;
  typedef typename Quadr_surface::Vector              Surface_vector;
  typedef typename Quadr_surface::Weighted_point      Surface_weighted_point;
  typedef typename Quadr_surface::RT                  Surface_RT;

//   typedef typename Triangulated_mixed_complex_traits::Point_3        TMC_Point;
//   typedef typename Triangulated_mixed_complex_traits::RT             TMC_RT;
//   typedef Weighted_point<TMC_Point,TMC_RT>        TMC_Weighted_point;

  typedef Skin_surface_quadratic_surface_3<Surface_traits> Quadratic_surface;
//   typedef Skin_surface_sphere_3<Surface_traits>         Sphere_surface;
//   typedef Skin_surface_hyperboloid_3<Surface_traits>    Hyperboloid_surface;

  typedef Weighted_converter_3< 
    Cartesian_converter < typename Regular_traits::Bare_point::R, 
			  typename Quadr_surface::K > >            R2S_converter;
    

//   typedef typename Polyhedron_traits::RT                   Mesh_RT;
//   typedef typename Polyhedron_traits::Point_3              Mesh_Point;
//   typedef Weighted_point<Mesh_Point,Mesh_RT>    Mesh_Weighted_point;
  
//   typedef typename Skin_traits_3::R2P_converter R2P_converter;
//   typedef typename Skin_traits_3::T2P_converter T2P_converter;

//   Triangulated_mixed_complex_observer_3() : 
//     shrink(.5) {
//   }

  Triangulated_mixed_complex_observer_3(Surface_RT shrink) : 
    shrink(shrink) {
  }

  void after_vertex_insertion(Rt_Simplex const &sDel, 
    			      Rt_Simplex const &sVor, 
			      TMC_Vertex_handle &vh) 
  {
  }

  void after_cell_insertion(Rt_Simplex const &s, TMC_Cell_handle &ch)  
  {
    if (!(s == prev_s)) {
      prev_s = s;
      Rt_Vertex_handle vh;
      Rt_Edge          e;
      Rt_Facet         f;
      Rt_Cell_handle   ch;

      switch (s.dimension()) {
      case 0:
	{
	  vh = s;
	  create_sphere(r2s_converter(vh->point().point()),
			-r2s_converter(vh->point().weight()),
			shrink,
			1);
	  break;
	}
      case 1:
	{
	  e = s;
	  Surface_weighted_point p0 = 
	    r2s_converter(e.first->vertex(e.second)->point());
	  Surface_weighted_point p1 = 
	    r2s_converter(e.first->vertex(e.third)->point());
	  
	  create_hyperboloid
	    (typename Surface_regular_traits::
	     Construct_weighted_circumcenter_3()(p0,p1),
	     typename Surface_regular_traits::
	     Compute_squared_radius_smallest_orthogonal_sphere_3()(p0,p1),
	     p0 - p1,
	     shrink,
	     1);
	  break;
	}
      case 2:
	{
	  f = s;
	  Surface_weighted_point p0 = 
	    r2s_converter(f.first->vertex((f.second+1)&3)->point());
	  Surface_weighted_point p1 = 
	    r2s_converter(f.first->vertex((f.second+2)&3)->point());
	  Surface_weighted_point p2 = 
	    r2s_converter(f.first->vertex((f.second+3)&3)->point());
	  
	  create_hyperboloid
	    (typename Surface_regular_traits::
	     Construct_weighted_circumcenter_3()(p0,p1,p2),
	     typename Surface_regular_traits::
	     Compute_squared_radius_smallest_orthogonal_sphere_3()(p0,p1,p2),
	     typename Surface_regular_traits::
	     Construct_orthogonal_vector_3()(p0,p1,p2),
	     1-shrink,
	     -1);
	  break;
	}
      case 3:
	{
	  ch = s;
	  create_sphere
	    (typename Surface_regular_traits::
	     Construct_weighted_circumcenter_3()
	     (r2s_converter(ch->vertex(0)->point()),
	      r2s_converter(ch->vertex(1)->point()),
	      r2s_converter(ch->vertex(2)->point()),
	      r2s_converter(ch->vertex(3)->point())),
	     typename Surface_regular_traits::
	     Compute_squared_radius_smallest_orthogonal_sphere_3()
	     (r2s_converter(ch->vertex(0)->point()),
	      r2s_converter(ch->vertex(1)->point()),
	      r2s_converter(ch->vertex(2)->point()),
	      r2s_converter(ch->vertex(3)->point())),
	     1-shrink,
	     -1);
	}
      }
    }
    ch->surf = surf;
  }

  Surface_RT shrink;
  Rt_Simplex prev_s;
  Quadr_surface *surf;

  void create_sphere(const Surface_point &c,
		     const Surface_RT &w,
		     const Surface_RT &s,
		     const int orient) {
    Q[1] = Q[3] = Q[4] = 0;
    Q[0] = Q[2] = Q[5] = orient/s;
    
    surf = new Quadratic_surface(Q, c, w);
  }

  void create_hyperboloid(const Surface_point &c,
			  const Surface_RT &w,
			  const Surface_vector &t,
			  const Surface_RT &s,
			  const int orient) {
    Surface_RT den = t*t*s*(1-s);
    
    Q[0] = orient*(-  t.x()*t.x()/den + 1/s);
    
    Q[1] = orient*(-2*t.y()*t.x()/den);
    Q[2] = orient*(-  t.y()*t.y()/den + 1/s);
    
    Q[3] = orient*(-2*t.z()*t.x()/den);
    Q[4] = orient*(-2*t.z()*t.y()/den);
    Q[5] = orient*(-  t.z()*t.z()/den + 1/s);
    
    surf = new Quadratic_surface(Q, c, w);
  }

  Surface_RT Q[6];
  R2S_converter r2s_converter;
};

CGAL_END_NAMESPACE

#endif // CGAL_TRIANGULATED_MIXED_COMPLEX_OBSERVER_3

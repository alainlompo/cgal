// Copyright (c) 2005,2006,2007,2009,2010,2011 Tel-Aviv University (Israel).
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
// Author(s)     : Idit Haran   <haranidi@post.tau.ac.il>
#ifndef CGAL_ARR_TRAPEZOID_RIC_POINT_LOCATION_FUNCTIONS_H
#define CGAL_ARR_TRAPEZOID_RIC_POINT_LOCATION_FUNCTIONS_H

/*! \file
* Member-function definitions for the Arr_trapezoid_ric_point_location<Arrangement>
* class.
*/

#define CGAL_TRAP_DEBUG

#ifdef CGAL_TRG_DEBUG
	#define CGAL_TRAP_PRINT_DEBUG(expr)   std::cout << expr << std::endl
#else
	#define CGAL_TRAP_PRINT_DEBUG(expr)
#endif

namespace CGAL {

//-----------------------------------------------------------------------------
// Locate the arrangement feature containing the given point.
//
template <class Arrangement_2>
Object Arr_trapezoid_ric_point_location<Arrangement_2>
::locate (const Point_2& p) const
{
  CGAL_TRAP_PRINT_DEBUG("locate point "<<p);

  //there are different internal compiler errors if we
  // typedef the Locate_type
  typename TD::Locate_type td_lt; 

  Td_map_item& tr = td.locate(p,td_lt);

  CGAL_TRAP_PRINT_DEBUG("after td.locate");

  // treat special case, where trapezoid is unbounded.
  //	for then get_parent() is not defined
  if (td_lt==TD::UNBOUNDED_TRAPEZOID)
  {
    CGAL_TRAP_PRINT_DEBUG("UNBOUNDED_TRAPEZOID");

    Face_const_handle ubf = _get_unbounded_face(tr, p, Are_all_sides_oblivious_tag());
    
    //check isolated vertices
      Isolated_vertex_const_iterator   iso_verts_it;
      for (iso_verts_it = ubf->isolated_vertices_begin();
           iso_verts_it != ubf->isolated_vertices_end();
           ++iso_verts_it)
      {
        if (m_traits->equal_2_object()(p, iso_verts_it->point()))
        {
          Vertex_const_handle  vh = iso_verts_it;
          return (CGAL::make_object (vh));
        }
      }
    return (CGAL::make_object (ubf));
  }

  //Halfedge_const_handle h = tr.top();

  switch(td_lt)
  {
  case TD::POINT:
    {
      //p is interior so id should fall on Td_active_vertex
      Td_active_vertex& v (boost::get<Td_active_vertex>(tr));
      Halfedge_const_handle h = v.top();
      CGAL_TRAP_PRINT_DEBUG("POINT");
      if (!h->target()->is_at_open_boundary())
      {
        if (m_traits->equal_2_object()(h->target()->point(), p))
        {
          Vertex_const_handle vh = h->target();
          return (CGAL::make_object (vh));
        }
      }
      if (!h->source()->is_at_open_boundary())
      {
        if (m_traits->equal_2_object()(h->source()->point(), p))
        {
          Vertex_const_handle vh = h->source();
          return (CGAL::make_object (vh));
        }
      }
      CGAL_error(); //if we reached here there's an error
      break;
    }

  case TD::CURVE:
    {
      Td_active_edge& e (boost::get<Td_active_edge>(tr));
      Halfedge_const_handle h = e.halfedge();
      CGAL_TRAP_PRINT_DEBUG("CURVE");
      if ( m_traits->is_in_x_range_2_object()(h->curve(),p) && 
           m_traits->compare_y_at_x_2_object()(p,h->curve()) == EQUAL)
      {
        return (CGAL::make_object(h));
      }
      else
      {
        bool res1 = m_traits->is_in_x_range_2_object()(h->curve(),p);
        Comparison_result res2 = m_traits->compare_y_at_x_2_object()(p,h->curve());
        std::cout << " is in x range = " << res1 << ", compare y at x is : " << res2 << "(not equal)\n";
        CGAL_error();
      }
      break;
    }

  case TD::TRAPEZOID:
    {
      Td_active_trapezoid t (boost::get<Td_active_trapezoid>(tr));
      Halfedge_const_handle h = t.top();
     CGAL_TRAP_PRINT_DEBUG("TRAPEZOID");
      if ( ((m_traits->is_in_x_range_2_object()(h->curve(),p)) &&
                (m_traits->compare_y_at_x_2_object()
                                          (p, h->curve()) == LARGER)) 
              !=
              (h->direction() == ARR_LEFT_TO_RIGHT)
             /*(m_traits->compare_x_2_object()(h->source()->point(),
                                        h->target()->point()) == SMALLER)*/
         )
      {
        h = h->twin();
      }
      Face_const_handle fh = h->face();

      //check isolated vertices
      Isolated_vertex_const_iterator   iso_verts_it;
      for (iso_verts_it = fh->isolated_vertices_begin();
          iso_verts_it != fh->isolated_vertices_end(); ++iso_verts_it)
      {
        if (m_traits->equal_2_object()(p, iso_verts_it->point()))
        {
          Vertex_const_handle  vh = iso_verts_it;
          return (CGAL::make_object (vh));
        }
      }

      return (CGAL::make_object(fh));
    }
  default:
    CGAL_TRAP_PRINT_DEBUG("DEFAULT");
    CGAL_error();
    break;
  }

  CGAL_TRAP_PRINT_DEBUG("EMPTY");
  return Object();   
}


/*! gets the unbounded face that contains the point when the trapezoid is unbounded
   */ 
template <class Arrangement>
typename Arr_trapezoid_ric_point_location<Arrangement>::Face_const_handle 
Arr_trapezoid_ric_point_location<Arrangement>
::_get_unbounded_face (Td_map_item& tr,const Point_2& p, Arr_all_sides_oblivious_tag) const
{
  //there's only one unbounded face
  return this->arrangement()->unbounded_faces_begin();
}


/*! gets the unbounded face that contains the point when the trapezoid is unbounded
   */ 
template <class Arrangement>
typename Arr_trapezoid_ric_point_location<Arrangement>::Face_const_handle 
Arr_trapezoid_ric_point_location<Arrangement>
::_get_unbounded_face (Td_map_item& item,const Point_2& p, Arr_not_all_sides_oblivious_tag) const
{
  Td_active_trapezoid tr (boost::get<Td_active_trapezoid>(item));
  Halfedge_const_handle h = tr.top();
  if (!tr.is_on_top_boundary() || !tr.is_on_bottom_boundary())
  { //if one of top or bottom edges is defined
    Halfedge_const_handle h = (!tr.is_on_top_boundary()) ? 
                                          tr.top() : tr.bottom();
    if ( ((m_traits->is_in_x_range_2_object()(h->curve(),p)) &&
              (m_traits->compare_y_at_x_2_object()(p, h->curve()) == LARGER))
           != (h->direction() == ARR_LEFT_TO_RIGHT))
    {
      h = h->twin();
    }
    return h->face();
  }
  else if (!tr.is_on_left_boundary())
  { //if top & bottom edges are not defined but the left() curve end is defined
    const typename TD::Curve_end left_ce(tr.left()->curve_end());

    //locate the degenerate trapezoid of left_ce. 
    
    //there are different internal compiler errors if we
    // typedef the Locate_type
    typename TD::Locate_type td_lt; 

    Td_map_item& left_v_item = td.locate(left_ce,td_lt);
    CGAL_assertion(td_lt == TD::POINT);
    Halfedge_const_handle he;
    if (boost::get<Td_active_vertex>(&left_v_item)!= NULL)
    {
      Td_active_vertex v(boost::get<Td_active_vertex>(left_v_item));
      he = v.bottom();
    }
    else
    {
      Td_active_fictitious_vertex v(boost::get<Td_active_fictitious_vertex>(left_v_item));
      he = v.bottom();
    }
    //its bottom() holds the "smallest" curve clockwise starting from 
    //  bottom (6 o'clock)
  
    CGAL_assertion_code(Halfedge_const_handle invalid_he);
    CGAL_assertion(he != invalid_he);

    //the Halfedge_handle source is left_ee.
    // this way the face on it's left is the desired one

    //MICHAL: maybe add a verification that the above occures
    return he->face();

  }
  else if (!tr.is_on_right_boundary())
  { //if top, bottom, left edges are not defined but the right() curve end is defined
    const typename TD::Curve_end right_ce(tr.right()->curve_end());
    
    //locate the degenerate trapezoid of right_ce. 
    
    //there are different internal compiler errors if we
    // typedef the Locate_type
    typename TD::Locate_type td_lt; 

    Td_map_item& right_v_item = td.locate(right_ce,td_lt);
    CGAL_assertion(td_lt == TD::POINT);
    Halfedge_const_handle he;
    if (boost::get<Td_active_vertex>(&right_v_item)!= NULL)
    {
      Td_active_vertex v(boost::get<Td_active_vertex>(right_v_item));
      he = v.top();
    }
    else
    {
      Td_active_fictitious_vertex v(boost::get<Td_active_fictitious_vertex>(right_v_item));
      he = v.top();
    }
    //its top() holds the "smallest" curve clockwise starting from 
    //  top (12 o'clock)
    
    CGAL_assertion_code(Halfedge_handle invalid_he);
    CGAL_assertion(he != invalid_he);

    //the Halfedge_handle source is right_ee.
    // this way the face on it's left is the desired one
    
    //MICHAL: maybe add a verification that the above occures
    return he->face();
  }
  
  //else, on all boundaries (top, bottom, left, right - are not defined),
  //    this is the only trapezoid in the map
  return this->arrangement()->unbounded_faces_begin();
}


//-----------------------------------------------------------------------------
// Locate the arrangement feature which a vertical ray emanating from the
// given point hits, considering isolated vertices.
//
template <class Arrangement>
Object Arr_trapezoid_ric_point_location<Arrangement>
::_vertical_ray_shoot (const Point_2& p, bool shoot_up) const
{
  //trying to workaround internal compiler error
  typename TD::Locate_type td_lt;
  Halfedge_const_handle invalid_he;
 
  X_trapezoid& tr = td.vertical_ray_shoot(p, td_lt, shoot_up);

  // treat special case, where trapezoid is unbounded.
  if (td_lt==TD::UNBOUNDED_TRAPEZOID)
  { 
    return (_check_isolated_for_vertical_ray_shoot(invalid_he, p, shoot_up, tr));
  }

  Halfedge_const_handle h = (shoot_up) ? tr.top() : tr.bottom();
  switch(td_lt)
  {
  case TD::POINT:
    if (!h->target()->is_at_open_boundary())
    {
    if (m_traits->equal_2_object()(h->target()->point(), p))
    {
      Vertex_const_handle vh = h->target();
      return (CGAL::make_object (vh));
    }
    }
    if (!h->source()->is_at_open_boundary())
    {
    if (m_traits->equal_2_object()(h->source()->point(), p))
    {
      Vertex_const_handle vh = h->source();
      return (CGAL::make_object (vh));
    }
    }

    CGAL_error();  //if we reached here - there's an error
    break;

 case TD::CURVE:
    if ((shoot_up && h->direction() == ARR_LEFT_TO_RIGHT) ||
        (!shoot_up && h->direction() == ARR_RIGHT_TO_LEFT))
    {
      h=h->twin();
    }
    return (CGAL::make_object(h));

  case TD::TRAPEZOID:
    if (!(((m_traits->is_in_x_range_2_object()(h->curve(),p)) &&
          (m_traits->compare_y_at_x_2_object()(p, h->curve()) == LARGER)) ==
          (h->direction() == ARR_LEFT_TO_RIGHT)
          /*(m_traits->compare_x_2_object()(h->source()->point(),
                                        h->target()->point()) == SMALLER)*/
        ))
    {
        h = h->twin();
    }
    return (_check_isolated_for_vertical_ray_shoot(h, p, shoot_up, tr));

  default:
    CGAL_error();
    break;
  }

  return (_check_isolated_for_vertical_ray_shoot(invalid_he, p, shoot_up, tr));
}

//-----------------------------------------------------------------------------
// In vertical ray shoot, when the closest halfedge is found (or unbounded
// face) we check the isolated vertices inside the face to check whether there
// is an isolated vertex right above/below the query point.
// 
template <class Arrangement>
Object Arr_trapezoid_ric_point_location<Arrangement>::
_check_isolated_for_vertical_ray_shoot (Halfedge_const_handle halfedge_found, 
                                        const Point_2& p, 
                                        bool shoot_up,
                                        const Td_map_item& tr) const
{
  const Comparison_result point_above_under = (shoot_up ? SMALLER : LARGER);
  typename Geometry_traits_2::Compare_x_2          compare_x =
    this->arrangement()->traits()->compare_x_2_object();
  typename Geometry_traits_2::Compare_xy_2         compare_xy =
    this->arrangement()->traits()->compare_xy_2_object();
  typename Geometry_traits_2::Compare_y_at_x_2     compare_y_at_x =
    this->arrangement()->traits()->compare_y_at_x_2_object();

  Isolated_vertex_const_iterator   iso_verts_it;
  Vertex_const_handle              closest_iso_v;
  const Vertex_const_handle        invalid_v;
  const Halfedge_const_handle      invalid_he;
  Face_const_handle                face;

  // If the closest feature is a valid halfedge, take its incident face.
  // Otherwise, take the unbounded face.
  if (halfedge_found == invalid_he)
  {
    face = _get_unbounded_face(tr, p, Are_all_sides_oblivious_tag());
  }
  else
    face = halfedge_found->face();

  // Go over the isolated vertices in the face.
  for (iso_verts_it = face->isolated_vertices_begin();
       iso_verts_it != face->isolated_vertices_end(); ++iso_verts_it)
  {
    // The current isolated vertex should have the same x-coordinate as the
    // query point in order to be below or above it.
    if (compare_x (p, iso_verts_it->point()) != EQUAL)
      continue;

    // Make sure the isolated vertex is above the query point (if we shoot up)
    // or below it (if we shoot down).
    if (compare_xy (p, iso_verts_it->point()) != point_above_under)
      continue;

    // Check if the current isolated vertex lies closer to the query point than
    // the closest feature so far.
    if (closest_iso_v == invalid_v)
    {
      // Compare the current isolated vertex with the closest halfedge.
      if (halfedge_found == invalid_he ||
          compare_y_at_x (iso_verts_it->point(),
                          halfedge_found->curve()) == point_above_under)
      {
        closest_iso_v = iso_verts_it;
      }
    }
    else if (compare_xy (iso_verts_it->point(),
                         closest_iso_v->point()) == point_above_under)
    {
      closest_iso_v = iso_verts_it;
    }
  }

  // If we found an isolated vertex above (or under) the query point, return
  // a handle to this vertex.
  if (closest_iso_v != invalid_v)
    return (CGAL::make_object (closest_iso_v));

  // If we are inside the unbounded face, return this face.
  if (halfedge_found == invalid_he)
    return (CGAL::make_object (face));

  // Return the halfedge lying above (or below) the query point.
  return (CGAL::make_object (halfedge_found));
}



} //namespace CGAL

#endif

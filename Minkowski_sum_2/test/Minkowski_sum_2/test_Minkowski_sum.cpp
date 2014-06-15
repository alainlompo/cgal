#include <CGAL/basic.h>

#ifdef CGAL_USE_GMP
  // GMP is installed. Use the GMP rational number-type. 
  #include <CGAL/Gmpq.h>
  typedef CGAL::Gmpq                                    Rational;
#else
  // GMP is not installed. Use CGAL's exact rational number-type.
  #include <CGAL/MP_Float.h>
  #include <CGAL/Quotient.h>
  typedef CGAL::Quotient<CGAL::MP_Float>                Rational;
#endif

#include <CGAL/Cartesian.h>
#include <CGAL/minkowski_sum_2.h>
#include <CGAL/Small_side_angle_bisector_decomposition_2.h>
#include <CGAL/Polygon_convex_decomposition_2.h>
#include <CGAL/Boolean_set_operations_2.h>
#include "read_polygon.h"
#include <cstring>

#include <list>

typedef CGAL::Cartesian<Rational>                   Kernel;

typedef Kernel::Point_2                             Point_2;
typedef Kernel::Segment_2                           Segment_2;
typedef CGAL::Polygon_2<Kernel>                     Polygon_2;
typedef CGAL::Polygon_with_holes_2<Kernel>          Polygon_with_holes_2;

/*! Check if two polygons with holes are the same. */
bool are_equal (const Polygon_with_holes_2& ph1,
                const Polygon_with_holes_2& ph2)
{
  std::list<Polygon_with_holes_2>   sym_diff;

  CGAL::symmetric_difference (ph1, ph2,
                              std::back_inserter(sym_diff));

  return (sym_diff.empty());
}

/*! The main program. */
int main (int argc, char **argv)
{
  // Read the input file. Because of the structure of the *.cmd file 
  // (which is concatenated to the command line) we need to get all the
  // inputs in one command line. This is the reason we read triplets/quadruplets of
  // arguments. Each triplet/quadruplet is one input for the program.
  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0] << ". The input are triplets/quadruplets of:"
	      << "<compare|verify> <polygon#1> <polygon#2> [polygon#3]" 
	      << std::endl;
    return (1);
  }

  int i = 1;
  while (i < argc)
  {
    bool verify = strcmp(argv[i], "verify") == 0;

    // Read the polygons from the input files.
    Polygon_2   pgn1, pgn2;
    Polygon_with_holes_2 result;
    
    if (! read_polygon (argv[i+1], pgn1))
    {
      std::cerr << "Failed to read: <" << argv[i+1] << ">." << std::endl;
      return (1);
    }
    
    if (! read_polygon (argv[i+2], pgn2))
    {
      std::cerr << "Failed to read: <" << argv[i+2] << ">." << std::endl;
      return (1);
    }

    if (verify)
    {
      if (! read_polygon_with_holes (argv[i+3], result))
      {
        std::cerr << "Failed to read: <" << argv[i+3] << ">." << std::endl;
        return (1);
      }
    }

    std::cout << "Testing " << argv[i+1] << " and " << argv[i+2] << std::endl;

    // Compute the Minkowski sum using the convolution method.
    Polygon_with_holes_2                                     sum_conv;
    
    std::cout << "Using the convolution method ... ";
    sum_conv = minkowski_sum_2 (pgn1, pgn2);
    std::cout << "Done." << std::endl;

    if (verify)
    {
      if (are_equal (result, sum_conv))
      {
        std::cout << "OK." << std::endl;
      }
      else
      {
        std::cout << "ERROR (different result)." << std::endl;
        return 1;
      }
    }
    else
    {
      result = sum_conv;
    }

    Polygon_with_holes_2                                     sum_conv_new;
    std::cout << "Using the reduced convolution method ... ";
    sum_conv_new = minkowski_sum_2_new (pgn1, pgn2);
    if (are_equal (result, sum_conv_new))
    {
      std::cout << "OK." << std::endl;
    }
    else
    {
      std::cout << "ERROR (different result)." << std::endl;
      return 1;
    }
    
    // Define auxiliary polygon-decomposition objects.
    CGAL::Small_side_angle_bisector_decomposition_2<Kernel>  ssab_decomp;
    CGAL::Optimal_convex_decomposition_2<Kernel>             opt_decomp;
    CGAL::Hertel_Mehlhorn_convex_decomposition_2<Kernel>     hm_approx_decomp;
    CGAL::Greene_convex_decomposition_2<Kernel>              greene_decomp;
    Polygon_with_holes_2                                     sum_decomp;
    
    std::cout << "Using the small-side angle-bisector decomposition ... ";
    sum_decomp = minkowski_sum_2 (pgn1, pgn2, ssab_decomp);
    if (are_equal (result, sum_decomp))
    {
      std::cout << "OK." << std::endl;
    }
    else
    {
      std::cout << "ERROR (different result)." << std::endl;
      return 1;
    }
    
    std::cout << "Using the optimal convex decomposition ... ";
    sum_decomp = minkowski_sum_2 (pgn1, pgn2, opt_decomp);
    if (are_equal (result, sum_decomp))
    {
      std::cout << "OK." << std::endl;
    }
    else
    {
      std::cout << "ERROR (different result)." << std::endl;
      return 1;
    }
    
    std::cout << "Using the Hertel--Mehlhorn decomposition ... ";
    sum_decomp = minkowski_sum_2 (pgn1, pgn2, hm_approx_decomp);
    if (are_equal (result, sum_decomp))
    {
      std::cout << "OK." << std::endl;
    }
    else
    {
      std::cout << "ERROR (different result)." << std::endl;
      return 1;
    }
    
    std::cout << "Using the Greene decomposition ... ";
    sum_decomp = minkowski_sum_2 (pgn1, pgn2, greene_decomp);
    if (are_equal (result, sum_decomp))
    {
      std::cout << "OK." << std::endl;
    }
    else
    {
      std::cout << "ERROR (different result)." << std::endl;
      return 1;
    }
    
    if (verify)
    {
      i += 4;
    }
    else
      i += 3;
  }

  return (0);
}

#ifndef IMPORTS_H
#define IMPORTS_H

#include <gmTrianglesystemModule>
#include <gmCoreModule>

namespace FemdomSlave {

using TriangleFacets = GMlib::TriangleFacets<float>;
using SqMatrix = GMlib::SqMatrix<float,2>;
using Triangle = GMlib::TSTriangle<float>;
using Vector = GMlib::Vector<float, 2>;
using DMatrix = GMlib::DMatrix<float>;
using DVector = GMlib::DVector<float>;
using Vertex = GMlib::TSVertex<float>;
using Point = GMlib::Point<float,2>;
using Edge = GMlib::TSEdge<float>;
using Angle = GMlib::Angle;

} // namespace FemdomSlave

#endif // IMPORTS_H

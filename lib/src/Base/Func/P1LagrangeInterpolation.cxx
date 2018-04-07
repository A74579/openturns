//                                               -*- C++ -*-
/**
 *  @brief P1 Lagrange interpolation between two meshes.
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/P1LagrangeInterpolation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

#include <algorithm>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(P1LagrangeInterpolation)

static const Factory<P1LagrangeInterpolation> Factory_P1LagrangeInterpolation;


/* Default constructor */
P1LagrangeInterpolation::P1LagrangeInterpolation()
  : FieldFunctionImplementation()
  , inputMesh_()
  , outputMesh_()
  , enclosingSimplex_()
  , nearestNeighbour_()
  , barycentricCoordinates_()
  , neighbours_()
{
  // Nothing to do
}

/* Parameters constructor */
P1LagrangeInterpolation::P1LagrangeInterpolation(const Mesh & inputMesh, const Mesh & outputMesh, const UnsignedInteger dimension)
  : FieldFunctionImplementation(inputMesh.getDimension(), dimension, dimension)
  , inputMesh_(inputMesh)
  , outputMesh_(outputMesh)
  , enclosingSimplex_(inputMesh.getVertices(), inputMesh.getSimplices())
  , nearestNeighbour_(inputMesh.getVertices())
  , barycentricCoordinates_()
  , neighbours_()
{
  computeProjection();
}


/* Parameters constructor */
P1LagrangeInterpolation::P1LagrangeInterpolation(const Mesh & inputMesh,
                                                 const Mesh & outputMesh,
                                                 const UnsignedInteger dimension,
                                                 const EnclosingSimplexAlgorithm & enclosingSimplex,
                                                 const NearestNeighbourAlgorithm & nearestNeighbour)
  : FieldFunctionImplementation(inputMesh.getDimension(), dimension, dimension)
  , inputMesh_(inputMesh)
  , outputMesh_(outputMesh)
  , enclosingSimplex_(enclosingSimplex.getImplementation()->emptyClone())
  , nearestNeighbour_(nearestNeighbour.getImplementation()->emptyClone())
  , barycentricCoordinates_()
  , neighbours_()
{
  enclosingSimplex_.setVerticesAndSimplices(inputMesh_.getVertices(), inputMesh_.getSimplices());
  nearestNeighbour_.setSample(inputMesh_.getVertices());
  computeProjection();
}


/* Virtual constructor */
P1LagrangeInterpolation * P1LagrangeInterpolation::clone() const
{
  return new P1LagrangeInterpolation(*this);
}

/* Mesh accessors */
Mesh P1LagrangeInterpolation::getInputMesh() const
{
  return inputMesh_;
}

Mesh P1LagrangeInterpolation::getOutputMesh(const Mesh & inputMesh) const
{
  if (inputMesh != inputMesh_) throw InvalidArgumentException(HERE) << "Error: input mesh is not the one used when building P1LagrangeInterpolation";
  return outputMesh_;
}

/* Field dimension accessor */
void P1LagrangeInterpolation::setDimension(const UnsignedInteger dimension)
{
  inputDimension_ = dimension;
  outputDimension_ = dimension;
}

/* Enclosing simplex algorithm accessor */
EnclosingSimplexAlgorithm P1LagrangeInterpolation::getEnclosingSimplexAlgorithm() const
{
  return enclosingSimplex_;
}

/* Nearest neighbour algorithm accessor */
NearestNeighbourAlgorithm P1LagrangeInterpolation::getNearestNeighbourAlgorithm() const
{
  return nearestNeighbour_;
}

/* Comparison operator */
Bool P1LagrangeInterpolation::operator ==(const P1LagrangeInterpolation & other) const
{
  if (this == &other) return true;
  return (inputMesh_ == other.inputMesh_) && (outputMesh_ == other.outputMesh_);
}


/* String converter */
String P1LagrangeInterpolation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << P1LagrangeInterpolation::GetClassName()
      << " name=" << getName()
      << " inputMesh=" << inputMesh_
      << " outputMesh=" << outputMesh_;
  return oss;
}

String P1LagrangeInterpolation::__str__( const String & offset ) const
{
  OSS oss(false);
  oss << "class=" << P1LagrangeInterpolation::GetClassName()
      << " name=" << getName()
      << " inputMesh=" << inputMesh_
      << " outputMesh=" << outputMesh_;
  return oss;
}

/* Compute sparse projection matrix */
void P1LagrangeInterpolation::computeProjection()
{
  const UnsignedInteger outputSize = outputMesh_.getVerticesNumber();
  barycentricCoordinates_ = Sample(outputSize, 1 + spatialDimension_);
  neighbours_ = IndicesCollection(outputSize, 1 + spatialDimension_);

  const Sample outputVertices(outputMesh_.getVertices());
  const Indices simplexIndices = enclosingSimplex_.query(outputVertices);
  Indices outside;
  for(UnsignedInteger i = 0; i < outputSize; ++i)
  {
    if (simplexIndices[i] >= inputMesh_.getSimplicesNumber())
    {
      outside.add(i);
    }
  }
  Indices nearestPointIndices;
  if (outside.getSize() > 0)
  {
    nearestPointIndices = nearestNeighbour_.query(outputVertices.select(outside));
  }

  Point coordinates(spatialDimension_ + 1);
  UnsignedInteger counterOutside = 0;
  Collection< std::pair<UnsignedInteger, Scalar> > neighbourAndCoefficient(1 + spatialDimension_);
  const IndicesCollection simplices(inputMesh_.getSimplices());
  for(UnsignedInteger i = 0; i < outputSize; ++i)
  {
    if (simplexIndices[i] >= inputMesh_.getSimplicesNumber())
    {
      barycentricCoordinates_(i, 0) = 1.0;
      const UnsignedInteger nearest = nearestPointIndices[counterOutside];
      // All other coefficients are zero, but we set all indices to the same
      // point in order to avoid memory gaps during matrix-matrix multiplication
      for (UnsignedInteger j = 0; j <= spatialDimension_; ++j)
        neighbours_(i, j) = nearest;
      ++counterOutside;
    }
    else
    {
      if (!inputMesh_.checkPointInSimplexWithCoordinates(outputVertices[i], simplexIndices[i], coordinates))
      {
        throw InvalidArgumentException(HERE);
      }
      IndicesCollection::const_iterator cit = simplices.cbegin_at(simplexIndices[i]);
      // Points are sorted to avoid memory gaps during matrix-matrix multiplication
      for (UnsignedInteger j = 0; j <= spatialDimension_; ++j, ++cit)
      {
        neighbourAndCoefficient[j].first = *cit;
        neighbourAndCoefficient[j].second = coordinates[j];
      }
      std::sort(neighbourAndCoefficient.begin(), neighbourAndCoefficient.end());
      for (UnsignedInteger j = 0; j <= spatialDimension_; ++j)
      {
        neighbours_(i, j) = neighbourAndCoefficient[j].first;
        barycentricCoordinates_(i, j) = neighbourAndCoefficient[j].second;
      }
    }
  }
}

Field P1LagrangeInterpolation::operator()(const Field & field) const
{
  if (field.getMesh() != inputMesh_) throw InvalidArgumentException(HERE) << "Error: the given field is not defined on the expected mesh";
  const UnsignedInteger dimension = field.getOutputDimension();
  if (dimension != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the given field has an invalid output dimension. Expect a dimension " << inputDimension_ << ", got " << dimension;
  const UnsignedInteger outputSize = outputMesh_.getVerticesNumber();
  const Sample values(field.getValues());
  Sample result(outputSize, dimension);
  for(UnsignedInteger i = 0; i < outputSize; ++i)
  {
    IndicesCollection::const_iterator cit = neighbours_.cbegin_at(i);
    for (UnsignedInteger j = 0; j <= spatialDimension_; ++j, ++cit)
    {
      const UnsignedInteger neighbour = *cit;
      const Scalar alpha = barycentricCoordinates_(i, j);
      for (UnsignedInteger p = 0; p < dimension; ++p)
      {
        result(i, p) += alpha * values(neighbour, p);
      }
    }
  }
  return Field(outputMesh_, result);
}

/* Method save() stores the object through the StorageManager */
void P1LagrangeInterpolation::save(Advocate & adv) const
{
  FieldFunctionImplementation::save(adv);
  adv.saveAttribute("inputMesh_", inputMesh_);
  adv.saveAttribute("outputMesh_", outputMesh_);
  adv.saveAttribute("enclosingSimplex_", *(enclosingSimplex_.getImplementation()->emptyClone()));
  adv.saveAttribute("nearestNeighbour_", *(nearestNeighbour_.getImplementation()->emptyClone()));
}

/* Method load() reloads the object from the StorageManager */
void P1LagrangeInterpolation::load(Advocate & adv)
{
  FieldFunctionImplementation::load(adv);
  adv.loadAttribute("inputMesh_", inputMesh_);
  adv.loadAttribute("outputMesh_", outputMesh_);
  adv.loadAttribute("enclosingSimplex_", enclosingSimplex_);
  adv.loadAttribute("nearestNeighbour_", nearestNeighbour_);
  enclosingSimplex_.setVerticesAndSimplices(inputMesh_.getVertices(), inputMesh_.getSimplices());
  nearestNeighbour_.setSample(inputMesh_.getVertices());
  computeProjection();
}

END_NAMESPACE_OPENTURNS

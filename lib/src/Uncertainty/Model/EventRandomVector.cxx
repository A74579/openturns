//                                               -*- C++ -*-
/**
 * @brief The class that implements an event random vector, i.e. a
 *        composite random vector built upon a boolean function which
 *        is described by a threshold and a comparison operator. The
 *        antecedent of the event must be a "classical" composite random
 *        vector in the context of OpenTURNS.
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "openturns/EventRandomVector.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ComparisonOperatorImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(EventRandomVector)

static const Factory<EventRandomVector> Factory_EventRandomVector;

/* Default constructor */
EventRandomVector::EventRandomVector()
  : CompositeRandomVector()
  , operator_()
  , threshold_()
{
  // Nothing to do
}

/* Constructor from RandomVector */
EventRandomVector::EventRandomVector(const RandomVectorImplementation & antecedent,
                                     const ComparisonOperator & op,
                                     const Scalar threshold)
  : CompositeRandomVector()
  , operator_(op)
  , threshold_(threshold)
{
  // Event can only be constructed from composite random vectors
  if (!antecedent.isComposite())
    throw InvalidArgumentException(HERE) << "Event can only be constructed from composite random vectors. The random vector ("
                                         << antecedent << ") passed as first argument of EventRandomVector "
                                         << " has incorrect type";
  // EventRandomVector can only be constructed from 1D random vectors
  if (antecedent.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "EventRandomVector can only be constructed from 1D random vectors. The random vector ("
                                         << antecedent << ") passed as first argument of EventRandomVector "
                                         << " has incorrect dimension";
  function_ = antecedent.getFunction();
  antecedent_ = antecedent.getAntecedent();
  setName(antecedent.getName());
  setDescription(antecedent.getDescription());
}

EventRandomVector * EventRandomVector::clone() const
{
  return new EventRandomVector(*this);
}

/* String converter */
String EventRandomVector::__repr__() const
{
  OSS oss;
  oss << "class=" << EventRandomVector::GetClassName()
      << " antecedent=" << CompositeRandomVector::__repr__()
      << " operator=" << operator_
      << " threshold=" << threshold_;
  return oss;
}

/* Dimension accessor */
UnsignedInteger EventRandomVector::getDimension() const
{
  return 1;
}

/* Operator accessor */
ComparisonOperator EventRandomVector::getOperator() const
{
  return operator_;
}

/* Threshold accessor */
Scalar EventRandomVector::getThreshold() const
{
  return threshold_;
}


Domain EventRandomVector::getDomain() const
{
  Interval result(threshold_, threshold_);
  if (operator_(1.0, 2.0))
    result.setFiniteLowerBound(Interval::BoolCollection(1, false));
  else
    result.setFiniteUpperBound(Interval::BoolCollection(1, false));
  return result;
}

/* Realization accessor */
Point EventRandomVector::getRealization() const
{
  return Point(1, operator_(CompositeRandomVector::getRealization()[0], threshold_));
}

/* Numerical sample accessor */
Sample EventRandomVector::getSample(const UnsignedInteger size) const
{
  // We don't build the return sample element by element because it doesn't
  // use the potential distribution of the computation. As the returned
  // sample can be huge, we use it twice in place
  // First, it stores a sample of its antecedent
  Sample returnSample(CompositeRandomVector::getSample(size));
  // Then, we loop over the sample and substitute realizations of the eventRandomVectorImplementation
  // in place of the realizations of the antecedent
  for (UnsignedInteger i = 0; i < size; ++i) returnSample[i][0] = operator_(returnSample[i][0], threshold_);
  returnSample.setName("EventRandomVector sample");
  returnSample.setDescription(getDescription());
  return returnSample;
}

Point EventRandomVector::getParameter() const
{
  Point parameter(CompositeRandomVector::getParameter());
  parameter.add(threshold_);
  return parameter;
}

void EventRandomVector::setParameter(const Point & parameter)
{
  const UnsignedInteger compositeParameterDimension = CompositeRandomVector::getParameter().getDimension();
  if (parameter.getDimension() != (compositeParameterDimension + 1))
    throw InvalidArgumentException(HERE) << "Wrong event random vector parameter size";
  Point compositeParameter(compositeParameterDimension);
  std::copy(parameter.begin(), parameter.begin() + compositeParameterDimension, compositeParameter.begin());
  CompositeRandomVector::setParameter(compositeParameter);
  threshold_ = parameter[compositeParameterDimension];
}

Description EventRandomVector::getParameterDescription() const
{
  Description description(CompositeRandomVector::getParameterDescription());
  description.add("threshold");
  return description;
}

/* Method save() stores the object through the StorageManager */
void EventRandomVector::save(Advocate & adv) const
{
  CompositeRandomVector::save(adv);
  adv.saveAttribute( "operator_", operator_ );
  adv.saveAttribute( "threshold_", threshold_ );
}

/* Method load() reloads the object from the StorageManager */
void EventRandomVector::load(Advocate & adv)
{
  CompositeRandomVector::load(adv);
  adv.loadAttribute( "operator_", operator_ );
  adv.loadAttribute( "threshold_", threshold_ );
}

END_NAMESPACE_OPENTURNS

//                                               -*- C++ -*-
/**
 *  @brief Factory for Student distribution
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
#include "openturns/StudentFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(StudentFactory)

static const Factory<StudentFactory> Factory_StudentFactory;

/* Default constructor */
StudentFactory::StudentFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
StudentFactory * StudentFactory::clone() const
{
  return new StudentFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution StudentFactory::build(const Sample & sample) const
{
  return buildAsStudent(sample).clone();
}

Distribution StudentFactory::build(const Point & parameter) const
{
  return buildAsStudent(parameter).clone();
}

Distribution StudentFactory::build() const
{
  return buildAsStudent().clone();
}

DistributionFactoryResult StudentFactory::buildEstimator(const Sample & sample) const
{
  return buildBootStrapEstimator(sample, true);
}

Student StudentFactory::buildAsStudent(const Sample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Student distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Student distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const Scalar mu = sample.computeMean()[0];
  const Scalar sigma = sample.computeStandardDeviationPerComponent()[0];
  const Scalar nu = 2.0 + 2.0 / (sigma * sigma - 1.0);
  if (!(nu > 2.0)) throw InvalidArgumentException(HERE) << "Error: can build a Student distribution only if nu > 2.0, here nu=" << nu;
  Student result(nu, mu);
  result.setDescription(sample.getDescription());
  return result;
}

Student StudentFactory::buildAsStudent(const Point & parameter) const
{
  try
  {
    Student distribution;
    distribution.setParameter(parameter);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Student distribution from the given parameters";
  }
}

Student StudentFactory::buildAsStudent() const
{
  return Student();
}

END_NAMESPACE_OPENTURNS

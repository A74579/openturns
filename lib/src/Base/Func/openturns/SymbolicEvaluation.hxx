//                                               -*- C++ -*-
/**
 *  @brief The class that implements the evaluation of an analytical function.
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

#ifndef OPENTURNS_SYMBOLICEVALUATION_HXX
#define OPENTURNS_SYMBOLICEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/SymbolicParser.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/Evaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SymbolicEvaluation
 *
 * The class that implement the evaluation of an analytical function.
 */
class OT_API SymbolicEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  SymbolicEvaluation();

  /** Default constructor */
  SymbolicEvaluation(const Description & inputVariablesNames,
                     const Description & outputVariablesNames,
                     const Description & formulas);

  /** Constructor with a single formula and multiple ouutputs */
  SymbolicEvaluation(const Description & inputVariablesNames,
                     const Description & outputVariablesNames,
                     const String & formula);

  /** Virtual constructor */
  virtual SymbolicEvaluation * clone() const;

  /** Comparison operator */
  Bool operator ==(const SymbolicEvaluation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  using EvaluationImplementation::operator();
  Point operator() (const Point & inP) const;
  Sample operator() (const Sample & inS) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Get the i-th marginal function */
  Evaluation getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  Evaluation getMarginal(const Indices & indices) const;

  /** Accessor to the input variables names */
  Description getInputVariablesNames() const;

  /** Accessor to the output variables names */
  Description getOutputVariablesNames() const;

  /** Accessor to the formulas */
  Description getFormulas() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

private:
  // initialize parser
  void initialize();

  friend class SymbolicGradient;
  friend class SymbolicHessian;

  Description inputVariablesNames_;
  Description outputVariablesNames_;
  Description formulas_;

  /** A mathematical expression parser from the muParser library */
  mutable SymbolicParser parser_;

}; /* class SymbolicEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SYMBOLICEVALUATION_HXX */

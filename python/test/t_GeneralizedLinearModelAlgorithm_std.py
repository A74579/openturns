#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

try:

    # Set Numerical precision to 4
    ot.PlatformInfo.SetNumericalPrecision(4)
    sampleSize = 40
    spatialDimension = 1

    # Create the function to estimate
    model = ot.SymbolicFunction(["x0"], ["x0"])

    X = ot.NumericalSample(sampleSize, spatialDimension)
    for i in range(sampleSize):
        X[i, 0] = 3.0 + (8.0 * i) / sampleSize
    Y = model(X)
    
    # Add a small noise to data
    Y += ot.GaussianProcess(ot.AbsoluteExponential([0.1], [0.2]), ot.Mesh(X)).getRealization().getValues()

    basis = ot.LinearBasisFactory(spatialDimension).build()
    # Case of a misspecified covariance model
    covarianceModel = ot.DiracCovarianceModel(spatialDimension)
    print("===================================================\n")
    algo = ot.GeneralizedLinearModelAlgorithm(X, Y, covarianceModel, basis)
    algo.run()

    result = algo.getResult()
    print("\ncovariance (dirac, optimized)=", result.getCovarianceModel())
    print("trend (dirac, optimized)=", result.getTrendCoefficients())
    print("===================================================\n")
    # Now without estimating covariance parameters
    basis = ot.LinearBasisFactory(spatialDimension).build()
    covarianceModel = ot.DiracCovarianceModel(spatialDimension)
    algo = ot.GeneralizedLinearModelAlgorithm(X, Y, covarianceModel, basis, True, True)
    algo.setOptimizeParameters(False)
    algo.run()
    result = algo.getResult()
    print("\ncovariance (dirac, not optimized)=", result.getCovarianceModel())
    print("trend (dirac, not optimized)=", result.getTrendCoefficients())
    print("===================================================\n")

    # Case of a well specified covariance model
    # Test the optimization when the amplitude is deduced analytically from the scale
    covarianceModel = ot.AbsoluteExponential(spatialDimension)
    algo = ot.GeneralizedLinearModelAlgorithm(X, Y, covarianceModel, basis)
    algo.run()
    result = algo.getResult()
    print("\ncovariance (reduced, unbiased)=", result.getCovarianceModel())
    print("trend (reduced, unbiased)=", result.getTrendCoefficients())
    print("===================================================\n")
    ot.ResourceMap.SetAsBool("GeneralizedLinearModelAlgorithm-UnbiasedVariance", False)
    algo = ot.GeneralizedLinearModelAlgorithm(X, Y, covarianceModel, basis)
    algo.run()
    result = algo.getResult()
    print("\ncovariance (reduced, biased)=", result.getCovarianceModel())
    print("trend (reduced, biased)=", result.getTrendCoefficients())
    print("===================================================\n")
    ot.ResourceMap.SetAsBool("GeneralizedLinearModelAlgorithm-UseAnalyticalAmplitudeEstimate", False)
    algo = ot.GeneralizedLinearModelAlgorithm(X, Y, covarianceModel, basis)
    algo.run()
    result = algo.getResult()
    print("\ncovariance (full optim)=", result.getCovarianceModel())
    print("trend (full optim)=", result.getTrendCoefficients())
    print("===================================================\n")

except:
    import sys
    print("t_GeneralizedLinearModelAlgorithm_std.py",
          sys.exc_info()[0], sys.exc_info()[1])

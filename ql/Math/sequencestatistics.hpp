
/*
 Copyright (C) 2003 Ferdinando Ametrano

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file sequencestatistics.hpp
    \brief Statistics tools for sequence (vector, list, array) samples
*/

#ifndef quantlib_sequence_statistics_hpp
#define quantlib_sequence_statistics_hpp

#include <ql/Math/statistics.hpp>

namespace QuantLib {

    //! Statistics analysis of N-dimensional (sequence) data
    /*! It provides 1-dimensional statistics as discrepancy plus
        N-dimensional (sequence) statistics (e.g. mean,
        variance, skewness, kurtosis, etc.) with one component for each
        dimension of the sample space.

        For most of the statistics this class relies on
        the StatisticsType underlying class to provide 1-D methods that
        will be iterated for all the components of the N-D data. These
        lifted methods are the union of all the methods that might be
        requested to the 1-D underlying StatisticsType class, with the
        usual compile-time checks provided by the template approach.
    */
    template <class StatisticsType = Statistics>
    class SequenceStatistics {
      public:
        // typedefs
        typedef StatisticsType statistics_type;
        // constructor
        SequenceStatistics(Size dimension);
        //! \name inspectors
        //@{
        Size size() const { return dimension_; }
        //@}
        //! \name covariance and correlation
        //@{
        //! returns the covariance Matrix
        Disposable<Matrix> covariance() const;
        //! returns the correlation Matrix
        Disposable<Matrix> correlation() const;
        //@}
        //! \name 1-D inspectors lifted from underlying statistics class
        //@{
        Size samples() const;
        Real weightSum() const;
        //@}
        //! \name N-D inspectors lifted from underlying statistics class
        //@{
        // void argument list
        std::vector<Real> mean() const;
        std::vector<Real> variance() const;
        std::vector<Real> standardDeviation() const;
        std::vector<Real> downsideVariance() const;
        std::vector<Real> downsideDeviation() const;
        std::vector<Real> semiVariance() const;
        std::vector<Real> semiDeviation() const;
        std::vector<Real> errorEstimate() const;
        std::vector<Real> skewness() const;
        std::vector<Real> kurtosis() const;
        std::vector<Real> min() const;
        std::vector<Real> max() const;

        // single argument list
        std::vector<Real> gaussianPercentile(Real y) const;
        std::vector<Real> percentile(Real y) const;

        std::vector<Real> gaussianPotentialUpside(Real percentile) const;
        std::vector<Real> potentialUpside(Real percentile) const;

        std::vector<Real> gaussianValueAtRisk(Real percentile) const;
        std::vector<Real> valueAtRisk(Real percentile) const;

        std::vector<Real> gaussianExpectedShortfall(Real percentile) const;
        std::vector<Real> expectedShortfall(Real percentile) const;

        std::vector<Real> regret(Real target) const;

        std::vector<Real> gaussianShortfall(Real target) const;
        std::vector<Real> shortfall(Real target) const;

        std::vector<Real> gaussianAverageShortfall(Real target) const;
        std::vector<Real> averageShortfall(Real target) const;

        //@}
        //! \name Modifiers
        //@{
        void reset(Size dimension = 0);
        template <class Sequence>
        void add(const Sequence& sample,
                 Real weight = 1.0) {
            add(sample.begin(),sample.end(),weight);
        }
        template <class Iterator>
        void add(Iterator begin,
                 Iterator end,
                 Real weight = 1.0) {
            QL_REQUIRE(std::distance(begin, end) == Integer(dimension_),
                       "sample size mismatch");

            quadraticSum_ += weight * outerProduct(begin, end,
                                                   begin, end);

            for (Size i=0; i<dimension_; begin++, i++)
                stats_[i].add(*begin, weight);

        }
        //@}
      protected:
        Size dimension_;
        std::vector<statistics_type> stats_;
        mutable std::vector<Real> results_;
        Matrix quadraticSum_;
    };


    // inline definitions

    template <class Stat>
    inline SequenceStatistics<Stat>::SequenceStatistics(Size dimension)
    : dimension_(0) {
        reset(dimension);
    }

    template <class Stat>
    inline Size SequenceStatistics<Stat>::samples() const {
        return stats_[0].samples();
    }

    template <class Stat>
    inline Real SequenceStatistics<Stat>::weightSum() const {
        return stats_[0].weightSum();
    }


    // macros for the implementation of the lifted methods

    // N-D methods' definition with void argument list
    #define DEFINE_SEQUENCE_STAT_CONST_METHOD_VOID(METHOD) \
    template <class Stat> \
    std::vector<Real> \
    SequenceStatistics<Stat>::METHOD() const { \
        for (Size i=0; i<dimension_; i++) \
            results_[i] = stats_[i].METHOD(); \
        return results_; \
    }
    DEFINE_SEQUENCE_STAT_CONST_METHOD_VOID(mean)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_VOID(variance)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_VOID(standardDeviation)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_VOID(downsideVariance)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_VOID(downsideDeviation)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_VOID(semiVariance)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_VOID(semiDeviation)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_VOID(errorEstimate)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_VOID(skewness)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_VOID(kurtosis)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_VOID(min)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_VOID(max)
    #undef DEFINE_SEQUENCE_STAT_CONST_METHOD_VOID


    // N-D methods' definition with single argument
    #define DEFINE_SEQUENCE_STAT_CONST_METHOD_DOUBLE(METHOD) \
    template <class Stat> \
    std::vector<Real> \
    SequenceStatistics<Stat>::METHOD(Real x) const { \
        for (Size i=0; i<dimension_; i++) \
            results_[i] = stats_[i].METHOD(x); \
        return results_; \
    }

    DEFINE_SEQUENCE_STAT_CONST_METHOD_DOUBLE(gaussianPercentile)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_DOUBLE(gaussianPotentialUpside)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_DOUBLE(gaussianValueAtRisk)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_DOUBLE(gaussianExpectedShortfall)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_DOUBLE(gaussianShortfall)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_DOUBLE(gaussianAverageShortfall)

    DEFINE_SEQUENCE_STAT_CONST_METHOD_DOUBLE(percentile)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_DOUBLE(potentialUpside)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_DOUBLE(valueAtRisk)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_DOUBLE(expectedShortfall)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_DOUBLE(regret)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_DOUBLE(shortfall)
    DEFINE_SEQUENCE_STAT_CONST_METHOD_DOUBLE(averageShortfall)
    #undef DEFINE_SEQUENCE_STAT_CONST_METHOD_DOUBLE


    template <class Stat>
    void SequenceStatistics<Stat>::reset(Size dimension) {
        if (dimension == 0)           // if no size given,
            dimension = dimension_;   // keep the current one
        QL_REQUIRE(dimension > 0, "null dimension");
        if (dimension == dimension_) {
            for (Size i=0; i<dimension_; i++)
                stats_[i].reset();
        } else {
            dimension_ = dimension;
            stats_ = std::vector<Stat>(dimension);
            results_ = std::vector<Real>(dimension);
        }
        quadraticSum_ = Matrix(dimension_, dimension_, 0.0);
    }



    template <class Stat>
    Disposable<Matrix> SequenceStatistics<Stat>::covariance() const {
        Real sampleWeight = weightSum();
        QL_REQUIRE(sampleWeight > 0.0,
                   "sampleWeight=0, unsufficient");

        Real sampleNumber = samples();
        QL_REQUIRE(sampleNumber > 1.0,
                   "sample number <=1, unsufficient");

        std::vector<Real> m = mean();
        Real inv = 1.0/sampleWeight;

        Matrix result = inv*quadraticSum_;
        result -= outerProduct(m.begin(), m.end(),
                               m.begin(), m.end());

        result *= (sampleNumber/(sampleNumber-1.0));
        return result;
    }


    template <class Stat>
    Disposable<Matrix> SequenceStatistics<Stat>::correlation() const {
        Matrix correlation = covariance();
        Array variances = correlation.diagonal();
        for (Size i=0; i<dimension_; i++){
            for (Size j=0; j<dimension_; j++){
                if (i==j) {
                    if (variances[i]==0.0) {
                        correlation[i][j] = 1.0;
                    } else {
                        correlation[i][j] *= 
                            1.0/QL_SQRT(variances[i]*variances[j]);
                    }
                } else {
                    if (variances[i]==0.0 && variances[j]==0) {
                        correlation[i][j] = 1.0;
                    } else if (variances[i]==0.0 || variances[j]==0.0) {
                        correlation[i][j] = 0.0;
                    } else {
                        correlation[i][j] *= 
                            1.0/QL_SQRT(variances[i]*variances[j]);
                    }
                }
            } // j for
        } // i for

        return correlation;
    }

}


#endif

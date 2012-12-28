#region Copyright
/*
 * The original .NET implementation of the SimMetrics library is taken from the Java
 * source and converted to NET using the Microsoft Java converter.
 * It is notclear who made the initial convertion to .NET.
 * 
 * This updated version has started with the 1.0 .NET release of SimMetrics and used
 * FxCop (http://www.gotdotnet.com/team/fxcop/) to highlight areas where changes needed 
 * to be made to the converted code.
 * 
 * this version with updates Copyright (c) 2006 Chris Parkinson.
 * 
 * For any queries on the .NET version please contact me through the 
 * sourceforge web address.
 * 
 * SimMetrics - SimMetrics is a java library of Similarity or Distance
 * Metrics, e.g. Levenshtein Distance, that provide float based similarity
 * measures between string Data. All metrics return consistant measures
 * rather than unbounded similarity scores.
 *
 * Copyright (C) 2005 Sam Chapman - Open Source Release v1.1
 *
 * Please Feel free to contact me about this library, I would appreciate
 * knowing quickly what you wish to use it for and any criticisms/comments
 * upon the SimMetric library.
 *
 * email:       s.chapman@dcs.shef.ac.uk
 * www:         http://www.dcs.shef.ac.uk/~sam/
 * www:         http://www.dcs.shef.ac.uk/~sam/stringmetrics.html
 *
 * address:     Sam Chapman,
 *              Department of Computer Science,
 *              University of Sheffield,
 *              Sheffield,
 *              S. Yorks,
 *              S1 4DP
 *              United Kingdom,
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#endregion

namespace SimMetricsMetricUtilities {
    using System;
    using SimMetricsApi;
    using SimMetricsUtilities;

    /// <summary>
    /// implements the Gotoh extension of the smith waterman method incorporating affine gaps in the strings
    /// </summary>
    [Serializable]
    sealed public class SmithWatermanGotoh : SmithWatermanGotohWindowedAffine {
        /// <summary>
        /// 
        /// </summary>
        const int affineGapWindowSize = 0x7fffffff;

        /// <summary>
        ///  a constant for calculating the estimated timing cost.
        /// </summary>
        const double estimatedTimingConstant = 2.2e-005F;

        /// <summary>
        /// constructor - default (empty).
        /// </summary>
        public SmithWatermanGotoh()
            : base(new AffineGapRange5To0Multiplier1(), new SubCostRange5ToMinus3(), affineGapWindowSize) {}

        /// <summary>
        /// constructor
        /// </summary>
        /// <param name="gapCostFunction">the gap cost function</param>
        public SmithWatermanGotoh(AbstractAffineGapCost gapCostFunction)
            : base(gapCostFunction, new SubCostRange5ToMinus3(), affineGapWindowSize) {}

        /// <summary>
        /// constructor
        /// </summary>
        /// <param name="gapCostFunction">the gap cost function</param>
        /// <param name="costFunction">the cost function to use</param>
        public SmithWatermanGotoh(AbstractAffineGapCost gapCostFunction, AbstractSubstitutionCost costFunction)
            : base(gapCostFunction, costFunction, affineGapWindowSize) {}

        /// <summary>
        /// constructor
        /// </summary>
        /// <param name="costFunction">the cost function to use</param>
        public SmithWatermanGotoh(AbstractSubstitutionCost costFunction)
            : base(new AffineGapRange5To0Multiplier1(), costFunction, affineGapWindowSize) {}

        /// <summary>
        /// gets the estimated time in milliseconds it takes to perform a similarity timing.
        /// </summary>
        /// <param name="firstWord"></param>
        /// <param name="secondWord"></param>
        /// <returns>the estimated time in milliseconds taken to perform the similarity measure</returns>
        public override double GetSimilarityTimingEstimated(string firstWord, string secondWord) {
            if ((firstWord != null) && (secondWord != null)) {
                double firstLength = firstWord.Length;
                double secondLength = secondWord.Length;
                return
                    (firstLength * secondLength * firstLength + firstLength * secondLength * secondLength) *
                    estimatedTimingConstant;
            }
            return 0.0;
        }

        /// <summary>
        /// returns the long string identifier for the metric.
        /// </summary>
        public override string LongDescriptionString { get { return "Implements the Smith-Waterman-Gotoh algorithm providing a similarity measure between two string"; } }

        /// <summary>
        /// returns the string identifier for the metric.
        /// </summary>
        public override string ShortDescriptionString { get { return "SmithWatermanGotoh"; } }
    }
}
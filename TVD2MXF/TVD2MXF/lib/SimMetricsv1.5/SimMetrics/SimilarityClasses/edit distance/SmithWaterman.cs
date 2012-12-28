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
    /// implements the Smith-Waterman edit distance function
    /// </summary>
    [Serializable]
    sealed public class SmithWaterman : AbstractStringMetric {
        const double defaultGapCost = 0.5;
        const double defaultMismatchScore = 0.0;
        const double defaultPerfectMatchScore = 1.0;
        /// <summary>
        /// a constant for calculating the estimated timing cost.
        /// </summary>
        const double estimatedTimingConstant = 0.000161F;

        /// <summary>
        /// constructor - default (empty).
        /// </summary>
        public SmithWaterman() : this(defaultGapCost, new SubCostRange1ToMinus2()) {}

        /// <summary>
        /// constructor
        /// </summary>
        /// <param name="costG">the cost of a gap</param>
        public SmithWaterman(double costG) : this(costG, new SubCostRange1ToMinus2()) {}

        /// <summary>
        /// constructor
        /// </summary>
        /// <param name="costG">the cost of a gap</param>
        /// <param name="costFunction">the cost function to use</param>
        public SmithWaterman(double costG, AbstractSubstitutionCost costFunction) {
            gapCost = costG;
            dCostFunction = costFunction;
        }

        /// <summary>
        /// constructor
        /// </summary>
        /// <param name="costFunction">the cost function to use</param>
        public SmithWaterman(AbstractSubstitutionCost costFunction) : this(defaultGapCost, costFunction) {}

        /// <summary>
        /// the private cost function used in the levenstein distance.
        /// </summary>
        AbstractSubstitutionCost dCostFunction;
        /// <summary>
        /// the cost of a gap.
        /// </summary>
        double gapCost;

        /// <summary>
        /// gets the similarity of the two strings using Smith Waterman distance.
        /// </summary>
        /// <param name="firstWord"></param>
        /// <param name="secondWord"></param>
        /// <returns>a value between 0-1 of the similarity</returns>
        public override double GetSimilarity(string firstWord, string secondWord) {
            if ((firstWord != null) && (secondWord != null)) {
                double smithWaterman = GetUnnormalisedSimilarity(firstWord, secondWord);
                double maxValue = Math.Min(firstWord.Length, secondWord.Length);
                if (dCostFunction.MaxCost > -gapCost) {
                    maxValue *= dCostFunction.MaxCost;
                }
                else {
                    maxValue *= (-gapCost);
                }
                if (maxValue == defaultMismatchScore) {
                    return defaultPerfectMatchScore;
                }
                else {
                    return smithWaterman / maxValue;
                }
            }
            return defaultMismatchScore;
        }

        /// <summary> gets a div class xhtml similarity explaining the operation of the metric.</summary>
        /// <param name="firstWord">string 1</param>
        /// <param name="secondWord">string 2</param>
        /// <returns> a div class html section detailing the metric operation.</returns>
        public override string GetSimilarityExplained(string firstWord, string secondWord) {
            throw new NotImplementedException();
        }

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
                return (firstLength * secondLength + firstLength + secondLength) * estimatedTimingConstant;
            }
            return 0.0;
        }

        /// <summary> 
        /// gets the un-normalised similarity measure of the metric for the given strings.</summary>
        /// <param name="firstWord"></param>
        /// <param name="secondWord"></param>
        /// <returns> returns the score of the similarity measure (un-normalised)</returns>
        public override double GetUnnormalisedSimilarity(string firstWord, string secondWord) {
            if ((firstWord != null) && (secondWord != null)) {
                int n = firstWord.Length;
                int m = secondWord.Length;
                if (n == 0) {
                    return m;
                }
                if (m == 0) {
                    return n;
                }
                double[][] d = new double[n][];
                for (int i = 0; i < n; i++) {
                    d[i] = new double[m];
                }
                double maxSoFar = defaultMismatchScore;
                for (int i = 0; i < n; i++) {
                    double cost = dCostFunction.GetCost(firstWord, i, secondWord, 0);
                    if (i == 0) {
                        d[0][0] = MathFunctions.MaxOf3(defaultMismatchScore, -gapCost, cost);
                    }
                    else {
                        d[i][0] = MathFunctions.MaxOf3(defaultMismatchScore, d[i - 1][0] - gapCost, cost);
                    }
                    if (d[i][0] > maxSoFar) {
                        maxSoFar = d[i][0];
                    }
                }

                for (int j = 0; j < m; j++) {
                    double cost = dCostFunction.GetCost(firstWord, 0, secondWord, j);
                    if (j == 0) {
                        d[0][0] = MathFunctions.MaxOf3(defaultMismatchScore, -gapCost, cost);
                    }
                    else {
                        d[0][j] = MathFunctions.MaxOf3(defaultMismatchScore, d[0][j - 1] - gapCost, cost);
                    }
                    if (d[0][j] > maxSoFar) {
                        maxSoFar = d[0][j];
                    }
                }

                for (int i = 1; i < n; i++) {
                    for (int j = 1; j < m; j++) {
                        double cost = dCostFunction.GetCost(firstWord, i, secondWord, j);
                        d[i][j] =
                            MathFunctions.MaxOf4(defaultMismatchScore, d[i - 1][j] - gapCost, d[i][j - 1] - gapCost,
                                                 d[i - 1][j - 1] + cost);
                        if (d[i][j] > maxSoFar) {
                            maxSoFar = d[i][j];
                        }
                    }
                }

                return maxSoFar;
            }
            return 0.0;
        }

        /// <summary>
        /// get the d(i,j) cost function.
        /// </summary>
        public AbstractSubstitutionCost DCostFunction { get { return dCostFunction; } set { DCostFunction = value; } }

        /// <summary>
        /// the gap cost for the distance function.
        /// </summary>
        public double GapCost { get { return gapCost; } set { gapCost = value; } }

        /// <summary>
        /// returns the long string identifier for the metric.
        /// </summary>
        public override string LongDescriptionString { get { return "Implements the Smith-Waterman algorithm providing a similarity measure between two string"; } }

        /// <summary>
        /// returns the string identifier for the metric .
        /// </summary>
        public override string ShortDescriptionString { get { return "SmithWaterman"; } }
    }
}
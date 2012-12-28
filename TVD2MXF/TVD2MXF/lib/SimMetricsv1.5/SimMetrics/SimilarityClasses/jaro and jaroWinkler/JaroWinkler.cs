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
    /// implements the Jaro Winkler string metric
    /// </summary>
    [Serializable]
    sealed public class JaroWinkler : AbstractStringMetric {
        /// <summary>
        /// maximum prefix length to use.
        /// </summary>
        /// <remarks>changed from the original 6 to 4 to match the original definition of JaroWinkler
        /// Chris Parkinson .NET 2.0 implementation only</remarks>
        const int minPrefixTestLength = 4;

        /// <summary>
        /// prefix adjustment scale.
        /// </summary>
        const double prefixAdustmentScale = 0.1F;

        /// <summary>
        /// constructor
        /// </summary>
        public JaroWinkler() {
            jaroStringMetric = new Jaro();
        }

        /// <summary>
        /// a constant for calculating the estimated timing cost.
        /// </summary>
        double estimatedTimingConstant = 4.342e-005F;

        /// <summary>
        /// private string metric allowing internal metric to be composed.
        /// </summary>
        AbstractStringMetric jaroStringMetric;

        /// <summary>
        /// gets the similarity measure of the JaroWinkler metric for the given strings.
        /// </summary>
        /// <param name="firstWord"></param>
        /// <param name="secondWord"></param>
        /// <returns>0-1 similarity measure of the JaroWinkler metric</returns>
        public override double GetSimilarity(string firstWord, string secondWord) {
            if ((firstWord != null) && (secondWord != null)) {
                double dist = jaroStringMetric.GetSimilarity(firstWord, secondWord);
                int prefixLength = GetPrefixLength(firstWord, secondWord);
                return dist + prefixLength * prefixAdustmentScale * (1.0 - dist);
            }
            return 0.0;
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
                return firstLength * secondLength * estimatedTimingConstant;
            }
            return 0.0;
        }

        /// <summary> 
        /// gets the un-normalised similarity measure of the metric for the given strings.</summary>
        /// <param name="firstWord"></param>
        /// <param name="secondWord"></param>
        /// <returns> returns the score of the similarity measure (un-normalised)</returns>
        public override double GetUnnormalisedSimilarity(string firstWord, string secondWord) {
            return GetSimilarity(firstWord, secondWord);
        }

        /// <summary>
        /// gets the prefix length found of common characters at the begining of the strings.
        /// </summary>
        /// <param name="firstWord"></param>
        /// <param name="secondWord"></param>
        /// <returns>the prefix length found of common characters at the begining of the strings</returns>
        static int GetPrefixLength(string firstWord, string secondWord) {
            if ((firstWord != null) && (secondWord != null)) {
                int n = MathFunctions.MinOf3(minPrefixTestLength, firstWord.Length, secondWord.Length);
                for (int i = 0; i < n; i++) {
                    if (firstWord[i] != secondWord[i]) {
                        return i;
                    }
                }

                return n;
            }
            return minPrefixTestLength;
        }

        /// <summary>
        /// returns the long string identifier for the metric.
        /// </summary>
        public override string LongDescriptionString {
            get {
                return
                    "Implements the Jaro-Winkler algorithm providing a similarity measure between two strings allowing character transpositions to a degree adjusting the weighting for common prefixes";
            }
        }

        /// <summary>
        /// returns the string identifier for the metric.
        /// </summary>
        public override string ShortDescriptionString { get { return "JaroWinkler"; } }
    }
}
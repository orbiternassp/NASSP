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

using System;
[assembly : CLSCompliant(true)]

namespace SimMetricsMetricUtilities {
    using System.Text;
    using SimMetricsApi;

    /// <summary>
    /// implements the Jaro string Metric.
    /// </summary>
    [Serializable]
    sealed public class Jaro : AbstractStringMetric {
        const double defaultMismatchScore = 0.0;
        /// <summary>
        /// a constant for calculating the estimated timing cost.
        /// </summary>
        double estimatedTimingConstant = 4.12e-005F;

        /// <summary>
        /// gets the similarity of the two strings using Jaro distance.
        /// </summary>
        /// <param name="firstWord"></param>
        /// <param name="secondWord"></param>
        /// <returns>a value between 0-1 of the similarity</returns>
        public override double GetSimilarity(string firstWord, string secondWord) {
            if ((firstWord != null) && (secondWord != null)) {
                //get half the length of the string rounded up - (this is the distance used for acceptable transpositions)
                int halflen = Math.Min(firstWord.Length, secondWord.Length) / 2 + 1;
                //get common characters
                StringBuilder common1 = GetCommonCharacters(firstWord, secondWord, halflen);
                int commonMatches = common1.Length;
                //check for zero in common
                if (commonMatches == 0) {
                    return defaultMismatchScore;
                }
                StringBuilder common2 = GetCommonCharacters(secondWord, firstWord, halflen);
                //check for same length common strings returning 0.0f is not the same
                if (commonMatches != common2.Length) {
                    return defaultMismatchScore;
                }
                //get the number of transpositions
                int transpositions = 0;
                for (int i = 0; i < commonMatches; i++) {
                    if (common1[i] != common2[i]) {
                        transpositions++;
                    }
                }

                //calculate jaro metric
                transpositions /= 2;
                double tmp1;
                tmp1 = commonMatches / (3.0 * firstWord.Length) + commonMatches / (3.0 * secondWord.Length) +
                       (commonMatches - transpositions) / (3.0 * commonMatches);
                return tmp1;
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
        /// returns a string buffer of characters from string1 within string2 if they are of a given
        /// distance seperation from the position in string1.
        /// </summary>
        /// <param name="firstWord">string one</param>
        /// <param name="secondWord">string two</param>
        /// <param name="distanceSep">separation distance</param>
        /// <returns>a string buffer of characters from string1 within string2 if they are of a given
        /// distance seperation from the position in string1</returns>
        static StringBuilder GetCommonCharacters(string firstWord, string secondWord, int distanceSep) {
            if ((firstWord != null) && (secondWord != null)) {
                StringBuilder returnCommons = new StringBuilder();
                StringBuilder copy = new StringBuilder(secondWord);
                for (int i = 0; i < firstWord.Length; i++) {
                    char ch = firstWord[i];
                    bool foundIt = false;
                    for (int j = Math.Max(0, i - distanceSep);
                         !foundIt && j < Math.Min(i + distanceSep, secondWord.Length);
                         j++) {
                        if (copy[j] == ch) {
                            foundIt = true;
                            returnCommons.Append(ch);
                            copy[j] = '#';
                        }
                    }
                }

                return returnCommons;
            }
            return null;
        }

        /// <summary>
        /// returns the long string identifier for the metric.
        /// </summary>
        public override string LongDescriptionString {
            get {
                return
                    "Implements the Jaro algorithm providing a similarity measure between two strings allowing character transpositions to a degree";
            }
        }

        /// <summary>
        /// returns the string identifier for the metric.
        /// </summary>
        public override string ShortDescriptionString { get { return "Jaro"; } }
    }
}
#region Copyright
/*
 * The original .NET implementation of the SimMetrics library is taken from the Java
 * source and converted to NET using the Microsoft Java converter.
 * It is not clear who made the initial convertion to .NET.
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
    using System.Collections.ObjectModel;
    using SimMetricsApi;
    using SimMetricsUtilities;

    /// <summary>
    ///a block distance implementation metric 
    /// </summary>
    [Serializable]
    sealed public class BlockDistance : AbstractStringMetric {
        /// <summary>
        /// constructor - default (empty).
        /// </summary>
        public BlockDistance() : this(new TokeniserWhitespace()) {}

        /// <summary>
        /// constructor
        /// </summary>
        /// <param name="tokeniserToUse">the tokeniser to use should a different tokeniser be required</param>
        public BlockDistance(ITokeniser tokeniserToUse) {
            tokeniser = tokeniserToUse;
            tokenUtilities = new TokeniserUtilities<string>();
        }

        /// <summary>
        /// a constant for calculating the estimated timing cost.
        /// </summary>
        double estimatedTimingConstant = 6.445714e-005F;
        /// <summary>
        /// 
        /// </summary>
        TokeniserUtilities<string> tokenUtilities;
        /// <summary>
        /// tokeniser for tokenisation of the query strings.
        /// </summary>
        ITokeniser tokeniser;

        /// <summary>
        /// gets the similarity of the two strings using BlockDistance.
        /// </summary>
        /// <param name="firstWord"></param>
        /// <param name="secondWord"></param>
        /// <returns>a 0-1 similarity score</returns>
        public override double GetSimilarity(string firstWord, string secondWord) {
            Collection<string> firstTokens = tokeniser.Tokenize(firstWord);
            Collection<string> secondTokens = tokeniser.Tokenize(secondWord);
            int totalPossible = firstTokens.Count + secondTokens.Count;
            double totalDistance = GetActualSimilarity(firstTokens, secondTokens);
            return (totalPossible - totalDistance) / totalPossible;
        }

        /// <summary>
        /// gets a div class xhtml similarity explaining the operation of the metric. 
        /// </summary>
        /// <param name="firstWord">string 1</param>
        /// <param name="secondWord">string 2</param>
        /// <returns> a div class html section detailing the metric operation.</returns>
        public override string GetSimilarityExplained(string firstWord, string secondWord) {
            throw new NotImplementedException();
        }

        /// <summary>
        /// gets the estimated time in milliseconds it takes to perform a similarity timing.
        /// </summary>
        /// <param name="firstWord">string 1</param>
        /// <param name="secondWord">string 2</param>
        /// <returns>the estimated time in milliseconds taken to perform the similarity measure</returns>
        public override double GetSimilarityTimingEstimated(string firstWord, string secondWord) {
            double firstTokens = tokeniser.Tokenize(firstWord).Count;
            double secondTokens = tokeniser.Tokenize(secondWord).Count;
            return
                ((firstTokens + secondTokens) * firstTokens + (firstTokens + secondTokens) * secondTokens) *
                estimatedTimingConstant;
        }

        /// <summary> 
        /// gets the un-normalised similarity measure of the metric for the given strings.</summary>
        /// <param name="firstWord"></param>
        /// <param name="secondWord"></param>
        /// <returns> returns the score of the similarity measure (un-normalised)</returns>
        public override double GetUnnormalisedSimilarity(string firstWord, string secondWord) {
            Collection<string> firstTokens = tokeniser.Tokenize(firstWord);
            Collection<string> secondTokens = tokeniser.Tokenize(secondWord);
            return GetActualSimilarity(firstTokens, secondTokens);
        }

        double GetActualSimilarity(Collection<string> firstTokens, Collection<string> secondTokens) {
            Collection<string> allTokens = tokenUtilities.CreateMergedList(firstTokens, secondTokens);

            int totalDistance = 0;

            foreach (string token in allTokens) {
                int countInfirstWord = 0;
                int countInsecondWord = 0;
                if (firstTokens.Contains(token)) {
                    countInfirstWord++;
                }
                if (secondTokens.Contains(token)) {
                    countInsecondWord++;
                }

                if (countInfirstWord > countInsecondWord) {
                    totalDistance += countInfirstWord - countInsecondWord;
                }
                else {
                    totalDistance += countInsecondWord - countInfirstWord;
                }
            }
            return totalDistance;
        }

        /// <summary>
        /// returns the long string identifier for the metric.
        /// </summary>
        public override string LongDescriptionString {
            get {
                return
                    "Implements the Block distance algorithm whereby vector space block distance is used to determine a similarity";
            }
        }

        /// <summary>
        /// returns the string identifier for the metric.
        /// </summary>
        public override string ShortDescriptionString { get { return "BlockDistance"; } }
    }
}
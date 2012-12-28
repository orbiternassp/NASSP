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
 * measures between String Data. All metrics return consistant measures
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

namespace SimMetricsUtilities {
    using System;
    using System.Collections.ObjectModel;
    using SimMetricsApi;

    /// <summary>
    /// SubCostRange5ToMinus3 implements a cost function as used in Monge Elkan where by an exact match
    /// no match or an approximate match whereby a set of characters are in an approximate range.
    /// for pairings in {dt} {gj} {lr} {mn} {bpv} {aeiou} {,.}
    /// </summary>
    [Serializable]
    sealed public class SubCostRange5ToMinus3 : AbstractSubstitutionCost {
        const int charApproximateMatchScore = 3;
        const int charExactMatchScore = 5;
        const int charMismatchMatchScore = -3;

        /// <summary>
        /// constructor
        /// Sets up the matching sets
        /// approximate match = +3,
        /// for pairings in {dt} {gj} {lr} {mn} {bpv} {aeiou} {,.}.
        /// </summary>
        public SubCostRange5ToMinus3() {
            {
                approx = new Collection<string>[7];
                approx[0] = new Collection<string>();
                approx[0].Add("d");
                approx[0].Add("t");
                approx[1] = new Collection<string>();
                approx[1].Add("g");
                approx[1].Add("j");
                approx[2] = new Collection<string>();
                approx[2].Add("l");
                approx[2].Add("r");
                approx[3] = new Collection<string>();
                approx[3].Add("m");
                approx[3].Add("n");
                approx[4] = new Collection<string>();
                approx[4].Add("b");
                approx[4].Add("p");
                approx[4].Add("v");
                approx[5] = new Collection<string>();
                approx[5].Add("a");
                approx[5].Add("e");
                approx[5].Add("i");
                approx[5].Add("o");
                approx[5].Add("u");
                approx[6] = new Collection<string>();
                approx[6].Add(",");
                approx[6].Add(".");
            }
        }

        /// <summary>
        /// approximate character set.
        /// </summary>
        Collection<string>[] approx;

        /// <summary>
        /// get cost between characters where
        /// d(i,j) = charExactMatchScore if i equals j,
        /// charApproximateMatchScore if i approximately equals j or
        /// charMismatchMatchScore if i does not equal j.
        /// </summary>
        /// <param name="firstWord">the string1 to evaluate the cost</param>
        /// <param name="firstWordIndex">the index within the string1 to test</param>
        /// <param name="secondWord">the string2 to evaluate the cost</param>
        /// <param name="secondWordIndex">the index within the string2 to test</param>
        /// <returns>the cost of a given subsitution d(i,j) as defined above</returns>
        public override double GetCost(String firstWord, int firstWordIndex, String secondWord, int secondWordIndex) {
            if ((firstWord != null) && (secondWord != null)) {
                if (firstWord.Length <= firstWordIndex || firstWordIndex < 0) {
                    return charMismatchMatchScore;
                }
                if (secondWord.Length <= secondWordIndex || secondWordIndex < 0) {
                    return charMismatchMatchScore;
                }
                if (firstWord[firstWordIndex] == secondWord[secondWordIndex]) {
                    return charExactMatchScore;
                }

                string si = firstWord[firstWordIndex].ToString().ToLowerInvariant();
                string ti = secondWord[secondWordIndex].ToString().ToLowerInvariant();
                for (int i = 0; i < approx.Length; i++) {
                    if (approx[i].Contains(si) && approx[i].Contains(ti)) {
                        return charApproximateMatchScore;
                    }
                }
            }
            return charMismatchMatchScore;
        }

        /// <summary>
        /// returns the maximum possible cost.
        /// </summary>
        public override double MaxCost { get { return charExactMatchScore; } }

        /// <summary>
        /// returns the minimum possible cost.
        /// </summary>
        public override double MinCost { get { return charMismatchMatchScore; } }

        /// <summary>
        /// returns the name of the cost function.
        /// </summary>
        public override String ShortDescriptionString { get { return "SubCostRange5ToMinus3"; } }
    }
}
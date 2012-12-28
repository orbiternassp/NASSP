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

namespace SimMetricsUtilities {
    using System;
    using System.Collections.Generic;
    using System.Text;
    using SimMetricsApi;

    /// <summary>
    /// implements a tokeniser splitting the string into qGrams extending beyond the ends of the string input, using padding characters.
    /// </summary>
    [Serializable]
    public sealed class TokeniserQGram3ExtendedOriginal : ITokeniser {
        #region constructor(s)
        /// <summary>
        /// constructor
        /// </summary>
        public TokeniserQGram3ExtendedOriginal() {
            stopWordHandler = new DummyStopTermHandler();
            tokenUtilities = new TokeniserUtilities<string>();
        }
        #endregion

        #region private fields, methods and constants
        /// <summary>
        /// padding used in q gram calculation. 
        /// </summary>
        char qGramEndPadding = '#';

        /// <summary>
        /// Length of each qGram
        /// </summary>
        int qGramLength = 3;

        /// <summary>
        /// padding used in q gram calculation.
        /// </summary>
        char qGramStartPadding = '#';

        /// <summary>
        /// stopWordHandler used by the tokenisation.
        /// </summary>
        ITermHandler stopWordHandler;

        TokeniserUtilities<string> tokenUtilities;
        #endregion

        #region public methods
        /// <summary>
        /// Return tokenized version of a string.
        /// </summary>
        /// <param name="word">input</param>
        /// <returns>tokenized version of a string</returns>
        public List<string> Tokenize(string word) {
            List<string> returnVect = new List<string>(10);
            if (word != null) {
                StringBuilder adjustedString = new StringBuilder();
                adjustedString.Append(qGramStartPadding);
                adjustedString.Append(qGramStartPadding);
                adjustedString.Append(word);
                adjustedString.Append(qGramEndPadding);
                adjustedString.Append(qGramEndPadding);
                int curPos = 0;
                for (int length = adjustedString.Length - 2; curPos < length; curPos++) {
                    string term = adjustedString.ToString(curPos, qGramLength);
                    if (!stopWordHandler.IsWord(term)) {
                        returnVect.Add(term);
                    }
                }
            }
            return returnVect;
        }

        /// <summary>
        /// Return tokenized set of a string.
        /// </summary>
        /// <param name="word">input</param>
        /// <returns>tokenized version of a string as a set</returns>
        public List<string> TokenizeToSet(string word) {
            if (word != null) {
                return tokenUtilities.AddList(Tokenize(word));
            }
            return null;
        }
        #endregion

        #region public properties
        /// <summary>
        /// displays the delimiters used  - ie none.
        /// </summary>
        public string Delimiters { get { return ""; } }

        /// <summary>
        /// displays the tokenisation method.
        /// </summary>
        public string ShortDescriptionString { get { return "TokeniserQGram3Extended"; } }

        /// <summary>
        /// the stop word handler used.
        /// </summary>
        public ITermHandler StopWordHandler { get { return stopWordHandler; } set { stopWordHandler = value; } }
        #endregion
    }
}
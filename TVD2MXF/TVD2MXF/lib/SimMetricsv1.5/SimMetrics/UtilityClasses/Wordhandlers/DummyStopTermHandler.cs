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
    using System.Text;
    using SimMetricsApi;

    /// <summary>
    /// DummyStopTermHandler implements a dummy stop word handling function whereby no stopwords are considered.
    /// </summary>
    sealed public class DummyStopTermHandler : ITermHandler {
        /// <summary>
        /// adds a word to the intewrface.
        /// </summary>
        /// <param name="termToAdd">termToAdd the word to add</param>
        public void AddWord(string termToAdd) {}

        /// <summary>
        /// isStopWord determines if a given term is a stop word or not.
        /// </summary>
        /// <param name="termToTest">termToTest the term to test</param>
        /// <returns>always returns false.</returns>
        public bool IsWord(string termToTest) {
            return false;
        }

        /// <summary>
        /// removes the given stopword from the list.
        /// </summary>
        /// <param name="termToRemove">termToRemove the stopword term to remove</param>
        public void RemoveWord(string termToRemove) {}

        /// <summary>
        /// gets the number of stopwords in the list.
        /// </summary>
        public int NumberOfWords { get { return 0; } }

        /// <summary>
        /// displays the stopWordHandler method.
        /// </summary>
        public string ShortDescriptionString { get { return "DummyStopTermHandler"; } }

        /// <summary>
        /// gets the stopwords as a stringBuffer.
        /// </summary>
        public StringBuilder WordsAsBuffer { get { return new StringBuilder(); } }
    }
}
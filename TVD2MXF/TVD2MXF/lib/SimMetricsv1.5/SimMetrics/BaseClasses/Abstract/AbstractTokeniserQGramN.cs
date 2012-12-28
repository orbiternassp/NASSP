#region Copyright
/* This new class in the .NET version holds abstract tokeniser class.
 * 
 * (c) Chris Parkinson 2006.
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

namespace SimMetricsApi {
    using System;
    using System.Collections.ObjectModel;
    using System.Text;
    using SimMetricsUtilities;

    /// <summary>
    /// Implements a QGram Tokeniser to cope with all gram sizes.
    /// </summary>
    /// <remarks>The cci value determines at what level the skip characters
    /// are gathered. This is a variation of the normal QGram analysis when
    /// character pairs are created having skipped characters in the words.</remarks>
    [Serializable]
    public abstract class AbstractTokeniserQGramN : ITokeniser {
        const string defaultEndPadCharacter = "#";
        const string defaultStartPadCharacter = "?";
        int characterCombinationIndex;
        int qGramLength;
        ITermHandler stopWordHandler;
        string suppliedWord;
        TokeniserUtilities<string> tokenUtilities;

        /// <summary>
        /// Return tokenized version of a string.
        /// </summary>
        /// <param name="word">input</param>
        /// <returns>tokenized version of a string</returns>
        public abstract Collection<string> Tokenize(string word);

        /// <summary>
        /// full version of Tokenise which allows for different token lengths 
        /// as well as the characterCombinationIndexValue error level as well.
        /// </summary>
        /// <param name="word">word to tokenise</param>
        /// <param name="extended">whether to generate extended tokens</param>
        /// <param name="tokenLength">length of tokens</param>
        /// <param name="characterCombinationIndexValue">error level for skip tokens</param>
        /// <returns>collection of tokens</returns>
        public Collection<string> Tokenize(string word, bool extended, int tokenLength, int characterCombinationIndexValue) {
            if (!String.IsNullOrEmpty(word)) {
                SuppliedWord = word;
                Collection<string> anArray = new Collection<string>();
                int wordLength = word.Length;
                int maxValue = 0;
                if (tokenLength > 0) {
                    maxValue = (tokenLength - 1);
                }
                StringBuilder testword = new StringBuilder(wordLength + (2 * maxValue));
                if (extended) {
                    testword.Insert(0, defaultStartPadCharacter, maxValue);
                }
                testword.Append(word);
                if (extended) {
                    testword.Insert(testword.Length, defaultEndPadCharacter, maxValue);
                }

                // normal n-gram keys characterCombinationIndex = 0
                string testWordOne = testword.ToString();
                int maxLoop;
                if (extended) {
                    maxLoop = wordLength + maxValue;
                }
                else {
                    maxLoop = wordLength - tokenLength + 1;
                }
                for (int i = 0; i < maxLoop; i++) {
                    string testWord = testWordOne.Substring(i, tokenLength);
                    if (!stopWordHandler.IsWord(testWord)) {
                        anArray.Add(testWord);
                    }
                }

                if (characterCombinationIndexValue != 0) {
                    // special characterCombinationIndex n-gram keys
                    testWordOne = testword.ToString();
                    maxLoop -= 1; // have to reduce by 1 as we are skipping a letter
                    for (int i = 0; i < maxLoop; i++) {
                        string testWord = testWordOne.Substring(i, maxValue) + testWordOne.Substring(i + tokenLength, 1);
                        if (!stopWordHandler.IsWord(testWord)) {
                            if (!anArray.Contains(testWord)) {
                                anArray.Add(testWord);
                            }
                        }
                    }
                }
                return anArray;
            }
            return null;
        }

        /// <summary>
        /// Return tokenized set of a string.
        /// </summary>
        /// <param name="word">input</param>
        /// <returns>tokenized version of a string as a set</returns>
        public Collection<string> TokenizeToSet(string word) {
            if (!String.IsNullOrEmpty(word)) {
                SuppliedWord = word;
                return TokenUtilities.CreateSet(Tokenize(word));
            }
            return null;
        }

        /// <summary>
        /// CCI - error level used for the sgram analysis. 
        /// </summary>
        /// <description>A value of 1 means the sgram will skip a letter when generating the tokens</description>
        public int CharacterCombinationIndex { get { return characterCombinationIndex; } set { characterCombinationIndex = value; } }

        /// <summary>
        /// displays the delimiters used  - ie none.
        /// </summary>
        public string Delimiters { get { return String.Empty; } }

        /// <summary>
        /// length of the qgram tokens to create
        /// </summary>
        public int QGramLength { get { return qGramLength; } set { qGramLength = value; } }

        /// <summary>
        /// displays the tokenisation method.
        /// </summary>
        public abstract string ShortDescriptionString { get; }

        /// <summary>
        ///  the stop word handler used.
        /// </summary>
        public ITermHandler StopWordHandler { get { return stopWordHandler; } set { stopWordHandler = value; } }

        /// <summary>
        /// supplied word
        /// </summary>
        public string SuppliedWord { get { return suppliedWord; } set { suppliedWord = value; } }

        /// <summary>
        /// class containing token utilities
        /// </summary>
        public TokeniserUtilities<string> TokenUtilities { get { return tokenUtilities; } set { tokenUtilities = value; } }
    }
}
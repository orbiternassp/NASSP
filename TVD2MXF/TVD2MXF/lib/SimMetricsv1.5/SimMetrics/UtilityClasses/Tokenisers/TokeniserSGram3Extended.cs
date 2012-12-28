#region Copyright
/* This new class in the .NET version holds a Bigram implementation.
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

namespace SimMetricsUtilities {
    using System;

    /// <summary>
    /// implementation of a SGram tokeniser using extended logic
    /// </summary>
    public class TokeniserSGram3Extended : TokeniserQGram3Extended {
        /// <summary>
        /// constructor
        /// </summary>
        public TokeniserSGram3Extended() : base() {
            CharacterCombinationIndex = 1;
        }

        /// <summary>
        /// displays the tokenisation method.
        /// </summary>
        public override string ShortDescriptionString { get { return "TokeniserSGram3Extended"; } }

        /// <summary>
        /// override the ToString method to give accurate information on current settings
        /// </summary>
        /// <returns>details of current tokeniser</returns>
        public override string ToString() {
            if (String.IsNullOrEmpty(SuppliedWord)) {
                return string.Format("{0} : no word passed for tokenising yet.", ShortDescriptionString);
            }
            else {
                if (CharacterCombinationIndex == 0) {
                    return
                        string.Format("{0} - currently holding : {1}.{2}The method is using a QGram length of {3}.",
                                      ShortDescriptionString, SuppliedWord, Environment.NewLine, Convert.ToInt32(QGramLength));
                }
                else {
                    return
                        string.Format(
                            "{0} - currently holding : {1}.{2}The method is using a character combination index of {3} and {4}a QGram length of {5}.",
                            ShortDescriptionString, SuppliedWord, Environment.NewLine,
                            Convert.ToInt32(CharacterCombinationIndex), Environment.NewLine, Convert.ToInt32(QGramLength));
                }
            }
        }
    }
}
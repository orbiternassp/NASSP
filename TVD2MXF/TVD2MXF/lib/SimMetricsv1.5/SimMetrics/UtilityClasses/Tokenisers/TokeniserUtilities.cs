#region Copyright
/* This new class in the .NET version holds utility functions for use with the
 * various Collection<T> token collections.
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
    using System.Collections.ObjectModel;

    /// <summary>
    /// class containing utility functions for the tokenisers to use. 
    /// these are in two main version
    /// collections or sets
    /// a collection can contain the same value multiple times ad set can only have the value once.
    /// </summary>
    /// <typeparam name="T">type for token collection</typeparam>
    [Serializable]
    public class TokeniserUtilities<T> {
        /// <summary>
        /// constructor
        /// </summary>
        public TokeniserUtilities() {
            allTokens = new Collection<T>();
            tokenSet = new Collection<T>();
        }

        Collection<T> allTokens;
        int firstSetTokenCount;
        int firstTokenCount;
        int secondSetTokenCount;
        int secondTokenCount;
        Collection<T> tokenSet;

        /// <summary>
        /// returns the number of common tokens from the two supplied token sets
        /// </summary>
        /// <returns></returns>
        public int CommonSetTerms() {
            return FirstSetTokenCount + SecondSetTokenCount - tokenSet.Count;
        }

        /// <summary>
        /// returns number of common tokens from the two supplied token collections
        /// </summary>
        /// <returns></returns>
        public int CommonTerms() {
            return FirstTokenCount + SecondTokenCount - allTokens.Count;
        }

        /// <summary>
        ///  method to merge two token lists to keep all tokens
        /// </summary>
        /// <param name="firstTokens">first token list</param>
        /// <param name="secondTokens">second token list</param>
        /// <returns>list of all tokens</returns>
        public Collection<T> CreateMergedList(Collection<T> firstTokens, Collection<T> secondTokens) {
            allTokens.Clear();
            firstTokenCount = firstTokens.Count;
            secondTokenCount = secondTokens.Count;
            MergeLists(firstTokens);
            MergeLists(secondTokens);
            return allTokens;
        }

        /// <summary>
        ///  method to merge two token lists to keep only unique tokens
        /// </summary>
        /// <param name="firstTokens">first token list</param>
        /// <param name="secondTokens">second token list</param>
        /// <returns>list of unique tokens</returns>
        public Collection<T> CreateMergedSet(Collection<T> firstTokens, Collection<T> secondTokens) {
            tokenSet.Clear();
            firstSetTokenCount = CalculateUniqueTokensCount(firstTokens);
            secondSetTokenCount = CalculateUniqueTokensCount(secondTokens);
            MergeIntoSet(firstTokens);
            MergeIntoSet(secondTokens);
            return tokenSet;
        }

        /// <summary>
        ///  method to create a single token list of unique tokens
        /// </summary>
        /// <param name="tokenList">token list to use</param>
        /// <returns>unique token list - sorted</returns>
        public Collection<T> CreateSet(Collection<T> tokenList) {
            tokenSet.Clear();
            AddUniqueTokens(tokenList);
            firstTokenCount = tokenSet.Count;
            secondTokenCount = 0;
            return tokenSet;
        }

        /// <summary>
        /// method for merging extra token lists into the set
        /// </summary>
        /// <param name="firstTokens">token list to merge</param>
        public void MergeIntoSet(Collection<T> firstTokens) {
            AddUniqueTokens(firstTokens);
        }

        /// <summary>
        /// method for merging into the list
        /// </summary>
        /// <param name="firstTokens">token list to merge</param>
        public void MergeLists(Collection<T> firstTokens) {
            AddTokens(firstTokens);
        }

        void AddTokens(Collection<T> tokenList) {
            foreach (T token in tokenList) {
                allTokens.Add(token);
            }
        }

        void AddUniqueTokens(Collection<T> tokenList) {
            foreach (T token in tokenList) {
                if (!tokenSet.Contains(token)) {
                    tokenSet.Add(token);
                }
            }
        }

        int CalculateUniqueTokensCount(Collection<T> tokenList) {
            Collection<T> myList = new Collection<T>();
            foreach (T token in tokenList) {
                if (!myList.Contains(token)) {
                    myList.Add(token);
                }
            }
            return myList.Count;
        }

        /// <summary>
        /// token count from first token list
        /// </summary>
        public int FirstSetTokenCount { get { return firstSetTokenCount; } }

        /// <summary>
        /// token count from first token list
        /// </summary>
        public int FirstTokenCount { get { return firstTokenCount; } }

        /// <summary>
        /// merged token List. unique tokens only
        /// </summary>
        public Collection<T> MergedTokens { get { return allTokens; } }

        /// <summary>
        /// token count from second token list
        /// </summary>
        public int SecondSetTokenCount { get { return secondSetTokenCount; } }

        /// <summary>
        /// token count from second token list
        /// </summary>
        public int SecondTokenCount { get { return secondTokenCount; } }

        /// <summary>
        /// merged token List. unique tokens only
        /// </summary>
        public Collection<T> TokenSet { get { return tokenSet; } }
    }
}
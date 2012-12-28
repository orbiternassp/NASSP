namespace SimMetricsMetricUtilities {
    using System;

    [Serializable]
    public sealed class ChapmanMatchingSoundex : MongeElkan
    {
        /*
        public ChapmanMatchingSoundex() : base(new Soundex()) {}

        public ChapmanMatchingSoundex(ITokeniser tokeniserToUse) : base(tokeniserToUse, new Soundex()) {}
          */

        private double estimatedTimingConstant = 0.02657143F;

        public override double GetSimilarityTimingEstimated(String firstWord, String secondWord) {
            double firstTokens = tokeniser.Tokenize(firstWord).Count;
            double secondTokens = tokeniser.Tokenize(secondWord).Count;
            return
                (tokeniser.Tokenize(firstWord).Count + tokeniser.Tokenize(secondWord).Count) *
                ((firstTokens + secondTokens) * estimatedTimingConstant);
        }

        public override String LongDescriptionString {
            get {
                return
                    "Implements the Chapman Matching Soundex algorithm whereby terms are matched and tested against the standard soundex algorithm - this is intended to provide a better rating for lists of proper names.";
            }
        }

        public override String ShortDescriptionString {
            get { return "ChapmanMatchingSoundex"; }
        }
    }
}
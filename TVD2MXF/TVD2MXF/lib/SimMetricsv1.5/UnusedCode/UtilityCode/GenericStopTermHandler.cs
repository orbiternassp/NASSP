namespace SimMetricsUtilities {
    using System;
    using System.Text;
    using SimMetricsApi;

    public sealed class GenericStopTermHandler : ITermHandler {
        public GenericStopTermHandler() {
            InitBlock();
        }

        private SupportClass.SetSupportCollection wordSet;

        public void AddWord(String termToAdd) {
            wordSet.Add(termToAdd);
        }

        public bool IsWord(String termToTest) {
            return wordSet.Contains(termToTest);
        }

        public void RemoveWord(String termToRemove) {
            wordSet.Remove(termToRemove);
        }

        private void InitBlock() {
            wordSet = new SupportClass.HashSetSupportCollection();
        }

        public int NumberOfWords {
            get { return wordSet.Count; }
        }

        public String ShortDescriptionString {
            get { return "GenericStopTermHandler"; }
        }

        public StringBuilder WordsAsBuffer {
            get {
                StringBuilder outputBuffer = new StringBuilder();
                outputBuffer.Append(wordSet.ToArray().ToString());
                return outputBuffer;
            }
        }
    }
}
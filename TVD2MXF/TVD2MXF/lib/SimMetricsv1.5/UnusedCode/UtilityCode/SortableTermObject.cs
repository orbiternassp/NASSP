namespace SimMetricsUtilities {
    using System;
    using System.Collections;

    [Serializable]
    internal sealed class SortableTermObject : IComparer
    {
        public SortableTermObject(String termToSet, int frequencyToSet) {
            term = termToSet;
            frequency = frequencyToSet;
        }

        private int frequency;
        private String term;

        public int Compare(Object o1, Object o2) {
            SortableTermObject localObject1 = (SortableTermObject)o1;
            SortableTermObject localObject2 = (SortableTermObject)o2;
            if (localObject1.frequency > localObject2.frequency) {
                return 1;
            }
            return localObject1.frequency >= localObject2.frequency ? 0 : -1;
        }

        public int Frequency {
            get { return frequency; }
        }

        public String Term {
            get { return term; }
        }
    }
}
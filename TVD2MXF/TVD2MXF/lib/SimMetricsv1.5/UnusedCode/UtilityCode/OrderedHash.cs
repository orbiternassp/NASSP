namespace SimMetricsUtilities {
    using System;
    using System.Collections;
    using System.Runtime.Serialization;

    [Serializable]
    internal sealed class OrderedHash : Hashtable, ISerializable {
        public OrderedHash() {
            InitBlock();
        }

        private ArrayList orderVector;
        private ArrayList valueVector;

        public Object Put(Object key, Object value_Renamed) {
            if (!orderVector.Contains(key)) {
                orderVector.Add(key);
            }
            if (value_Renamed is ArrayList) {
                valueVector.AddRange((ArrayList)value_Renamed);
            }
            else {
                valueVector.Add(value_Renamed);
            }
            return SupportClass.PutElement(this, key, value_Renamed);
        }

        private void InitBlock() {
            orderVector = new ArrayList(10);
            valueVector = new ArrayList(10);
        }

        public ArrayList OrderedKeys {
            get { return orderVector; }
        }

        public ArrayList OrderedValues {
            get { return valueVector; }
        }
    }
}
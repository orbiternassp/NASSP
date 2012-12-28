namespace SimMetricsUtilities {
    using System;
    using System.Collections;
    using SimMetricsApi;

    [Serializable]
    public sealed class MeanMetricArbitrator : IMetricArbitrator
    {
        public MeanMetricArbitrator() {
            metricsForArbitration = new ArrayList(10);
        }

        private ArrayList metricsForArbitration;

        public void AddArbitrationMetric(IStringMetric arbitrationMetric) {
            metricsForArbitration.Add(arbitrationMetric);
        }

        public void AddArbitrationMetrics(ArrayList arbitrationMetrics) {
            metricsForArbitration.AddRange(arbitrationMetrics);
        }

        public void ClearArbitrationMetrics() {
            metricsForArbitration.Clear();
        }

        public double GetArbitrationScore(String firstWord, String secondWord) {
            double score = 0.0F;
            if (metricsForArbitration.Count == 0) {
                return score;
            }
            //UPGRADE_TODO: Method 'java.util.Iterator.hasNext' was converted to 'System.Collections.IEnumerator.MoveNext' which has a different behavior. 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="jlca1073_javautilIteratorhasNext"'
            for (IEnumerator metricIt = metricsForArbitration.GetEnumerator(); metricIt.MoveNext();) {
                //UPGRADE_TODO: Method 'java.util.Iterator.next' was converted to 'System.Collections.IEnumerator.Current' which has a different behavior. 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="jlca1073_javautilIteratornext"'
                score += ((IStringMetric)metricIt.Current).GetSimilarity(firstWord, secondWord);
            }

            //UPGRADE_WARNING: Narrowing conversions may produce unexpected results in C#. 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="jlca1042"'
            return score / (double)metricsForArbitration.Count;
        }

        public long GetArbitrationTimingActual(String firstWord, String secondWord) {
            //UPGRADE_TODO: Method 'java.lang.System.currentTimeMillis' was converted to 'System.DateTime.Now' which has a different behavior. 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="jlca1073_javalangSystemcurrentTimeMillis"'
            long timeBefore = (DateTime.Now.Ticks - 621355968000000000) / 10000;
            GetArbitrationScore(firstWord, secondWord);
            //UPGRADE_TODO: Method 'java.lang.System.currentTimeMillis' was converted to 'System.DateTime.Now' which has a different behavior. 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="jlca1073_javalangSystemcurrentTimeMillis"'
            long timeAfter = (DateTime.Now.Ticks - 621355968000000000) / 10000;
            return timeAfter - timeBefore;
        }

        public double GetArbitrationTimingEstimated(String firstWord, String secondWord) {
            double estimatedTime = 0.0F;
            //UPGRADE_TODO: Method 'java.util.Iterator.hasNext' was converted to 'System.Collections.IEnumerator.MoveNext' which has a different behavior. 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="jlca1073_javautilIteratorhasNext"'
            for (IEnumerator metricIt = metricsForArbitration.GetEnumerator(); metricIt.MoveNext();) {
                //UPGRADE_TODO: Method 'java.util.Iterator.next' was converted to 'System.Collections.IEnumerator.Current' which has a different behavior. 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="jlca1073_javautilIteratornext"'
                estimatedTime += ((IStringMetric)metricIt.Current).GetSimilarityTimingEstimated(firstWord, secondWord);
            }

            return estimatedTime;
        }

        public ArrayList ArbitrationMetrics {
            get { return metricsForArbitration; }

            set { metricsForArbitration = value; }
        }

        public String LongDescriptionString {
            get { return "MeanMetricArbitrator gives equal weightings too all metrics and returns an arbitrated score for all"; }
        }

        public String ShortDescriptionString {
            get { return "MeanMetricArbitrator"; }
        }
    }
}
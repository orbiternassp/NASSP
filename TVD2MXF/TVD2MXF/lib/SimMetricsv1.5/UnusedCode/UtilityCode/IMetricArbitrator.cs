namespace SimMetricsApi {
    using System;
    using System.Collections;

    /// <summary>
    /// InterfaceMetricArbitrator provides an interface for a metric arbitrator.
    /// </summary>
    /// <namespace>SimMetricsApi</namespace>
    /// <date>29-Apr-2004</date>
    /// <author>Sam Chapman <a href="http://www.dcs.shef.ac.uk/~sam/">Website</a>, <a href="mailto:sam@dcs.shef.ac.uk">Email</a>.</author>
    /// <version>v 1.1</version>
    public interface IMetricArbitrator {
        /// <summary>
        /// adds an InterfaceStringMetric to those used for arbitration.
        /// </summary>
        /// <param name="arbitrationMetric">arbitrationMetric of InterfaceStringMetric to be added to those used for arbitration.</param>
        void AddArbitrationMetric(IStringMetric arbitrationMetric);

        /// <summary>
        /// adds an individual arbitration metric.
        /// </summary>
        /// <param name="arbitrationMetrics">arbitrationMetric an InterfaceStringMetric to add to the arbitrated metrics.</param>
        void AddArbitrationMetrics(ArrayList arbitrationMetrics);

        /// <summary>
        /// removes all arbitration metrics.
        /// </summary>
        void ClearArbitrationMetrics();

        /// <summary>
        /// returns an arbitrated value of similarity.
        /// </summary>
        /// <param name="firstWord"></param>
        /// <param name="secondWord"></param>
        /// <returns>a float between zero to one (zero = no similarity, one = matching strings)</returns>
        double GetArbitrationScore(String firstWord, String secondWord);

        /// <summary>
        /// gets the actual time in milliseconds it takes to perform an Arbitration.
        /// This call takes as long as the underlying metrics to perform so should not be performed in normal circumstances.</summary>
        /// <param name="firstWord"></param>
        /// <param name="secondWord"></param>
        /// <returns>the actual time in milliseconds taken to perform the MetricArbitrator</returns>
        long GetArbitrationTimingActual(String firstWord, String secondWord);

        /// <summary>
        /// gets the estimated time in milliseconds it takes to perform the arbitration.
        /// </summary>
        /// <param name="firstWord"></param>
        /// <param name="secondWord"></param>
        /// <returns>the estimated time in milliseconds taken to perform the similarity measure</returns>
        double GetArbitrationTimingEstimated(String firstWord, String secondWord);

        /// <summary>
        /// gets the arbitration metrics used.
        /// set arbitration metrics with those given.
        /// </summary>
        /// <value>arbitrationMetrics a vector of InterfaceStringMetric's to be used in the arbitration</value>
        ArrayList ArbitrationMetrics { get; set; }

        /// <summary>
        /// returns a long string of the MetricArbitrator description.
        /// </summary>
        String LongDescriptionString { get; }

        /// <summary>
        /// returns a string of the MetricArbitrator name.
        /// </summary>
        String ShortDescriptionString { get; }
    }
}
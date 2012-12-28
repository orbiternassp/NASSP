using System;
using System.Collections;
using System.Text;
using SimMetricsApi;
using SimMetricsMetricUtilities;
using SimMetricsUtilities;

public sealed class TestEstimatedTimes {
    private const int TESTMAXLENGTHTIMINGSTEPSIZE = 50;
    private const int TESTMAXLENGTHTIMINGTERMLENGTH = 10;
    private const int TESTMAXLENGTHTIMINGTEST = 2000;
    private const int TESTTIMINGMILLISECONDSPERTEST = 200;

    public TestEstimatedTimes() {}

    [STAThread]
    public static void Main(String[] args) {
        ArrayList testMetricVector = new ArrayList(10);
        testMetricVector.Add(new Levenstein());
        testMetricVector.Add(new NeedlemanWunch());
        testMetricVector.Add(new SmithWaterman());
        testMetricVector.Add(new ChapmanLengthDeviation());
        testMetricVector.Add(new ChapmanMeanLength());
        testMetricVector.Add(new SmithWatermanGotoh());
        testMetricVector.Add(new SmithWatermanGotohWindowedAffine());
        testMetricVector.Add(new BlockDistance());
        testMetricVector.Add(new MongeElkan());
        testMetricVector.Add(new Jaro());
        testMetricVector.Add(new JaroWinkler());
        testMetricVector.Add(new ChapmanMatchingSoundex());
        testMetricVector.Add(new MatchingCoefficient());
        testMetricVector.Add(new DiceSimilarity());
        testMetricVector.Add(new JaccardSimilarity());
        testMetricVector.Add(new OverlapCoefficient());
        testMetricVector.Add(new EuclideanDistance());
        testMetricVector.Add(new CosineSimilarity());
        testMetricVector.Add(new QGramsDistance());
        testMethod(testMetricVector, args);
    }

    private static void testMethod(ArrayList metricVector, String[] args) {
        Console.Out.WriteLine("Usage: testMethod ");
        Console.Out.WriteLine("AS NO INPUT - running defualt test\n");
        Console.Out.WriteLine("Performing Tests with Following Metrics:");
        for (int i = 0; i < metricVector.Count; i++) {
            Console.Out.WriteLine("m" + (i + 1) + " " + ((AbstractStringMetric)metricVector[i]).ShortDescriptionString);
        }

        Console.Out.WriteLine();
        Console.Out.Write("\n");
        int metricTests = 0;
        long totalTime = (DateTime.Now.Ticks - 621355968000000000) / 10000;
        for (int i = 0; i < metricVector.Count; i++) {
            AbstractStringMetric metric = (AbstractStringMetric)metricVector[i];
            Console.Out.Write("m" + (i + 1) + "\t");
            StringBuilder testString = new StringBuilder();
            int termLen = 0;
            for (int len = 1; len < 2000;) {
                if (termLen < 10) {
                    testString.Append((char)(97 + (int)(SupportClass.Random.NextDouble() * 25D)));
                }
                else {
                    testString.Append(' ');
                    termLen = 0;
                }
                len++;
                termLen++;
            }

            for (int len = 1; len < 2000; len += 50) {
                long timeTaken = 0L;
                int iterations = 0;
                String input1 = testString.ToString(0, len);
                while (timeTaken < 200L) {
                    timeTaken += metric.GetSimilarityTimingActual(input1, input1);
                    iterations++;
                    metricTests++;
                }
                Console.Out.Write(Convert.ToDecimal((float)timeTaken / (float)iterations) + "(" + metric.GetSimilarityTimingEstimated(input1, input1) +
                                  ")\t");
            }

            Console.Out.Write("\t(" + metric.ShortDescriptionString + ") - testsSoFar = " + metricTests + "\n");
        }

        totalTime = (DateTime.Now.Ticks - 621355968000000000) / 10000 - totalTime;
        Console.Out.WriteLine("\nTotal Metrics Tests = " + metricTests + " in " + totalTime + "ms\t\t meaning " +
                              Convert.ToDecimal((float)metricTests / (float)totalTime) + " tests per millisecond");
    }
}
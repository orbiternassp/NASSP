using System;
using System.Collections;
using SimMetricsApi;
using SimMetricsMetricUtilities;
using SimMetricsUtilities;

public sealed class TestArbitrators {
    public TestArbitrators() {}

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
        //testMetricVector.Add(new Soundex());
        testMetricVector.Add(new ChapmanMatchingSoundex());
        testMetricVector.Add(new MatchingCoefficient());
        testMetricVector.Add(new DiceSimilarity());
        testMetricVector.Add(new JaccardSimilarity());
        testMetricVector.Add(new OverlapCoefficient());
        testMetricVector.Add(new EuclideanDistance());
        testMetricVector.Add(new CosineSimilarity());
        testMetricVector.Add(new QGramsDistance());
        IMetricArbitrator arbitrator = new MeanMetricArbitrator();
        arbitrator.ArbitrationMetrics = testMetricVector;
        testMethod(arbitrator);
    }

    private static void testMethod(IMetricArbitrator arbitrator) {
        Console.Out.WriteLine("Performing Arbitrartion with: " + arbitrator.ShortDescriptionString);
        Console.Out.WriteLine("Using the Following Test Cases:");
        for (int i = 0; i < testCases.Length; i++) {
            Console.Out.WriteLine("t" + (i + 1) + " \"" + testCases[i][0] + "\" vs \"" + testCases[i][1] + "\"");
        }

        Console.Out.WriteLine();
        //UPGRADE_ISSUE: Class 'java.text.DecimalFormat' was not converted. 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="jlca1000_javatextDecimalFormat"'
        //UPGRADE_ISSUE: Constructor 'java.text.DecimalFormat.DecimalFormat' was not converted. 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="jlca1000_javatextDecimalFormat"'
        //DecimalFormat df = new DecimalFormat("0.00");
        for (int j = 0; j < testCases.Length; j++) {
            float result = arbitrator.GetArbitrationScore(testCases[j][0], testCases[j][1]);
            Console.Out.WriteLine(Convert.ToDecimal(result) + " for \"" + testCases[j][0] + "\" vs \"" + testCases[j][1] + "\"");
        }
    }

    private const String string1 = "Sam J Chapman";
    private const String string2 = "Samuel Chapman";
    private const String string3 = "S Chapman";
    private const String string4 = "Samuel John Chapman";
    private const String string5 = "John Smith";
    private const String string6 = "Richard Smith";
    private const String string7 = "aaaa mnop zzzz";
    private const String string8 = "bbbb mnop yyyy";
    private const String string9 = "aa mnop zzzzzz";
    private const String string10 = "a";
    private const String string11 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    private const String string12 = "aaaaa bcdefgh mmmmmmmm stuvwx zzzzzz";
    private const String string13 = "jjjjj bcdefgh qqqqqqqq stuvwx yyyyyy";
    private const String string14 = "aaaaa bcdefgh stuvwx zzzzzz";
    private const String string15 = "aaaaa aaaaa aaaaa zzzzzz";
    private const String string16 = "aaaaa aaaaa";

    private static readonly String[][] testCases = {
                                                       new String[] { "Sam J Chapman", "Samuel Chapman" }, new String[] { "Sam J Chapman", "S Chapman" }
                                                       , new String[] { "Samuel Chapman", "S Chapman" },
                                                       new String[] { "Sam J Chapman", "Sam J Chapman" },
                                                       new String[] { "Samuel John Chapman", "John Smith" },
                                                       new String[] { "John Smith", "Richard Smith" }, new String[] { "John Smith", "Sam J Chapman" },
                                                       new String[] { "Sam J Chapman", "Richard Smith" },
                                                       new String[] { "Sam J Chapman", "Samuel John Chapman" },
                                                       new String[] { "Samuel Chapman", "Samuel John Chapman" },
                                                       new String[] { "aaaa mnop zzzz", "bbbb mnop yyyy" },
                                                       new String[] { "aaaa mnop zzzz", "aa mnop zzzzzz" },
                                                       new String[] { "bbbb mnop yyyy", "aa mnop zzzzzz" }, new String[] { "a", "a" },
                                                       new String[]
                                                           {
                                                               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
                                                               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                                           },
                                                       new String[] { "a", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" },
                                                       new String[] { "aaaaa bcdefgh mmmmmmmm stuvwx zzzzzz", "jjjjj bcdefgh qqqqqqqq stuvwx yyyyyy" },
                                                       new String[] { "aaaaa bcdefgh mmmmmmmm stuvwx zzzzzz", "aaaaa bcdefgh stuvwx zzzzzz" },
                                                       new String[] { "aaaaa bcdefgh stuvwx zzzzzz", "aaaaa aaaaa aaaaa zzzzzz" },
                                                       new String[] { "aaaaa aaaaa", "aaaaa aaaaa" }
                                                   };
}
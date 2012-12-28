using System;
using SimMetricsApi;
using SimMetricsMetricUtilities;

public sealed class TestMetrics {
    /* public TestMetrics()
	{
	
	} */

    [STAThread]
    public static void Main(String[] args) {
        //clifton added
        AbstractStringMetric metric = new CosineSimilarity();
        float result = metric.GetSimilarity("clifton phua", "cliff pua clifton");
        Console.Out.WriteLine(result); //Display the string. 
    }

    /* public static void main(String args[])
	{
	Vector testMetricVector = new Vector();
	//testMetricVector.add(new Levenstein());
	//testMetricVector.add(new NeedlemanWunch());
	//testMetricVector.add(new SmithWaterman());
	//testMetricVector.add(new ChapmanLengthDeviation());
	//testMetricVector.add(new ChapmanMeanLength());
	//testMetricVector.add(new SmithWatermanGotoh());
	//testMetricVector.add(new SmithWatermanGotohWindowedAffine());
	//testMetricVector.add(new BlockDistance());
	//testMetricVector.add(new MongeElkan());
	//testMetricVector.add(new Jaro());
	//testMetricVector.add(new JaroWinkler());
	//testMetricVector.add(new Soundex());
	//testMetricVector.add(new ChapmanMatchingSoundex());
	//testMetricVector.add(new MatchingCoefficient());
	//testMetricVector.add(new DiceSimilarity());
	testMetricVector.add(new JaccardSimilarity())
	//testMetricVector.add(new OverlapCoefficient());
	//testMetricVector.add(new EuclideanDistance());
	//testMetricVector.add(new CosineSimilarity());
	//testMetricVector.add(new QGramsDistance());
	
	testMethod(testMetricVector, args);
	}
	
	private static void testMethod(Vector metricVector, String args[])
	{
	boolean useCmdArgs = false;
	boolean testTimingComplexity = false;
	if(args.length == 1)
	testTimingComplexity = true;
	else
	if(args.length == 2)
	{
	useCmdArgs = true;
	} else
	{
	System.out.println("Usage: testMethod \"String1 to Test\" \"String2 to test\"");
	System.out.println("or");
	System.out.println("Usage: testMethod \"timing");
	System.out.println("AS NO INPUT - running defualt test cases\n");
	}
	System.out.println("Performing Tests with Following Metrics:");
	for(int i = 0; i < metricVector.size(); i++)
	System.out.println("m" + (i + 1) + " " + ((AbstractStringMetric)metricVector.get(i)).GetShortDescriptionString());
	
	System.out.println();
	if(!useCmdArgs)
	{
	System.out.println("Using the Following Test Cases:");
	for(int i = 0; i < testCases.length; i++)
	System.out.println("t" + (i + 1) + " \"" + testCases[i][0] + "\" vs \"" + testCases[i][1] + "\"");
	
	System.out.println();
	} else
	{
	System.out.println("Using the Input Test Case:");
	System.out.println("t1 \"" + args[0] + "\" vs \"" + args[1] + "\"");
	System.out.println();
	}
	System.out.print("  \t");
	if(!useCmdArgs)
	{
	for(int j = 0; j < testCases.length; j++)
	if(j < 9)
	System.out.print("t" + (j + 1) + "=\t (t" + (j + 1) + "ms)\t");
	else
	System.out.print("t" + (j + 1) + "= (t" + (j + 1) + "ms)");
	
	} else
	{
	System.out.print("t1");
	}
	System.out.print("\n");
	DecimalFormat df = new DecimalFormat("0.00");
	int metricTests = 0;
	long totalTime = System.currentTimeMillis();
	for(int i = 0; i < metricVector.size(); i++)
	{
	AbstractStringMetric metric = (AbstractStringMetric)metricVector.get(i);
	System.out.print("m" + (i + 1) + "\t");
	if(testTimingComplexity)
	{
	StringBuffer testString = new StringBuffer();
	int termLen = 0;
	for(int len = 1; len < 3000;)
	{
	if(termLen < 10)
	{
	testString.append((char)(97 + (int)(Math.random() * 25D)));
	} else
	{
	testString.append(' ');
	termLen = 0;
	}
	len++;
	termLen++;
	}
	
	for(int len = 1; len < 3000; len += 50)
	{
	long timeTaken = 0L;
	int iterations = 0;
	String input1 = testString.substring(0, len);
	while(timeTaken < 200L) 
	{
	timeTaken += metric.getSimilarityTimingActual(input1, input1);
	iterations++;
	metricTests++;
	}
	System.out.print(df.format((float)timeTaken / (float)iterations) + "\t");
	}
	
	} else
	if(!useCmdArgs)
	{
	for(int j = 0; j < testCases.length; j++)
	{
	float result = metric.getSimilarity(testCases[j][0], testCases[j][1]);
	metricTests++;
	long timeTaken = 0L;
	int iterations = 0;
	while(timeTaken < 200L) 
	{
	timeTaken += metric.getSimilarityTimingActual(testCases[j][0], testCases[j][1]);
	iterations++;
	metricTests++;
	}
	System.out.print(df.format(result) + " (" + df.format((float)timeTaken / (float)iterations) + ")\t");
	}
	
	} else
	{
	float result = metric.getSimilarity(args[0], args[1]);
	metricTests++;
	long timeTaken = 0L;
	int iterations = 0;
	while(timeTaken < 250L) 
	{
	timeTaken += metric.getSimilarityTimingActual(args[0], args[1]);
	iterations++;
	metricTests++;
	}
	System.out.print(df.format(result) + " (" + df.format((float)timeTaken / (float)iterations) + ")\t");
	}
	System.out.print("\t(" + metric.GetShortDescriptionString() + ") - testsSoFar = " + metricTests + "\n");
	}
	
	totalTime = System.currentTimeMillis() - totalTime;
	System.out.println("\nTotal Metrics Tests = " + metricTests + " in " + totalTime + "ms\t\t meaning " + df.format((float)metricTests / (float)totalTime) + " tests per millisecond");
	}
	
	private static final int TESTTIMINGMILLISECONDSPERTEST = 200;
	private static final int TESTMAXLENGTHTIMINGTEST = 3000;
	private static final int TESTMAXLENGTHTIMINGSTEPSIZE = 50;
	private static final int TESTMAXLENGTHTIMINGTERMLENGTH = 10;
	private static final String string1 = "Sam J Chapman";
	private static final String string2 = "Samuel Chapman";
	private static final String string3 = "S Chapman";
	private static final String string4 = "Samuel John Chapman";
	private static final String string5 = "John Smith";
	private static final String string6 = "Richard Smith";
	private static final String string7 = "aaaa mnop zzzz";
	private static final String string8 = "bbbb mnop yyyy";
	private static final String string9 = "aa mnop zzzzzz";
	private static final String string10 = "a";
	private static final String string11 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	private static final String string12 = "aaaaa bcdefgh mmmmmmmm stuvwx zzzzzz";
	private static final String string13 = "jjjjj bcdefgh qqqqqqqq stuvwx yyyyyy";
	private static final String string14 = "aaaaa bcdefgh stuvwx zzzzzz";
	private static final String string15 = "aaaaa aaaaa aaaaa zzzzzz";
	private static final String string16 = "aaaaa aaaaa";
	private static final String testCases[][] = {
	{
	"Sam J Chapman", "Samuel Chapman"
	}, {
	"Sam J Chapman", "S Chapman"
	}, {
	"Samuel Chapman", "S Chapman"
	}, {
	"Sam J Chapman", "Sam J Chapman"
	}, {
	"Samuel John Chapman", "John Smith"
	}, {
	"John Smith", "Richard Smith"
	}, {
	"John Smith", "Sam J Chapman"
	}, {
	"Sam J Chapman", "Richard Smith"
	}, {
	"Sam J Chapman", "Samuel John Chapman"
	}, {
	"Samuel Chapman", "Samuel John Chapman"
	}, {
	"aaaa mnop zzzz", "bbbb mnop yyyy"
	}, {
	"aaaa mnop zzzz", "aa mnop zzzzzz"
	}, {
	"bbbb mnop yyyy", "aa mnop zzzzzz"
	}, {
	"a", "a"
	}, {
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
	}, {
	"a", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
	}, {
	"aaaaa bcdefgh mmmmmmmm stuvwx zzzzzz", "jjjjj bcdefgh qqqqqqqq stuvwx yyyyyy"
	}, {
	"aaaaa bcdefgh mmmmmmmm stuvwx zzzzzz", "aaaaa bcdefgh stuvwx zzzzzz"
	}, {
	"aaaaa bcdefgh stuvwx zzzzzz", "aaaaa aaaaa aaaaa zzzzzz"
	}, {
	"aaaaa aaaaa", "aaaaa aaaaa"
	}
	}; */
}
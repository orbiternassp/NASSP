namespace SimMetrics.UnitTestsForSimMetrics.EditDistanceTests {
    using System;
    using System.Collections.Generic;
    using NUnit.Framework;
    using SimMetricsMetricUtilities;
    using SimMetricsUtilities;
    using UnitTestsForSimmetrics.Properties;

    [TestFixture]
    ///
        /// 
    sealed public class SmithWatermanUnitTests {
        #region Test Data Setup
        struct swTestRecord {
            public string nameOne;
            public string nameTwo;
            public double swDefault;
            public double swCost;
            public double swCostFunction;
            public double swCostAndCostFunction;
        }

        struct swgTestRecord {
            public string nameOne;
            public string nameTwo;
            public double swgDefault;
            public double swgGapCostFunction;
            public double swgCostFunction;
            public double swgGapCostAndCostFunctions;
        }

        struct swgaTestRecord {
            public string nameOne;
            public string nameTwo;
            public double swgaDefault;
            public double swgaWindowSize;
            public double swgaGapCostFunction;
            public double swgaGapCostFunctionAndWindowSize;
            public double swgaGapCostAndCostFunctions;
            public double swgaGapCostAndCostFunctionsAndWindowSize;
            public double swgaCostFunction;
            public double swgaCostFunctionAndWindowSize;
        }

        Settings addressSettings = Settings.Default;
        List<swTestRecord> swTestNames = new List<swTestRecord>(26);
        List<swgTestRecord> swgTestNames = new List<swgTestRecord>(26);
        List<swgaTestRecord> swgaTestNames = new List<swgaTestRecord>(26);

        void swAddNames(string addChars) {
            if (addChars != null) {
                string[] letters = addChars.Split(',');
                swTestRecord testName;
                testName.nameOne = letters[0];
                testName.nameTwo = letters[1];
                testName.swDefault = Convert.ToDouble(letters[2]);
                testName.swCost = Convert.ToDouble(letters[3]);
                testName.swCostFunction = Convert.ToDouble(letters[4]);
                testName.swCostAndCostFunction = Convert.ToDouble(letters[5]);
                swTestNames.Add(testName);
            }
        }

        void swgAddNames(string addChars) {
            if (addChars != null) {
                string[] letters = addChars.Split(',');
                swgTestRecord testName;
                testName.nameOne = letters[0];
                testName.nameTwo = letters[1];
                testName.swgDefault = Convert.ToDouble(letters[2]);
                testName.swgGapCostFunction = Convert.ToDouble(letters[3]);
                testName.swgCostFunction = Convert.ToDouble(letters[4]);
                testName.swgGapCostAndCostFunctions = Convert.ToDouble(letters[5]);
                swgTestNames.Add(testName);
            }
        }

        void swgaAddNames(string addChars) {
            if (addChars != null) {
                string[] letters = addChars.Split(',');
                swgaTestRecord testName;
                testName.nameOne = letters[0];
                testName.nameTwo = letters[1];
                testName.swgaDefault = Convert.ToDouble(letters[2]);
                testName.swgaWindowSize = Convert.ToDouble(letters[3]);
                testName.swgaGapCostFunction = Convert.ToDouble(letters[4]);
                testName.swgaGapCostFunctionAndWindowSize = Convert.ToDouble(letters[5]);
                testName.swgaGapCostAndCostFunctions = Convert.ToDouble(letters[6]);
                testName.swgaGapCostAndCostFunctionsAndWindowSize = Convert.ToDouble(letters[7]);
                testName.swgaCostFunction = Convert.ToDouble(letters[8]);
                testName.swgaCostFunctionAndWindowSize = Convert.ToDouble(letters[9]);
                swgaTestNames.Add(testName);
            }
        }

        void LoadData() {
            swAddNames(addressSettings.swName1);
            swAddNames(addressSettings.swName2);
            swAddNames(addressSettings.swName3);
            swAddNames(addressSettings.swName4);
            swAddNames(addressSettings.swName5);
            swAddNames(addressSettings.swName6);
            swAddNames(addressSettings.swName7);

            swgAddNames(addressSettings.swgName1);
            swgAddNames(addressSettings.swgName2);
            swgAddNames(addressSettings.swgName3);
            swgAddNames(addressSettings.swgName4);
            swgAddNames(addressSettings.swgName5);
            swgAddNames(addressSettings.swgName6);
            swgAddNames(addressSettings.swgName7);

            swgaAddNames(addressSettings.swgaName1);
            swgaAddNames(addressSettings.swgaName2);
            swgaAddNames(addressSettings.swgaName3);
            swgaAddNames(addressSettings.swgaName4);
            swgaAddNames(addressSettings.swgaName5);
            swgaAddNames(addressSettings.swgaName6);
            swgaAddNames(addressSettings.swgaName7);
            swgaAddNames(addressSettings.swgaName8);
        }
        #endregion

        #region SmithWaterman Tests
        [Test]
        [Category("SmithWaterman")]
        public void SmithWatermanShortDescription() {
            Assert.AreEqual(mySmithWatermanDefault.ShortDescriptionString, "SmithWaterman",
                            "Problem with SmithWaterman test ShortDescription");
        }

        [Test]
        [Category("SmithWaterman")]
        public void SmithWatermanDefaultTestData() {
            foreach (swTestRecord testRecord in swTestNames) {
                Assert.AreEqual(testRecord.swDefault.ToString("F3"),
                                mySmithWatermanDefault.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with SmithWaterman test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }

        [Test]
        [Category("SmithWaterman")]
        public void SmithWatermanCostTestData() {
            foreach (swTestRecord testRecord in swTestNames) {
                Assert.AreEqual(testRecord.swDefault.ToString("F3"),
                                mySmithWatermanCost.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with SmithWaterman test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }

        [Test]
        [Category("SmithWaterman")]
        public void SmithWatermanCostFunctionTestData() {
            foreach (swTestRecord testRecord in swTestNames) {
                Assert.AreEqual(testRecord.swDefault.ToString("F3"),
                                mySmithWatermanCostFunction.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with SmithWaterman test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }

        [Test]
        [Category("SmithWaterman")]
        public void SmithWatermanCostAndCostFunctionTestData() {
            foreach (swTestRecord testRecord in swTestNames) {
                Assert.AreEqual(testRecord.swDefault.ToString("F3"),
                                mySmithWatermanCostAndCostFunction.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).
                                    ToString("F3"),
                                "Problem with SmithWaterman test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }
        #endregion

        #region SmithWatermanGotoh tests
        [Test]
        [Category("SmithWatermanGotoh")]
        public void SmithWatermanGotohShortDescription() {
            Assert.AreEqual(mySmithWatermanGotohDefault.ShortDescriptionString, "SmithWatermanGotoh",
                            "Problem with SmithWaterman test ShortDescription");
        }

        [Test]
        [Category("SmithWatermanGotoh")]
        public void SmithWatermanGotohDefaultTestData() {
            foreach (swgTestRecord testRecord in swgTestNames) {
                Assert.AreEqual(testRecord.swgDefault.ToString("F3"),
                                mySmithWatermanGotohDefault.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with SmithWatermanGotoh test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }

        [Test]
        [Category("SmithWatermanGotoh")]
        public void SmithWatermanGotoGapCostFunctionTestData() {
            foreach (swgTestRecord testRecord in swgTestNames) {
                Assert.AreEqual(testRecord.swgGapCostFunction.ToString("F3"),
                                mySmithWatermanGotohGapCostFunction.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).
                                    ToString("F3"),
                                "Problem with SmithWatermanGotoh test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }

        [Test]
        [Category("SmithWatermanGotoh")]
        public void SmithWatermanGotoCostFunctionTestData() {
            foreach (swgTestRecord testRecord in swgTestNames) {
                Assert.AreEqual(testRecord.swgCostFunction.ToString("F3"),
                                mySmithWatermanGotohCostFunction.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString(
                                    "F3"),
                                "Problem with SmithWatermanGotoh test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }

        [Test]
        [Category("SmithWatermanGotoh")]
        public void SmithWatermanGotoGapCostAndCostFunctionsTestData() {
            foreach (swgTestRecord testRecord in swgTestNames) {
                Assert.AreEqual(testRecord.swgGapCostAndCostFunctions.ToString("F3"),
                                mySmithWatermanGotohGapCostAndCostFunctions.GetSimilarity(testRecord.nameOne, testRecord.nameTwo)
                                    .ToString("F3"),
                                "Problem with SmithWatermanGotoh test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }
        #endregion

        #region SmithWatermanGotohWindowedAffine tests
        [Test]
        [Category("SWGWA")]
        public void SWGWAShortDescription() {
            Assert.AreEqual(mySWGWADefault.ShortDescriptionString, "SmithWatermanGotohWindowedAffine",
                            "Problem with SmithWaterman test ShortDescription");
        }

        [Test]
        [Category("SWGWA")]
        public void SWGWADefaultTestData() {
            foreach (swgaTestRecord testRecord in swgaTestNames) {
                Assert.AreEqual(testRecord.swgaDefault.ToString("F3"),
                                mySWGWADefault.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with SWGWA test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }

        [Test]
        [Category("SWGWA")]
        public void SWGWAWindowSizeTestData() {
            foreach (swgaTestRecord testRecord in swgaTestNames) {
                Assert.AreEqual(testRecord.swgaWindowSize.ToString("F3"),
                                mySWGWAWindowSize.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with SWGWA test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }

        [Test]
        [Category("SWGWA")]
        public void SWGWAGapCostFunctionTestData() {
            foreach (swgaTestRecord testRecord in swgaTestNames) {
                Assert.AreEqual(testRecord.swgaGapCostFunction.ToString("F3"),
                                mySWGWAGapCostFunction.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with SWGWA test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }

        [Test]
        [Category("SWGWA")]
        public void SWGWAGapCostFunctionAndWindowSizeTestData() {
            foreach (swgaTestRecord testRecord in swgaTestNames) {
                Assert.AreEqual(testRecord.swgaGapCostFunctionAndWindowSize.ToString("F3"),
                                mySWGWAGapCostFunctionAndWindowSize.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).
                                    ToString("F3"), "Problem with SWGWA test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }

        [Test]
        [Category("SWGWA")]
        public void SWGWAGapCostAndCostFunctionsTestData() {
            foreach (swgaTestRecord testRecord in swgaTestNames) {
                Assert.AreEqual(testRecord.swgaGapCostAndCostFunctions.ToString("F3"),
                                mySWGWAGapCostAndCostFunctions.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString(
                                    "F3"), "Problem with SWGWA test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }

        [Test]
        [Category("SWGWA")]
        public void SWGWAGapCostAndCostFunctionsAndWindowSizeTestData() {
            foreach (swgaTestRecord testRecord in swgaTestNames) {
                Assert.AreEqual(testRecord.swgaGapCostAndCostFunctionsAndWindowSize.ToString("F3"),
                                mySWGWAGapCostAndCostFunctionsAndWindowSize.GetSimilarity(testRecord.nameOne, testRecord.nameTwo)
                                    .ToString("F3"),
                                "Problem with SWGWA test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }

        [Test]
        [Category("SWGWA")]
        public void SWGWACostFunctionTestData() {
            foreach (swgaTestRecord testRecord in swgaTestNames) {
                Assert.AreEqual(testRecord.swgaCostFunction.ToString("F3"),
                                mySWGWACostFunction.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with SWGWA test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }

        [Test]
        [Category("SWGWA")]
        public void SWGWACostFunctionAndWindowSizeTestData() {
            foreach (swgaTestRecord testRecord in swgaTestNames) {
                Assert.AreEqual(testRecord.swgaCostFunctionAndWindowSize.ToString("F3"),
                                mySWGWACostFunctionAndWindowSize.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString(
                                    "F3"), "Problem with SWGWA test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }
        #endregion

        #region private fields
        SmithWaterman mySmithWatermanDefault;
        SmithWaterman mySmithWatermanCost;
        SmithWaterman mySmithWatermanCostFunction;
        SmithWaterman mySmithWatermanCostAndCostFunction;

        SmithWatermanGotoh mySmithWatermanGotohDefault;
        SmithWatermanGotoh mySmithWatermanGotohGapCostFunction;
        SmithWatermanGotoh mySmithWatermanGotohCostFunction;
        SmithWatermanGotoh mySmithWatermanGotohGapCostAndCostFunctions;

        SmithWatermanGotohWindowedAffine mySWGWADefault;
        SmithWatermanGotohWindowedAffine mySWGWAWindowSize;
        SmithWatermanGotohWindowedAffine mySWGWAGapCostFunction;
        SmithWatermanGotohWindowedAffine mySWGWAGapCostFunctionAndWindowSize;
        SmithWatermanGotohWindowedAffine mySWGWAGapCostAndCostFunctions;
        SmithWatermanGotohWindowedAffine mySWGWAGapCostAndCostFunctionsAndWindowSize;
        SmithWatermanGotohWindowedAffine mySWGWACostFunction;
        SmithWatermanGotohWindowedAffine mySWGWACostFunctionAndWindowSize;
        #endregion

        [SetUp]
        public void SetUp() {
            LoadData();

            # region SmithWaterman classes
            // 0.5F and SubCostRange1ToMinus2 are the values used for the default constructor
            mySmithWatermanDefault = new SmithWaterman();
            mySmithWatermanCost = new SmithWaterman(0.5D);
            mySmithWatermanCostFunction = new SmithWaterman(new SubCostRange1ToMinus2());
            mySmithWatermanCostAndCostFunction = new SmithWaterman(0.5D, new SubCostRange1ToMinus2());
            #endregion

            // we also need to check running a different set of tests

            #region SmithWatermanGotoh classes
            mySmithWatermanGotohDefault = new SmithWatermanGotoh();
            mySmithWatermanGotohGapCostFunction = new SmithWatermanGotoh(new AffineGapRange5To0Multiplier1());
            mySmithWatermanGotohCostFunction = new SmithWatermanGotoh(new SubCostRange5ToMinus3());
            mySmithWatermanGotohGapCostAndCostFunctions =
                new SmithWatermanGotoh(new AffineGapRange5To0Multiplier1(), new SubCostRange5ToMinus3());
            #endregion

            #region SmithWatermanGotohWindowedAffine classes
            mySWGWADefault = new SmithWatermanGotohWindowedAffine();
            mySWGWAWindowSize = new SmithWatermanGotohWindowedAffine(100);
            mySWGWAGapCostFunction = new SmithWatermanGotohWindowedAffine(new AffineGapRange5To0Multiplier1());
            mySWGWAGapCostFunctionAndWindowSize = new SmithWatermanGotohWindowedAffine(new AffineGapRange5To0Multiplier1(), 100);
            mySWGWAGapCostAndCostFunctions =
                new SmithWatermanGotohWindowedAffine(new AffineGapRange5To0Multiplier1(), new SubCostRange5ToMinus3());
            mySWGWAGapCostAndCostFunctionsAndWindowSize =
                new SmithWatermanGotohWindowedAffine(new AffineGapRange5To0Multiplier1(), new SubCostRange5ToMinus3(), 100);
            mySWGWACostFunction = new SmithWatermanGotohWindowedAffine(new SubCostRange5ToMinus3());
            mySWGWACostFunctionAndWindowSize = new SmithWatermanGotohWindowedAffine(new SubCostRange5ToMinus3(), 100);
            #endregion
        }
    }
}
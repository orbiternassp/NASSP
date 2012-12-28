namespace SimMetrics.UnitTestsForSimMetrics.TokenBasedTests {
    using System;
    using System.Collections.Generic;
    using NUnit.Framework;
    using SimMetricsMetricUtilities;
    using UnitTestsForSimmetrics.Properties;

    [TestFixture]
    sealed public class JaccardSimilarityUnitTests {
        #region Test Data Setup
        struct TestRecord {
            public string nameOne;
            public string nameTwo;
            public double jaccardSimilarityMatchLevel;
        }

        Settings addressSettings = Settings.Default;
        List<TestRecord> testNames = new List<TestRecord>(26);

        void AddNames(string addChars) {
            if (addChars != null) {
                string[] letters = addChars.Split(',');
                TestRecord testName;
                testName.nameOne = letters[0];
                testName.nameTwo = letters[1];
                testName.jaccardSimilarityMatchLevel = Convert.ToDouble(letters[15]);
                testNames.Add(testName);
            }
        }

        void LoadData() {
            AddNames(addressSettings.blockDistance1);
            AddNames(addressSettings.blockDistance2);
            AddNames(addressSettings.blockDistance3);
        }
        #endregion

        #region JaccardSimilarity Tests
        [Test]
        [Category("JaccardSimilarity Test")]
        public void JaccardSimilarity_ShortDescription() {
            Assert.AreEqual("JaccardSimilarity", myJaccardSimilarity.ShortDescriptionString,
                            "Problem with JaccardSimilarity test short description.");
        }

        [Test]
        [Category("JaccardSimilarity Test")]
        public void JaccardSimilarity_TestData() {
            foreach (TestRecord testRecord in testNames) {
                Assert.AreEqual(testRecord.jaccardSimilarityMatchLevel.ToString("F3"),
                                myJaccardSimilarity.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with JaccardSimilarity test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }
        #endregion

        JaccardSimilarity myJaccardSimilarity;

        [SetUp]
        public void SetUp() {
            LoadData();
            myJaccardSimilarity = new JaccardSimilarity();
        }
    }
}
namespace SimMetrics.UnitTestsForSimMetrics.TokenBasedTests {
    using System;
    using System.Collections.Generic;
    using NUnit.Framework;
    using SimMetricsMetricUtilities;
    using UnitTestsForSimmetrics.Properties;

    [TestFixture]
    sealed public class DiceSimilarityUnitTests {
        #region Test Data Setup
        struct TestRecord {
            public string nameOne;
            public string nameTwo;
            public double diceSimilarityMatchLevel;
        }

        Settings addressSettings = Settings.Default;
        List<TestRecord> testNames = new List<TestRecord>(26);

        void AddNames(string addChars) {
            if (addChars != null) {
                string[] letters = addChars.Split(',');
                TestRecord testName;
                testName.nameOne = letters[0];
                testName.nameTwo = letters[1];
                testName.diceSimilarityMatchLevel = Convert.ToDouble(letters[13]);
                testNames.Add(testName);
            }
        }

        void LoadData() {
            AddNames(addressSettings.blockDistance1);
            AddNames(addressSettings.blockDistance2);
            AddNames(addressSettings.blockDistance3);
        }
        #endregion

        #region DiceSimilarity Tests
        [Test]
        [Category("DiceSimilarity Test")]
        public void DiceSimilarity_ShortDescription() {
            Assert.AreEqual("DiceSimilarity", myDiceSimilarity.ShortDescriptionString,
                            "Problem with DiceSimilarity test short description.");
        }

        [Test]
        [Category("DiceSimilarity Test")]
        public void DiceSimilarity_TestData() {
            foreach (TestRecord testRecord in testNames) {
                Assert.AreEqual(testRecord.diceSimilarityMatchLevel.ToString("F3"),
                                myDiceSimilarity.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with DiceSimilarity test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }
        #endregion

        DiceSimilarity myDiceSimilarity;

        [SetUp]
        public void SetUp() {
            LoadData();
            myDiceSimilarity = new DiceSimilarity();
        }
    }
}
namespace SimMetrics.UnitTestsForSimMetrics.TokenBasedTests {
    using System;
    using System.Collections.Generic;
    using NUnit.Framework;
    using SimMetricsMetricUtilities;
    using UnitTestsForSimmetrics.Properties;

    [TestFixture]
    sealed public class CosineSimilarityUnitTests {
        #region Test Data Setup
        struct TestRecord {
            public string nameOne;
            public string nameTwo;
            public double CosineSimilarityMatchLevel;
        }

        Settings addressSettings = Settings.Default;
        List<TestRecord> testNames = new List<TestRecord>(26);

        void AddNames(string addChars) {
            if (addChars != null) {
                string[] letters = addChars.Split(',');
                TestRecord testName;
                testName.nameOne = letters[0];
                testName.nameTwo = letters[1];
                testName.CosineSimilarityMatchLevel = Convert.ToDouble(letters[6]);
                testNames.Add(testName);
            }
        }

        void LoadData() {
            AddNames(addressSettings.blockDistance1);
            AddNames(addressSettings.blockDistance2);
            AddNames(addressSettings.blockDistance3);
        }
        #endregion

        #region Block Distance Tests
        [Test]
        [Category("CosineSimilarity Test")]
        public void CosineSimilarityShortDescription() {
            Assert.AreEqual("CosineSimilarity", myCosineSimilarity.ShortDescriptionString,
                            "Problem with CosineSimilarity test short description.");
        }

        [Test]
        [Category("CosineSimilarity Test")]
        public void CosineSimilarityTestData() {
            foreach (TestRecord testRecord in testNames) {
                Assert.AreEqual(testRecord.CosineSimilarityMatchLevel.ToString("F3"),
                                myCosineSimilarity.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                string.Format("{0}CosineSimilarity{1}{2}{3}{4}", Environment.NewLine, Environment.NewLine,
                                              testRecord.nameOne, Environment.NewLine, testRecord.nameTwo));
            }
        }
        #endregion

        CosineSimilarity myCosineSimilarity;

        [SetUp]
        public void SetUp() {
            LoadData();
            myCosineSimilarity = new CosineSimilarity();
        }
    }
}
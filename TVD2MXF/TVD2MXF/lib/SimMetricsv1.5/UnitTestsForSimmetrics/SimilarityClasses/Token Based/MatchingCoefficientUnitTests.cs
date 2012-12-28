namespace SimMetrics.UnitTestsForSimMetrics.TokenBasedTests {
    using System;
    using System.Collections.Generic;
    using NUnit.Framework;
    using SimMetricsMetricUtilities;
    using UnitTestsForSimmetrics.Properties;

    [TestFixture]
    sealed public class MatchingCoefficientUnitTests {
        #region Test Data Setup
        struct TestRecord {
            public string nameOne;
            public string nameTwo;
            public double matchingCoefficientMatchLevel;
        }

        Settings addressSettings = Settings.Default;
        List<TestRecord> testNames = new List<TestRecord>(26);

        void AddNames(string addChars) {
            if (addChars != null) {
                string[] letters = addChars.Split(',');
                TestRecord testName;
                testName.nameOne = letters[0];
                testName.nameTwo = letters[1];
                testName.matchingCoefficientMatchLevel = Convert.ToDouble(letters[16]);
                testNames.Add(testName);
            }
        }

        void LoadData() {
            AddNames(addressSettings.blockDistance1);
            AddNames(addressSettings.blockDistance2);
            AddNames(addressSettings.blockDistance3);
        }
        #endregion

        #region MatchingCoefficient Tests
        [Test]
        [Category("MatchingCoefficient Test")]
        public void MatchingCoefficient_ShortDescription() {
            Assert.AreEqual("MatchingCoefficient", myMatchingCoefficient.ShortDescriptionString,
                            "Problem with MatchingCoefficient test short description.");
        }

        [Test]
        [Category("MatchingCoefficient Test")]
        public void MatchingCoefficient_TestData() {
            foreach (TestRecord testRecord in testNames) {
                Assert.AreEqual(testRecord.matchingCoefficientMatchLevel.ToString("F3"),
                                myMatchingCoefficient.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with MatchingCoefficient test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }
        #endregion

        MatchingCoefficient myMatchingCoefficient;

        [SetUp]
        public void SetUp() {
            LoadData();
            myMatchingCoefficient = new MatchingCoefficient();
        }
    }
}
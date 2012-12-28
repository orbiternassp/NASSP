namespace SimMetrics.UnitTestsForSimMetrics.TokenBasedTests {
    using System;
    using System.Collections.Generic;
    using NUnit.Framework;
    using SimMetricsMetricUtilities;
    using UnitTestsForSimmetrics.Properties;

    [TestFixture]
    sealed public class OverlapCoefficientUnitTests {
        #region Test Data Setup
        struct TestRecord {
            public string nameOne;
            public string nameTwo;
            public double overlapCoefficientMatchLevel;
        }

        Settings addressSettings = Settings.Default;
        List<TestRecord> testNames = new List<TestRecord>(26);

        void AddNames(string addChars) {
            if (addChars != null) {
                string[] letters = addChars.Split(',');
                TestRecord testName;
                testName.nameOne = letters[0];
                testName.nameTwo = letters[1];
                testName.overlapCoefficientMatchLevel = Convert.ToDouble(letters[18]);
                testNames.Add(testName);
            }
        }

        void LoadData() {
            AddNames(addressSettings.blockDistance1);
            AddNames(addressSettings.blockDistance2);
            AddNames(addressSettings.blockDistance3);
        }
        #endregion

        #region OverlapCoefficient Tests
        [Test]
        [Category("OverlapCoefficient Test")]
        public void OverlapCoefficient_ShortDescription() {
            Assert.AreEqual("OverlapCoefficient", myOverlapCoefficient.ShortDescriptionString,
                            "Problem with OverlapCoefficient test short description.");
        }

        [Test]
        [Category("OverlapCoefficient Test")]
        public void OverlapCoefficient_TestData() {
            foreach (TestRecord testRecord in testNames) {
                Assert.AreEqual(testRecord.overlapCoefficientMatchLevel.ToString("F3"),
                                myOverlapCoefficient.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with OverlapCoefficient test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }
        #endregion

        OverlapCoefficient myOverlapCoefficient;

        [SetUp]
        public void SetUp() {
            LoadData();
            myOverlapCoefficient = new OverlapCoefficient();
        }
    }
}
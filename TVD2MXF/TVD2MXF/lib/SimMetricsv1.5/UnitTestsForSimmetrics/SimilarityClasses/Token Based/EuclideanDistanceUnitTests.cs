namespace SimMetrics.UnitTestsForSimMetrics.TokenBasedTests {
    using System;
    using System.Collections.Generic;
    using NUnit.Framework;
    using SimMetricsMetricUtilities;
    using UnitTestsForSimmetrics.Properties;

    [TestFixture]
    sealed public class EuclideanDistanceUnitTests {
        #region Test Data Setup
        struct TestRecord {
            public string nameOne;
            public string nameTwo;
            public double euclideanDistanceMatchLevel;
        }

        Settings addressSettings = Settings.Default;
        List<TestRecord> testNames = new List<TestRecord>(26);

        void AddNames(string addChars) {
            if (addChars != null) {
                string[] letters = addChars.Split(',');
                TestRecord testName;
                testName.nameOne = letters[0];
                testName.nameTwo = letters[1];
                testName.euclideanDistanceMatchLevel = Convert.ToDouble(letters[14]);
                testNames.Add(testName);
            }
        }

        void LoadData() {
            AddNames(addressSettings.blockDistance1);
            AddNames(addressSettings.blockDistance2);
            AddNames(addressSettings.blockDistance3);
        }
        #endregion

        #region EuclideanDistance Tests
        [Test]
        [Category("EuclideanDistance Test")]
        public void EuclideanDistance_ShortDescription() {
            Assert.AreEqual("EuclideanDistance", myEuclideanDistance.ShortDescriptionString,
                            "Problem with EuclideanDistance test short description.");
        }

        [Test]
        [Category("EuclideanDistance Test")]
        public void EuclideanDistance_TestData() {
            foreach (TestRecord testRecord in testNames) {
                Assert.AreEqual(testRecord.euclideanDistanceMatchLevel.ToString("F3"),
                                myEuclideanDistance.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with EuclideanDistance test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }
        #endregion

        EuclideanDistance myEuclideanDistance;

        [SetUp]
        public void SetUp() {
            LoadData();
            myEuclideanDistance = new EuclideanDistance();
        }
    }
}
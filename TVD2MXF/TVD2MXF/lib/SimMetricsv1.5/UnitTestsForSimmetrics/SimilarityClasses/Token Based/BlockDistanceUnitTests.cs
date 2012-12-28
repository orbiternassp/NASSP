namespace SimMetrics.UnitTestsForSimMetrics.TokenBasedTests {
    using System;
    using System.Collections.Generic;
    using NUnit.Framework;
    using SimMetricsMetricUtilities;
    using UnitTestsForSimmetrics.Properties;

    [TestFixture]
    sealed public class BlockDistanceUnitTests {
        #region Test Data Setup
        struct TestRecord {
            public string nameOne;
            public string nameTwo;
            public double BlockDistanceMatchLevel;
        }

        Settings addressSettings = Settings.Default;
        List<TestRecord> testNames = new List<TestRecord>(26);

        void AddNames(string addChars) {
            if (addChars != null) {
                string[] letters = addChars.Split(',');
                TestRecord testName;
                testName.nameOne = letters[0];
                testName.nameTwo = letters[1];
                testName.BlockDistanceMatchLevel = Convert.ToDouble(letters[2]);
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
        [Category("BlockDistance Test")]
        public void BlockDistanceShortDescription() {
            Assert.AreEqual("BlockDistance", myBlockDistance.ShortDescriptionString,
                            "Problem with BlockDistance test short description.");
        }

        [Test]
        [Category("BlockDistance Test")]
        public void BlockDistanceTestData() {
            foreach (TestRecord testRecord in testNames) {
                Assert.AreEqual(testRecord.BlockDistanceMatchLevel.ToString("F3"),
                                myBlockDistance.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with BlockDistance test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }
        #endregion

        BlockDistance myBlockDistance;

        [SetUp]
        public void SetUp() {
            LoadData();
            myBlockDistance = new BlockDistance();
        }
    }
}
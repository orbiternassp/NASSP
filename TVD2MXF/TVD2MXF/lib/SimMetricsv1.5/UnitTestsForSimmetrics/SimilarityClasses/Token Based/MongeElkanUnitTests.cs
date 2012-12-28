namespace SimMetrics.UnitTestsForSimMetrics.TokenBasedTests {
    using System;
    using System.Collections.Generic;
    using NUnit.Framework;
    using SimMetricsMetricUtilities;
    using UnitTestsForSimmetrics.Properties;

    [TestFixture]
    sealed public class MongeElkanUnitTests {
        #region Test Data Setup
        struct TestRecord {
            public string nameOne;
            public string nameTwo;
            public double mongeElkanMatchLevel;
        }

        Settings addressSettings = Settings.Default;
        List<TestRecord> testNames = new List<TestRecord>(26);

        void AddNames(string addChars) {
            if (addChars != null) {
                string[] letters = addChars.Split(',');
                TestRecord testName;
                testName.nameOne = letters[0];
                testName.nameTwo = letters[1];
                testName.mongeElkanMatchLevel = Convert.ToDouble(letters[17]);
                testNames.Add(testName);
            }
        }

        void LoadData() {
            AddNames(addressSettings.blockDistance1);
            AddNames(addressSettings.blockDistance2);
            AddNames(addressSettings.blockDistance3);
        }
        #endregion

        #region MongeElkan Tests
        [Test]
        [Category("MongeElkan Test")]
        public void MongeElkan_ShortDescription() {
            Assert.AreEqual("MongeElkan", myMongeElkan.ShortDescriptionString, "Problem with MongeElkan test short description.");
        }

        [Test]
        [Category("MongeElkan Test")]
        public void MongeElkan_TestData() {
            foreach (TestRecord testRecord in testNames) {
                Assert.AreEqual(testRecord.mongeElkanMatchLevel.ToString("F3"),
                                myMongeElkan.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with MongeElkan test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }
        #endregion

        MongeElkan myMongeElkan;

        [SetUp]
        public void SetUp() {
            LoadData();
            myMongeElkan = new MongeElkan();
        }
    }
}
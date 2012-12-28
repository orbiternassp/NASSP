namespace SimMetrics.UnitTestsForSimMetrics.EditDistanceTests {
    using System;
    using System.Collections.Generic;
    using NUnit.Framework;
    using SimMetricsMetricUtilities;
    using UnitTestsForSimmetrics.Properties;

    [TestFixture]
    sealed public class NeedlemanWunchUnitTests {
        #region Test Data Setup
        struct TestRecord {
            public string nameOne;
            public string nameTwo;
            public double needlemanWunchMatchLevel;
        }

        Settings addressSettings = Settings.Default;
        List<TestRecord> testNames = new List<TestRecord>(26);

        void AddNames(string addChars) {
            if (addChars != null) {
                string[] letters = addChars.Split(',');
                TestRecord testName;
                testName.nameOne = letters[0];
                testName.nameTwo = letters[1];
                testName.needlemanWunchMatchLevel = Convert.ToDouble(letters[11]);
                testNames.Add(testName);
            }
        }

        void LoadData() {
            AddNames(addressSettings.blockDistance1);
            AddNames(addressSettings.blockDistance2);
            AddNames(addressSettings.blockDistance3);
            AddNames(addressSettings.jaroName1);
            AddNames(addressSettings.jaroName2);
            AddNames(addressSettings.jaroName3);
            AddNames(addressSettings.jaroName4);
            AddNames(addressSettings.jaroName5);
            AddNames(addressSettings.jaroName6);
            AddNames(addressSettings.jaroName7);
        }
        #endregion

        #region NeedlemanWunch Tests
        [Test]
        [Category("NeedlemanWunch Test")]
        public void NeedlemanWunch_ShortDescription() {
            Assert.AreEqual("NeedlemanWunch", myNeedlemanWunch.ShortDescriptionString,
                            "Problem with NeedlemanWunch test short description.");
        }

        [Test]
        [Category("NeedlemanWunch Test")]
        public void NeedlemanWunch_TestData() {
            foreach (TestRecord testRecord in testNames) {
                Assert.AreEqual(testRecord.needlemanWunchMatchLevel.ToString("F3"),
                                myNeedlemanWunch.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with NeedlemanWunch test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }
        #endregion

        NeedlemanWunch myNeedlemanWunch;

        [SetUp]
        public void SetUp() {
            LoadData();
            myNeedlemanWunch = new NeedlemanWunch();
        }
    }
}
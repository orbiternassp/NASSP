namespace SimMetrics.UnitTestsForSimMetrics.LengthBasedTests {
    using System;
    using System.Collections.Generic;
    using NUnit.Framework;
    using SimMetricsMetricUtilities;
    using UnitTestsForSimmetrics.Properties;

    [TestFixture]
    sealed public class ChapmanMeanLengthUnitTests {
        #region Test Data Setup
        struct TestRecord {
            public string nameOne;
            public string nameTwo;
            public double ChapmanLengthMatchLevel;
        }

        Settings addressSettings = Settings.Default;
        List<TestRecord> testNames = new List<TestRecord>(26);

        void AddNames(string addChars) {
            if (addChars != null) {
                string[] letters = addChars.Split(',');
                TestRecord testName;
                testName.nameOne = letters[0];
                testName.nameTwo = letters[1];
                testName.ChapmanLengthMatchLevel = Convert.ToDouble(letters[5]);
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

        #region Chapman Deviation Tests
        [Test]
        [Category("Chapman Mean Length Test")]
        public void ChapmanMeanLength_ShortDescription() {
            Assert.AreEqual("ChapmanMeanLength", myChapmanMeanLength.ShortDescriptionString,
                            "Problem with Chapman Mean Length test short description.");
        }

        [Test]
        [Category("Chapman Mean Length Test")]
        public void ChapmanMeanLength_TestData() {
            foreach (TestRecord testRecord in testNames) {
                Assert.AreEqual(testRecord.ChapmanLengthMatchLevel.ToString("F3"),
                                myChapmanMeanLength.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with Chapman Mean Length test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }
        #endregion

        ChapmanMeanLength myChapmanMeanLength;

        [SetUp]
        public void SetUp() {
            LoadData();
            myChapmanMeanLength = new ChapmanMeanLength();
        }
    }
}
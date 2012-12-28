namespace SimMetrics.UnitTestsForSimMetrics.EditDistanceTests {
    using System;
    using System.Collections.Generic;
    using NUnit.Framework;
    using SimMetricsMetricUtilities;
    using UnitTestsForSimmetrics.Properties;

    [TestFixture]
    sealed public class LevensteinUnitTests {
        #region Test Data Setup
        struct TestRecord {
            public string nameOne;
            public string nameTwo;
            public double levensteinMatchLevel;
        }

        Settings addressSettings = Settings.Default;
        List<TestRecord> testNames = new List<TestRecord>(26);

        void AddNames(string addChars) {
            if (addChars != null) {
                string[] letters = addChars.Split(',');
                TestRecord testName;
                testName.nameOne = letters[0];
                testName.nameTwo = letters[1];
                testName.levensteinMatchLevel = Convert.ToDouble(letters[10]);
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

        #region Levenstein Tests
        [Test]
        [Category("Levenstein Test")]
        public void Levenstein_ShortDescription() {
            Assert.AreEqual("Levenstein", myLevenstein.ShortDescriptionString, "Problem with Levenstein test short description.");
        }

        [Test]
        [Category("Levenstein Test")]
        public void Levenstein_TestData() {
            foreach (TestRecord testRecord in testNames) {
                Assert.AreEqual(testRecord.levensteinMatchLevel.ToString("F3"),
                                myLevenstein.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with Levenstein test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }
        #endregion

        Levenstein myLevenstein;

        [SetUp]
        public void SetUp() {
            LoadData();
            myLevenstein = new Levenstein();
        }
    }
}
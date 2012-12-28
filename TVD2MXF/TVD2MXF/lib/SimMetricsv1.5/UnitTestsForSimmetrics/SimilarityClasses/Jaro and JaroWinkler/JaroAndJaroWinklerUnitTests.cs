namespace SimMetrics.UnitTestsForSimMetrics.JaroAndJaroWinklerTests {
    using System;
    using System.Collections.Generic;
    using NUnit.Framework;
    using SimMetricsMetricUtilities;
    using UnitTestsForSimmetrics.Properties;

    [TestFixture]
    sealed public class JaroAndJaroWinklerUnitTests {
        #region Test Data Setup
        /// Test Data is taken from
        /// Approximate string Comparison and its Effect on an Advanced Record Linkage System
        /// Porter and Winkler 1997
        /// Chapter 6.
        struct TestRecord {
            public string nameOne;
            public string nameTwo;
            public double jaroMatchLevel;
            public double jaroWinklerMatchLevel;
        }

        Settings addressSettings = Settings.Default;
        List<TestRecord> testNames = new List<TestRecord>(26);

        void AddNames(string addChars) {
            if (addChars != null) {
                string[] letters = addChars.Split(',');
                TestRecord testName;
                testName.nameOne = letters[0];
                testName.nameTwo = letters[1];
                testName.jaroMatchLevel = Convert.ToDouble(letters[2]);
                testName.jaroWinklerMatchLevel = Convert.ToDouble(letters[3]);
                testNames.Add(testName);
            }
        }

        void LoadData() {
            AddNames(addressSettings.jaroName1);
            AddNames(addressSettings.jaroName2);
            AddNames(addressSettings.jaroName3);
            AddNames(addressSettings.jaroName4);
            AddNames(addressSettings.jaroName5);
            AddNames(addressSettings.jaroName6);
            AddNames(addressSettings.jaroName7);
        }
        #endregion

        #region Jaro Tests
        [Test]
        [Category("Jaro Test")]
        public void JaroShortDescription() {
            //myJaro.
            Assert.AreEqual(myJaro.ShortDescriptionString, "Jaro", "Problem with Jaro test ShortDescription");
        }

        [Test]
        [Category("Jaro Test")]
        public void JaroTestData() {
            foreach (TestRecord testRecord in testNames) {
                Assert.AreEqual(testRecord.jaroMatchLevel.ToString("F3"),
                                myJaro.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with Jaro test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }
        #endregion

        #region JaroWinkler tests
        [Test]
        [Category("JaroWinkler Test")]
        public void JaroWinklerShortDescription() {
            Assert.AreEqual(myJaroWinkler.ShortDescriptionString, "JaroWinkler", "Problem with Jaro test ShortDescription");
        }

        [Test]
        [Category("JaroWinkler Test")]
        public void JaroWinklerTestData() {
            foreach (TestRecord testRecord in testNames) {
                Assert.AreEqual(testRecord.jaroWinklerMatchLevel.ToString("F3"),
                                myJaroWinkler.GetSimilarity(testRecord.nameOne, testRecord.nameTwo).ToString("F3"),
                                "Problem with JaroWinkler test - " + testRecord.nameOne + ' ' + testRecord.nameTwo);
            }
        }
        #endregion

        Jaro myJaro;
        JaroWinkler myJaroWinkler;

        [SetUp]
        public void SetUp() {
            LoadData();
            myJaro = new Jaro();
            myJaroWinkler = new JaroWinkler();
        }
    }
}
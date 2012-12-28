namespace SimMetrics.UnitTestsForSimMetrics.QGramTests {
    using System;
    using System.Collections.Generic;
    using NUnit.Framework;
    using SimMetricsMetricUtilities;
    using SimMetricsUtilities;
    using UnitTestsForSimmetrics.Properties;

    [TestFixture]
    sealed public class QGramDistanceUnitTests {
        #region Test Data Setup
        struct TestRecord {
            string nameOne;
            string nameTwo;
            double trigramExtendedDistanceMatchLevel;
            double trigramDistanceMatchLevel;
            double bigramExtendedDistanceMatchLevel;
            double bigramDistanceMatchLevel;

            public TestRecord(string firstName, string secondNamem, double scoreOne, double scoreTwo, double scoreThree,
                              double scoreFour) {
                nameOne = firstName;
                nameTwo = secondNamem;
                trigramExtendedDistanceMatchLevel = scoreOne;
                trigramDistanceMatchLevel = scoreTwo;
                bigramExtendedDistanceMatchLevel = scoreThree;
                bigramDistanceMatchLevel = scoreFour;
            }

            public override string ToString() {
                return
                    NameOne + " : " + NameTwo + " : " + TrigramExtendedDistanceMatchLevel + " : " + TrigramDistanceMatchLevel +
                    " : " + BigramExtendedDistanceMatchLevel + " : " + BigramDistanceMatchLevel;
            }

            public string NameOne { get { return nameOne; } set { nameOne = value; } }
            public string NameTwo { get { return nameTwo; } set { nameTwo = value; } }
            public double TrigramExtendedDistanceMatchLevel { get { return trigramExtendedDistanceMatchLevel; } set { trigramExtendedDistanceMatchLevel = value; } }
            public double TrigramDistanceMatchLevel { get { return trigramDistanceMatchLevel; } set { trigramDistanceMatchLevel = value; } }
            public double BigramExtendedDistanceMatchLevel { get { return bigramExtendedDistanceMatchLevel; } set { bigramExtendedDistanceMatchLevel = value; } }
            public double BigramDistanceMatchLevel { get { return bigramDistanceMatchLevel; } set { bigramDistanceMatchLevel = value; } }
        }

        Settings addressSettings = Settings.Default;
        List<TestRecord> testNames = new List<TestRecord>(26);

        void AddNames(string addChars) {
            if (addChars != null) {
                string[] letters = addChars.Split(',');
                TestRecord testName =
                    new TestRecord(letters[0], letters[1], Convert.ToDouble(letters[12]), Convert.ToDouble(letters[7]),
                                   Convert.ToDouble(letters[8]), Convert.ToDouble(letters[9]));
                testNames.Add(testName);
            }
        }

        void LoadData() {
            AddNames(addressSettings.blockDistance1);
            AddNames(addressSettings.blockDistance2);
            AddNames(addressSettings.blockDistance3);
        }
        #endregion

        #region General Test
        [Test]
        [Category("TrigramExtendedDistance Test")]
        public void QGramDistance_ShortDescription() {
            Assert.AreEqual("QGramsDistance", myTrigramExtendedDistance.ShortDescriptionString,
                            "Problem with QGramDistance test short description.");
        }
        #endregion

        #region TrigramExtendedDistance Tests
        [Test]
        [Category("TrigramExtendedDistance Test")]
        public void TrigramExtendedDistance_TestData() {
            foreach (TestRecord testRecord in testNames) {
                Assert.AreEqual(testRecord.TrigramExtendedDistanceMatchLevel.ToString("F3"),
                                myTrigramExtendedDistance.GetSimilarity(testRecord.NameOne, testRecord.NameTwo).ToString("F3"),
                                string.Format("{0} TrigramExtendedDistance {1} {2}{3}{4}", Environment.NewLine,
                                              Environment.NewLine, testRecord.NameOne, Environment.NewLine, testRecord.NameTwo));
            }
        }
        #endregion

        #region TrigramDistance Tests
        [Test]
        [Category("TrigramDistance Test")]
        public void TrigramDistance_TestData() {
            foreach (TestRecord testRecord in testNames) {
                Assert.AreEqual(testRecord.TrigramDistanceMatchLevel.ToString("F3"),
                                myTrigramDistance.GetSimilarity(testRecord.NameOne, testRecord.NameTwo).ToString("F3"),
                                string.Format("{0} TrigramDistance {1} {2}{3}{4}", Environment.NewLine, Environment.NewLine,
                                              testRecord.NameOne, Environment.NewLine, testRecord.NameTwo));
            }
        }
        #endregion

        #region BigramExtendedDistance Tests
        [Test]
        [Category("BigramExtendedDistance Test")]
        public void BigramExtendedDistance_TestData() {
            foreach (TestRecord testRecord in testNames) {
                Assert.AreEqual(testRecord.BigramExtendedDistanceMatchLevel.ToString("F3"),
                                myBigramExtendedDistance.GetSimilarity(testRecord.NameOne, testRecord.NameTwo).ToString("F3"),
                                string.Format("{0} BigramExtendedDistance {1} {2}{3}{4}", Environment.NewLine,
                                              Environment.NewLine, testRecord.NameOne, Environment.NewLine, testRecord.NameTwo));
            }
        }
        #endregion

        #region BigramDistance Tests
        [Test]
        [Category("BigramDistance Test")]
        public void BigramDistance_TestData() {
            foreach (TestRecord testRecord in testNames) {
                Assert.AreEqual(testRecord.BigramDistanceMatchLevel.ToString("F3"),
                                myBigramDistance.GetSimilarity(testRecord.NameOne, testRecord.NameTwo).ToString("F3"),
                                string.Format("{0} BigramDistance {1} {2}{3}{4}", Environment.NewLine, Environment.NewLine,
                                              testRecord.NameOne, Environment.NewLine, testRecord.NameTwo));
            }
        }
        #endregion

        QGramsDistance myTrigramExtendedDistance;
        QGramsDistance myTrigramDistance;
        QGramsDistance myBigramExtendedDistance;
        QGramsDistance myBigramDistance;

        [SetUp]
        public void SetUp() {
            LoadData();
            // default is TokeniserQGram3Extended
            myTrigramExtendedDistance = new QGramsDistance();
            myTrigramDistance = new QGramsDistance(new TokeniserQGram3());
            myBigramExtendedDistance = new QGramsDistance(new TokeniserQGram2Extended());
            myBigramDistance = new QGramsDistance(new TokeniserQGram2());
        }
    }
}
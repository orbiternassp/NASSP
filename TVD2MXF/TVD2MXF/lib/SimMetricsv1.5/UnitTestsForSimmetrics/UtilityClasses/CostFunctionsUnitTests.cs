namespace SimMetrics.UnitTestsForSimMetrics.UtilityTests {
    using NUnit.Framework;
    using SimMetricsUtilities;

    [TestFixture]
    sealed public class CostFunctionsUnitTests {
        #region AffineGapRange1To0Multiplier1Over3 Tests
        [Test]
        [Category("AffineGapRange1To0Multiplier1Over3 Test")]
        public void AffineGapRange1To0Multiplier1Over3ShortDescription() {
            Assert.AreEqual("AffineGapRange1To0Multiplier1Over3", myCostFunction1.ShortDescriptionString,
                            "Problem with AffineGapRange1To0Multiplier1Over3 test short description.");
        }

        [Test]
        [Category("AffineGapRange1To0Multiplier1Over3 Test")]
        public void AffineGapRange1To0Multiplier1Over3PassTest() {
            double result = myCostFunction1.GetCost("CHRIS", 1, 3);
            Assert.AreEqual("1.333", result.ToString("F3"), "Problem with AffineGapRange1To0Multiplier1Over3 pass test.");
        }

        [Test]
        [Category("AffineGapRange1To0Multiplier1Over3 Test")]
        public void AffineGapRange1To0Multiplier1Over3FailTest() {
            double result = myCostFunction1.GetCost("CHRIS", 4, 3);
            Assert.AreEqual("0.000", result.ToString("F3"), "Problem with AffineGapRange1To0Multiplier1Over3 fail test.");
        }
        #endregion

        #region AffineGapRange5To0Multiplier1 Tests
        [Test]
        [Category("AffineGapRange5To0Multiplier1 Test")]
        public void AffineGapRange5To0Multiplier1ShortDescription() {
            Assert.AreEqual("AffineGapRange5To0Multiplier1", myCostFunction2.ShortDescriptionString,
                            "Problem with AffineGapRange5To0Multiplier1 test short description.");
        }

        [Test]
        [Category("AffineGapRange5To0Multiplier1 Test")]
        public void AffineGapRange5To0Multiplier1PassTest() {
            double result = myCostFunction2.GetCost("CHRIS", 1, 3);
            Assert.AreEqual("6.000", result.ToString("F3"), "Problem with AffineGapRange5To0Multiplier1 pass test.");
        }

        [Test]
        [Category("AffineGapRange5To0Multiplier1 Test")]
        public void AffineGapRange5To0Multiplier1FailTest() {
            double result = myCostFunction2.GetCost("CHRIS", 4, 3);
            Assert.AreEqual("0.000", result.ToString("F3"), "Problem with AffineGapRange5To0Multiplier1 fail test.");
        }
        #endregion

        #region SubCostRange0To1 Tests
        [Test]
        [Category("SubCostRange0To1 Test")]
        public void SubCostRange0To1ShortDescription() {
            Assert.AreEqual("SubCostRange0To1", myCostFunction3.ShortDescriptionString,
                            "Problem with SubCostRange0To1 test short description.");
        }

        [Test]
        [Category("SubCostRange0To1 Test")]
        public void SubCostRange0To1PassTest() {
            double result = myCostFunction3.GetCost("CHRIS", 1, "KRIS", 3);
            Assert.AreEqual("1.000", result.ToString("F3"), "Problem with SubCostRange0To1 pass test.");
        }

        [Test]
        [Category("SubCostRange0To1 Test")]
        public void SubCostRange0To1FailTest() {
            double result = myCostFunction3.GetCost("CHRIS", 4, "KRIS", 3);
            Assert.AreEqual("0.000", result.ToString("F3"), "Problem with SubCostRange0To1 fail test.");
        }
        #endregion

        #region SubCostRange1ToMinus2 Tests
        [Test]
        [Category("SubCostRange1ToMinus2 Test")]
        public void SubCostRange1ToMinus2ShortDescription() {
            Assert.AreEqual("SubCostRange1ToMinus2", myCostFunction4.ShortDescriptionString,
                            "Problem with SubCostRange1ToMinus2 test short description.");
        }

        [Test]
        [Category("SubCostRange1ToMinus2 Test")]
        public void SubCostRange1ToMinus2PassTest() {
            double result = myCostFunction4.GetCost("CHRIS", 1, "CHRIS", 1);
            Assert.AreEqual("1.000", result.ToString("F3"), "Problem with SubCostRange1ToMinus2 pass test.");
        }

        [Test]
        [Category("SubCostRange1ToMinus2 Test")]
        public void SubCostRange1ToMinus2FailTest() {
            // fail due to first word index greater than word length
            Assert.AreEqual("-2.000", myCostFunction4.GetCost("CHRIS", 6, "CHRIS", 3).ToString("F3"),
                            "Problem with SubCostRange1ToMinus2 fail test.");
            // fail due to second word index greater than word length
            Assert.AreEqual("-2.000", myCostFunction4.GetCost("CHRIS", 3, "CHRIS", 6).ToString("F3"),
                            "Problem with SubCostRange1ToMinus2 fail test.");
            // fail to different chars
            Assert.AreEqual("-2.000", myCostFunction4.GetCost("CHRIS", 1, "KRIS", 1).ToString("F3"),
                            "Problem with SubCostRange1ToMinus2 fail test.");
        }
        #endregion

        #region SubCostRange5ToMinus3 Tests
        [Test]
        [Category("SubCostRange5ToMinus3 Test")]
        public void SubCostRange5ToMinus3ShortDescription() {
            Assert.AreEqual("SubCostRange5ToMinus3", myCostFunction5.ShortDescriptionString,
                            "Problem with SubCostRange5ToMinus3 test short description.");
        }

        [Test]
        [Category("SubCostRange5ToMinus3 Test")]
        public void SubCostRange5ToMinus3PassTest() {
            double result = myCostFunction5.GetCost("CHRIS", 1, "CHRIS", 1);
            Assert.AreEqual("5.000", result.ToString("F3"), "Problem with SubCostRange5ToMinus3 pass test.");
        }

        [Test]
        [Category("SubCostRange5ToMinus3 Test")]
        public void SubCostRange5ToMinus3FailTest() {
            // fail due to first word index greater than word length
            Assert.AreEqual("-3.000", myCostFunction5.GetCost("CHRIS", 6, "CHRIS", 3).ToString("F3"),
                            "Problem with SubCostRange5ToMinus3 fail test.");
            // fail due to second word index greater than word length
            Assert.AreEqual("-3.000", myCostFunction5.GetCost("CHRIS", 3, "CHRIS", 6).ToString("F3"),
                            "Problem with SubCostRange5ToMinus3 fail test.");
            // fail to different chars
            Assert.AreEqual("-3.000", myCostFunction5.GetCost("CHRIS", 1, "KRIS", 1).ToString("F3"),
                            "Problem with SubCostRange5ToMinus3 fail test.");
        }

        [Test]
        [Category("SubCostRange5ToMinus3 Test")]
        public void SubCostRange5ToMinus3ApproxTest() {
            Assert.AreEqual("3.000", myCostFunction5.GetCost("GILL", 0, "JILL", 0).ToString("F3"),
                            "Problem with SubCostRange5ToMinus3 fail test.");
        }
        #endregion

        AffineGapRange1To0Multiplier1Over3 myCostFunction1;
        AffineGapRange5To0Multiplier1 myCostFunction2;
        SubCostRange0To1 myCostFunction3;
        SubCostRange1ToMinus2 myCostFunction4;
        SubCostRange5ToMinus3 myCostFunction5;

        [SetUp]
        public void SetUp() {
            myCostFunction1 = new AffineGapRange1To0Multiplier1Over3();
            myCostFunction2 = new AffineGapRange5To0Multiplier1();
            myCostFunction3 = new SubCostRange0To1();
            myCostFunction4 = new SubCostRange1ToMinus2();
            myCostFunction5 = new SubCostRange5ToMinus3();
        }
    }
}
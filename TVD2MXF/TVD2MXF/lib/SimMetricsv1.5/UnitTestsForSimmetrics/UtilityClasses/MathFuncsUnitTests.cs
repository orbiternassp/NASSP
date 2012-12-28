namespace SimMetrics.UnitTestsForSimMetrics.UtilityTests {
    using NUnit.Framework;
    using SimMetricsUtilities;

    [TestFixture]
    sealed public class MathFuncsUnitTests {
        #region MathFuncs Tests
        [Test]
        [Category("MathFuncs Test")]
        public void MathFuncsMinOf3() {
            Assert.AreEqual(1, MathFunctions.MinOf3(1, 2, 3), "Problem with MathFuncs MinOf3 int.");
            Assert.AreEqual(1.0, MathFunctions.MinOf3(1.0, 2.0, 3.0), "Problem with MathFuncs MinOf3 double.");
        }

        [Test]
        [Category("MathFuncs Test")]
        public void MathFuncsMaxOf3() {
            Assert.AreEqual(3, MathFunctions.MaxOf3(1, 2, 3), "Problem with MathFuncs MaxOf3 int.");
            Assert.AreEqual(3.0, MathFunctions.MaxOf3(1.0, 2.0, 3.0), "Problem with MathFuncs MaxOf3 double.");
        }

        [Test]
        [Category("MathFuncs Test")]
        public void MathFuncsMaxOf4() {
            Assert.AreEqual(4, MathFunctions.MaxOf4(1, 2, 3, 4), "Problem with MathFuncs MaxOf4 int.");
            Assert.AreEqual(4.0, MathFunctions.MaxOf4(1.0, 2.0, 3.0, 4.0), "Problem with MathFuncs MaxOf4 double.");
        }
        #endregion

        [SetUp]
        public void SetUp() {}
    }
}
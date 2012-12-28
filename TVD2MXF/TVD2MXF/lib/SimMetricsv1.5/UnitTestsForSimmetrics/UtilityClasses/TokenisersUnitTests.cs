namespace SimMetrics.UnitTestsForSimMetrics.UtilityTests {
    using System;
    using System.Collections.ObjectModel;
    using NUnit.Framework;
    using SimMetricsUtilities;

    [TestFixture]
    sealed public class TokenisersUnitTests {
        #region TokeniserQGram3 Tests
        [Test]
        [Category("TokeniserQGram3 Test")]
        public void TokeniserQGram3ShortDescription() {
            Assert.AreEqual("TokeniserQGram3", myTokeniserQGram3.ShortDescriptionString,
                            "Problem with TokeniserQGram3 test short description.");
        }

        [Test]
        [Category("TokeniserQGram3Extended Test")]
        public void TokeniserQGram3ExtendedShortDescription() {
            Assert.AreEqual("TokeniserQGram3Extended", myTokeniserQGram3Extended.ShortDescriptionString,
                            "Problem with TokeniserQGram3Extended test short description.");
        }

        [Test]
        [Category("TokeniserQGram3 Test")]
        public void TokeniserQGram3TestData() {
            Collection<string> myKnownResult = new Collection<string>();
            myKnownResult.Add("CHR");
            myKnownResult.Add("HRI");
            myKnownResult.Add("RIS");
            //myKnownResult.TrimExcess();
            Collection<string> myResult = myTokeniserQGram3.Tokenize("CHRIS");
            for (int i = 0; i < myKnownResult.Count; i++) {
                Assert.AreEqual(myKnownResult[i], myResult[i], "Problem with TokeniserQGram3 test.");
            }
        }

        [Test]
        [Category("TokeniserQGram3 Test")]
        public void TokeniserQGram3ExtendedTestData() {
            Collection<string> myKnownResult = new Collection<string>();
            myKnownResult.Add("??C");
            myKnownResult.Add("?CH");
            myKnownResult.Add("CHR");
            myKnownResult.Add("HRI");
            myKnownResult.Add("RIS");
            myKnownResult.Add("IS#");
            myKnownResult.Add("S##");
            Collection<string> myResult = myTokeniserQGram3Extended.Tokenize("CHRIS");
            for (int i = 0; i < myKnownResult.Count; i++) {
                Assert.AreEqual(myKnownResult[i], myResult[i], "Problem with TokeniserQGram3Extended test.");
            }
        }
        #endregion

        #region TokeniserQGram2 Tests
        [Test]
        [Category("TokeniserQGram2 Test")]
        public void TokeniserQGram2ShortDescription() {
            Assert.AreEqual("TokeniserQGram2", myTokeniserQGram2.ShortDescriptionString,
                            "Problem with TokeniserQGram2 test short description.");
        }

        [Test]
        [Category("TokeniserQGram2Extended Test")]
        public void TokeniserQGram2ExtendedShortDescription() {
            Assert.AreEqual("TokeniserQGram2Extended", myTokeniserQGram2Extended.ShortDescriptionString,
                            "Problem with TokeniserQGram2Extended test short description.");
        }

        [Test]
        [Category("TokeniserQGram2 Test")]
        public void TokeniserQGram2TestData() {
            Collection<string> myKnownResult = new Collection<string>();
            myKnownResult.Add("CH");
            myKnownResult.Add("HR");
            myKnownResult.Add("RI");
            myKnownResult.Add("IS");
            //myKnownResult.TrimExcess();
            Collection<string> myResult = myTokeniserQGram2.Tokenize("CHRIS");
            for (int i = 0; i < myKnownResult.Count; i++) {
                Assert.AreEqual(myKnownResult[i], myResult[i], "Problem with TokeniserQGram2 test.");
            }
        }

        [Test]
        [Category("TokeniserQGram2 CCI Test")]
        public void TokeniserQGram2TestWithCci1_Data() {
            Collection<string> myKnownResult = new Collection<string>();
            myKnownResult.Add("CH");
            myKnownResult.Add("HR");
            myKnownResult.Add("RI");
            myKnownResult.Add("IS");
            myKnownResult.Add("CR");
            myKnownResult.Add("HI");
            myKnownResult.Add("RS");
            //myKnownResult.TrimExcess();
            myTokeniserQGram2.CharacterCombinationIndex = 1;
            Collection<string> myResult = myTokeniserQGram2.Tokenize("CHRIS");
            for (int i = 0; i < myKnownResult.Count; i++) {
                Assert.AreEqual(myKnownResult[i], myResult[i], "Problem with TokeniserQGram2 test.");
            }
        }

        [Test]
        [Category("TokeniserQGram2 Test")]
        public void TokeniserQGram2ExtendedTestData() {
            Collection<string> myKnownResult = new Collection<string>();
            myKnownResult.Add("?C");
            myKnownResult.Add("CH");
            myKnownResult.Add("HR");
            myKnownResult.Add("RI");
            myKnownResult.Add("IS");
            myKnownResult.Add("S#");
            Collection<string> myResult = myTokeniserQGram2Extended.Tokenize("CHRIS");
            for (int i = 0; i < myKnownResult.Count; i++) {
                Assert.AreEqual(myKnownResult[i], myResult[i], "Problem with TokeniserQGram2Extended test.");
            }
        }

        [Test]
        [Category("TokeniserQGram2 CCI Test")]
        public void TokeniserQGram2ExtendedTestCc1_Data() {
            Collection<string> myKnownResult = new Collection<string>();
            myKnownResult.Add("?C");
            myKnownResult.Add("CH");
            myKnownResult.Add("HR");
            myKnownResult.Add("RI");
            myKnownResult.Add("IS");
            myKnownResult.Add("S#");
            myKnownResult.Add("?H");
            myKnownResult.Add("CR");
            myKnownResult.Add("HI");
            myKnownResult.Add("RS");
            myKnownResult.Add("I#");
            myTokeniserQGram2Extended.CharacterCombinationIndex = 1;
            Collection<string> myResult = myTokeniserQGram2Extended.Tokenize("CHRIS");
            for (int i = 0; i < myKnownResult.Count; i++) {
                Assert.AreEqual(myKnownResult[i], myResult[i], "Problem with TokeniserQGram2Extended test.");
            }
        }
        #endregion

        #region SGram tests
        [Test]
        public void TokeniserSGram2ExtendedTesting_ToString() {
            myTokeniserSGram2Extended.Tokenize("CHRIS");
            string result = myTokeniserSGram2Extended.ShortDescriptionString + " - currently holding : CHRIS." +
                            Environment.NewLine + "The method is using a character combination index of " +
                            Convert.ToInt32(myTokeniserSGram2Extended.CharacterCombinationIndex) + " and " + Environment.NewLine +
                            "a QGram length of " + Convert.ToInt32(myTokeniserSGram2Extended.QGramLength) + ".";
            Assert.AreEqual(result, myTokeniserSGram2Extended.ToString(), "ToString method");
        }

        [Test]
        public void TokeniserSGram2ExtendedTestCc1_Data() {
            Collection<string> myKnownResult = new Collection<string>();
            myKnownResult.Add("?C");
            myKnownResult.Add("CH");
            myKnownResult.Add("HR");
            myKnownResult.Add("RI");
            myKnownResult.Add("IS");
            myKnownResult.Add("S#");
            myKnownResult.Add("?H");
            myKnownResult.Add("CR");
            myKnownResult.Add("HI");
            myKnownResult.Add("RS");
            myKnownResult.Add("I#");
            Collection<string> myResult = myTokeniserSGram2Extended.Tokenize("CHRIS");
            for (int i = 0; i < myKnownResult.Count; i++) {
                Assert.AreEqual(myKnownResult[i], myResult[i], "Problem with TokeniserQGram2Extended test.");
            }
        }

        [Test]
        public void TokeniserSGram2TestWithCci1_Data() {
            Collection<string> myKnownResult = new Collection<string>();
            myKnownResult.Add("CH");
            myKnownResult.Add("HR");
            myKnownResult.Add("RI");
            myKnownResult.Add("IS");
            myKnownResult.Add("CR");
            myKnownResult.Add("HI");
            myKnownResult.Add("RS");
            Collection<string> myResult = myTokeniserSGram2.Tokenize("CHRIS");
            for (int i = 0; i < myKnownResult.Count; i++) {
                Assert.AreEqual(myKnownResult[i], myResult[i], "Problem with TokeniserQGram2 test.");
            }
        }
        #endregion

        #region White Space Tests
        [Test]
        [Category("TokeniserWhitespace Test")]
        public void TokeniserWhitespaceShortDescription() {
            Assert.AreEqual("TokeniserWhitespace", myTokeniserWhitespace.ShortDescriptionString,
                            "Problem with TokeniserWhitespace test short description.");
        }

        [Test]
        [Category("TokeniserWhitespace Test")]
        public void TokeniserWhitespaceTestData() {
            Collection<string> myKnownResult = new Collection<string>();
            myKnownResult.Add("CHRIS");
            myKnownResult.Add("IS");
            myKnownResult.Add("HERE");
            Collection<string> myResult = myTokeniserWhitespace.Tokenize("CHRIS IS HERE");
            for (int i = 0; i < myKnownResult.Count; i++) {
                Assert.AreEqual(myKnownResult[i], myResult[i], "Problem with TokeniserWhitespace test.");
            }
        }

        [Test]
        [Category("TokeniserWhitespace Test")]
        public void TokeniserWhitespaceDelimiterTest() {
            Collection<string> myKnownResult = new Collection<string>();
            myKnownResult.Add("CHRIS");
            myKnownResult.Add("IS");
            myKnownResult.Add("");
            myKnownResult.Add("HERE");
            myKnownResult.Add("woo");
            Collection<string> myResult = myTokeniserWhitespace.Tokenize("CHRIS\nIS\r HERE\twoo");
            for (int i = 0; i < myKnownResult.Count; i++) {
                Assert.AreEqual(myKnownResult[i], myResult[i], "Problem with TokeniserWhitespace test.");
            }
        }
        #endregion

        TokeniserQGram3 myTokeniserQGram3;
        TokeniserQGram3Extended myTokeniserQGram3Extended;
        TokeniserQGram2 myTokeniserQGram2;
        TokeniserSGram2 myTokeniserSGram2;
        TokeniserQGram2Extended myTokeniserQGram2Extended;
        TokeniserSGram2Extended myTokeniserSGram2Extended;
        TokeniserWhitespace myTokeniserWhitespace;

        [SetUp]
        public void SetUp() {
            myTokeniserQGram3 = new TokeniserQGram3();
            myTokeniserQGram3Extended = new TokeniserQGram3Extended();
            myTokeniserQGram2 = new TokeniserQGram2();
            myTokeniserSGram2 = new TokeniserSGram2();
            myTokeniserQGram2Extended = new TokeniserQGram2Extended();
            myTokeniserSGram2Extended = new TokeniserSGram2Extended();
            myTokeniserWhitespace = new TokeniserWhitespace();
        }
    }
}
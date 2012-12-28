This is an updated version of the original .NET implementation and not a conversion of the newest Java Code.

I have principally changed method names, parameter names, converted methods to properties etc.

The .DOX file is a Doc-O-Matic (www.doc-o-matic.com) project file for v 3.10 as that is the newest I have.
THe .FxCop file is from the Microsoft FxCop checking tool.
THe .mmcs file is the ModelMaker 8.20 (www.modelmakertools.com) project file. Only used for creating the diagrams.
The .resharper.user file is a ReSharper (www.jetbrains.com) configuation file.

The files in the unused folder are those which were included with the original Java package but are not required to compile
the SimMetrics library.

TimeAndMetricTests
These are the classes used in the Java library to test the matrics and also the completion times for each metric.

Utility Code
ChapmanMatchingSoundex - this need to have the Phonetics Soundex algorithm included withint the SimMetrics library to run.
I still need to refactor that library before the classes can be included.

GenericGazateerTermHandler
GenericStopTermHandler
Both of these functions may be used instead of the DummyStopTermHandler under the WordHandlers folder.

IMetricArbitrator
Interface used in the TimeandMEtric tests

MeanMetricArbitrator
Arbitrator used in the TimeAndMetricTests

OrderedHash
SortableTermObject




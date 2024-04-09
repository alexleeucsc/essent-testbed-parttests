package essent

import java.io.File

import scopt.OptionParser


case class OptFlags(
    firInputFile: File = null,
    removeFlatConnects: Boolean = true,
    regUpdates: Boolean = true,
    conditionalMuxes: Boolean = true,
    useCondParts: Boolean = true,
    writeHarness: Boolean = false,
    dumpLoFirrtl: Boolean = false,
    trackSigs: Boolean = false,
    trackParts: Boolean = false,
    trackExts: Boolean = false,
    partStats: Boolean = false,
    partCutoff: Int = 8,
    withVCD: Boolean = false,
    withFST: Boolean = false,
    essentLogLevel: String = "warn",
    firrtlLogLevel: String = "warn",
    graphDumpFile: String = null,
    useExtPart: Boolean = false,
    dumpPreMergeInfo: Boolean = false,
    internalMergeInfo: Boolean = false,
    timeSpentCycleChecking: Boolean = false,
    origCpp: Boolean = false,
    readInGraphDirect: Boolean = false,
    readInGraphDirectPath: String = "",
    printOutClustSizes: Boolean = false,
    printOutClustCutCount: Boolean = false,
    trackSigWithinMux: Boolean = false) {
  def inputFileDir() = firInputFile.getParent
  def outputDir() = if (inputFileDir == null) "" else inputFileDir()
  def getGraphDumpFile() = if (graphDumpFile == null) "" else graphDumpFile
}

class ArgsParser {
  val parser = new OptionParser[OptFlags]("essent") {
    arg[File]("<file>").required().unbounded().action( (x, c) =>
      c.copy(firInputFile = x) ).text(".fir input file")

    opt[Unit]("O0").abbr("O0").action( (_, c) => c.copy(
        removeFlatConnects = false,
        regUpdates = false,
        conditionalMuxes = false,
        useCondParts=false)
    ).text("disable all optimizations")

    opt[Unit]("O1").abbr("O1").action( (_, c) => c.copy(
        removeFlatConnects = true,
        regUpdates = true,
        conditionalMuxes = false,
        useCondParts=false)
    ).text("enable only optimizations without conditionals")

    opt[Unit]("O2").abbr("O2").action( (_, c) => c.copy(
        removeFlatConnects = true,
        regUpdates = true,
        conditionalMuxes = true,
        useCondParts=false)
    ).text("enable conditional evaluation of mux inputs")

    opt[Unit]("O3").abbr("O3").action( (_, c) => c.copy(
        removeFlatConnects = true,
        regUpdates = true,
        conditionalMuxes = true,
        useCondParts=true)
    ).text("enable all optimizations (default)")

    opt[Unit]("dump").action( (_, c) => c.copy(
        dumpLoFirrtl = true)
    ).text("dump low-firrtl prior to essent executing")

    opt[Unit]('h', "harness").action( (_, c) => c.copy(
        writeHarness = true)
    ).text("generate harness for Verilator debug API")

    opt[String]("essent-log-level").abbr("ell").valueName("<Error|Warn|Info|Debug|Trace>")
    .validate { x =>
      if (Array("error", "warn", "info", "debug", "trace").contains(x.toLowerCase)) success
      else failure(s"$x bad value must be one of error|warn|info|debug|trace")
    }
    .action( (level, c) => c.copy(essentLogLevel = level ) )
    .text("logging level for essent processing after firrtl")

    opt[String]("firrtl-log-level").abbr("fll").valueName("<Error|Warn|Info|Debug|Trace>")
    .validate { x =>
      if (Array("error", "warn", "info", "debug", "trace").contains(x.toLowerCase)) success
      else failure(s"$x bad value must be one of error|warn|info|debug|trace")
    }
    .action( (level, c) => c.copy(firrtlLogLevel = level ) )
    .text("logging level for firrtl preprocessing")

    help("help").text("prints this usage text")

    opt[Unit]("activity-signal").action( (_, c) => c.copy(
        trackSigs = true)
    ).text("track individual signal activities")

    opt[Unit]("activity-parts").action( (_, c) => c.copy(
        useCondParts = true,
        trackParts = true)
    ).text("print out partition activity stats")

    opt[Unit]("activity-exts").action( (_, c) => c.copy(
        trackSigs = true,
        trackExts = true)
    ).text("track individual signal extinguishes (with activities)")

    opt[Unit]("stats-parts").action( (_, c) => c.copy(
        useCondParts = true,
        partStats = true)
    ).text("output topo information from partitioning")

    opt[Int]("part-cutoff").action( (x, c) => c.copy(
        partCutoff = x)
    ).text("parameter used for partitioning")

    opt[Unit]("withVCD").abbr("withVCD").action( (_, c) => c.copy(
        removeFlatConnects = false,
        withVCD = true)
    ).text("parameter used for vcd generation")

    opt[Unit]("withFST").abbr("withFST").action( (_, c) => c.copy(
        removeFlatConnects = false,
        withVCD = true,
        withFST = true)
    ).text("parameter used for vcd generation")

    opt[String]("graphDump").required().unbounded().action( (x, c) =>
      c.copy(graphDumpFile = x) ).text("file to dump graph info to")

    opt[Unit]("externalPartitioner").action( (_, c) => c.copy(
        useExtPart = true,
    )).text("use external partitioner")
    opt[Unit]("preMergeInfo").action( (_, c) => c.copy(
        dumpPreMergeInfo = true,
    )).text("dumps all graphDump info into a new file called graphDump+str(_preMerge), which will contain validNode, idToStmt, and inNeigh/outNeigh info from BEFORE any merging happens (useful for testing new partitioners)")
    opt[Unit]("internalMergeInfo").action( (_, c) => c.copy(
        internalMergeInfo = true,
    )).text("dumps all graphDump info into a new file called graphDump+str(_internalMergeInfo), which will contain idToMergeID, mergeIDToMembers, inNeigh/outNeigh info from after internal merging happens (useful for evlauting internal partitioner)")  
    opt[Unit]("timeSpentCycleChecking").action( (_, c) => c.copy(
        timeSpentCycleChecking = true,
    )).text("gathers time spent checking for cycles, prints out one value at the end of partitioning")    
    opt[Unit]("origCpp").action( (_, c) => c.copy(
        origCpp = true,
    )).text("run with external partitioner, but using the cpp algo with no algorithmic changes")    

    opt[Unit]("readInGraphDirect").action( (_, c) => c.copy(
        readInGraphDirect = true,
    )).text("if true, read in readInGraphDirectPath graph obj directly")    
    opt[String]("readInGraphDirectPath").unbounded().action( (x, c) =>
      c.copy(readInGraphDirectPath = x) ).text("readInGraphDirectPath")

    opt[Unit]("printOutClustSizes").action( (_, c) => c.copy(
        printOutClustSizes = true,
    )).text("print out a vec of all clust sizes; makes getting effective activaiton easier")    
    opt[Unit]("printOutClustCutCount").action( (_, c) => c.copy(
        printOutClustCutCount = true,
    )).text("print out a vec of all PARTFLAG counts for each cluster; makes getting PARTFLAG stats possible")    
    opt[Unit]("trackSigWithinMux").action( (_, c) => c.copy(
        trackSigWithinMux = true,
    )).text("track a num that is simply the real number of signals evaluated when a mux is evalutaed ")    

  }

  def getConfig(args: Seq[String]): Option[OptFlags] = parser.parse(args, OptFlags())
}

object TestFlags {
  def apply(inputFirFile: File): OptFlags = {
    OptFlags(firInputFile = inputFirFile, writeHarness = true)
  }
}

package essent

import essent.Graph.NodeID

import logger._

import collection.mutable.{ArrayBuffer, HashSet}

import scala.io.Source

import MFFC.{Unclaimed,Excluded}
import java.io._

class AcyclicPart(val mg: MergeGraph, excludeSet: Set[NodeID]) extends LazyLogging {

  //042523 - stores mergeIDs of big clusters we don't want to keep merging
  var clustersToKeep = Set[NodeID] ()

  def partsRemaining() = (mg.mergeIDToMembers.keys.toSet - excludeSet).size

  def findSmallParts(smallPartCutoff: Int) = mg.mergeIDToMembers.keys.toSeq filter {
    id => (mg.nodeSize(id) < smallPartCutoff) && (!excludeSet.contains(id)) && (!clustersToKeep.contains(id))
  }

  def perfomMergesIfPossible(mergesToConsider: Seq[Seq[NodeID]]) = {
    val mergesMade = mergesToConsider flatMap { mergeReq => {
      assert(mergeReq.size > 1)
      val partsStillExist = mergeReq.forall{ mg.mergeIDToMembers.contains(_) }
      if (partsStillExist && mg.mergeIsAcyclic(mergeReq.toSet)) {
        assert(mergeReq forall { id => !excludeSet.contains(id) })
        mg.mergeGroups(mergeReq.head, mergeReq.tail)
        Seq(mergeReq)
      } else Seq()
    }}
    mergesMade
  }

  def numEdgesRemovedByMerge(mergeReq: Seq[NodeID]): Int = {
    val totalInDegree = (mergeReq map { mg.inNeigh(_).size }).sum
    val totalOutDegree = (mergeReq map { mg.outNeigh(_).size }).sum
    val mergedInDegree = ((mergeReq flatMap mg.inNeigh).distinct diff mergeReq).size
    val mergedOutDegree = ((mergeReq flatMap mg.outNeigh).distinct diff mergeReq).size
    totalInDegree + totalOutDegree - (mergedInDegree + mergedOutDegree)
  }

  def coarsenWithMFFCs() {
    val mffcResults = MFFC(mg, excludeSet)
    mg.applyInitialAssignments(mffcResults)
    logger.info(s"  #mffcs found: ${mg.mergeIDToMembers.size - excludeSet.size}")
    logger.info(s"  largest mffc: ${(mg.mergeIDToMembers.values.map{_.size}).max}")
  }

  def coarsenWithMFFCs_notInitial() {
    print("@size of clustersToKeep: "+clustersToKeep.size.toString)
    //val mffcResults = MFFC(mg, excludeSet++clustersToKeep)
    val mffcResults = MFFC(mg, excludeSet)
    val asMap = Util.groupIndicesByValue(mffcResults)
    // print("@size of problem: "+asMap.size.toString)
    // var asMapIdx = 0
    // asMap foreach { case (mergeID, members) => {
    //   print("member size: "+members.size.toString+"\n")
    //   for (i <- 1 until members.size){
    //     mergeSmallParts(members(0), members(i))
    //   }
    //   asMapIdx = asMapIdx + 1
    //   if (asMapIdx%100==0){
    //     print("asMapIdx: "+asMapIdx.toString())
    //   }
    // }}
    for((mergeID, members) <- asMap){
      assert(members.contains(mergeID))
    }
    val mergesMade = perfomMergesIfPossible(asMap.values.toSeq.filter({membs => membs.size>1}))
    logger.info(s"  #mffcs found: ${mg.mergeIDToMembers.size - excludeSet.size}")
    logger.info(s"  largest mffc: ${(mg.mergeIDToMembers.values.map{_.size}).max}")
    printf(s"  #mffcs found: ${mg.mergeIDToMembers.size - excludeSet.size}")
    printf(s"  largest mffc: ${(mg.mergeIDToMembers.values.map{_.size}).max}")
  }

  def coarsenWithInputFile_addExclude() {
    val graphFile = "/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/pythonExperiments/experiments/04_05_23_things/042523_bigclusters"
    val IDXtoMergeID = ArrayBuffer.fill(mg.numNodes)(Unclaimed)
    for (line <- Source.fromFile(graphFile).getLines) {
        val clusterIDs = line.split(" ").map{ID => ID.toInt}
        IDXtoMergeID(clusterIDs(0)) = clusterIDs(1)
    }
    assert(!IDXtoMergeID.contains(Unclaimed))
    print("DEBUG: coarsenWithInputFile_addExclude\n")
    // for(id <- IDXtoMergeID){
    //   print(id)
    //   print("\n")
    // }
    clustersToKeep = mg.applyInitialAssignments_returnMergeIDs(IDXtoMergeID)
    TopologicalSort(mg)
    printf(s"  #clusters created: ${mg.mergeIDToMembers.size - excludeSet.size}")
    printf(s"  largest partion: ${(mg.mergeIDToMembers.values.map{_.size}).max}")
    logger.info(s"  #clusters created: ${mg.mergeIDToMembers.size - excludeSet.size}")
    logger.info(s"  largest partion: ${(mg.mergeIDToMembers.values.map{_.size}).max}")
  }

  def mergeSingleInputPartsIntoParents(smallPartCutoff: Int = 20) {
    val smallPartIDs = findSmallParts(smallPartCutoff)

    // //-----071923 debug-----
    // var fMSS_merges = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallSiblings_merges_scala",true)
    // val pMSS_merges = new java.io.PrintWriter(fMSS_merges)
    // mg.outputMGMergeInfo_modfile(pMSS_merges)
    // var fMSS_neighs = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallSiblings_neighs_scala",true)
    // val pMSS_neighs = new java.io.PrintWriter(fMSS_neighs)
    // mg.outputGNeighbors_modfile(pMSS_neighs)    

    val singleInputIDs = smallPartIDs filter { id => (mg.inNeigh(id).size == 1) }
    val singleInputParents = (singleInputIDs flatMap mg.inNeigh).distinct
    val baseSingleInputIDs = singleInputIDs diff singleInputParents
    logger.info(s"  merging up ${baseSingleInputIDs.size} single-input parts")
    baseSingleInputIDs foreach { childID => {
      val parentID = mg.inNeigh(childID).head
      if (!excludeSet.contains(parentID) & !(clustersToKeep.contains(parentID)))
        mg.mergeGroups(parentID, Seq(childID))
    }}
    if (baseSingleInputIDs.size < singleInputIDs.size)
      mergeSingleInputPartsIntoParents(smallPartCutoff)
  }

  def mergeSmallSiblings(smallPartCutoff: Int = 10) {
    val smallPartIDs = findSmallParts(smallPartCutoff)

    //-----071923 debug-----
    var fMSS_merges = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallSiblings_merges_scala",true)
    val pMSS_merges = new java.io.PrintWriter(fMSS_merges)
    mg.outputMGMergeInfo_modfile(pMSS_merges)
    var fMSS_neighs = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallSiblings_neighs_scala",true)
    val pMSS_neighs = new java.io.PrintWriter(fMSS_neighs)
    mg.outputGNeighbors_modfile(pMSS_neighs)

    val inputsAndIDPairs = smallPartIDs map { id => {
      val inputsCanonicalized = mg.inNeigh(id).toSeq.sorted
      (inputsCanonicalized, id)
    }}
    val inputsToSiblings = Util.groupByFirst(inputsAndIDPairs.toSeq)
    // NOTE: since inputs *exactly* the same, don't need to do merge safety check
    val mergesToConsider = inputsToSiblings collect {
      case (inputIDs, siblingIDs) if (siblingIDs.size > 1) => siblingIDs
    }
    logger.info(s"  attempting to merge ${mergesToConsider.size} groups of small siblings")
    
    //-----072423 DEBUG FILE-----
    print("072423 DEBUG: mergesToConsider.toSeq.size: \n")
    print(mergesToConsider.toSeq.size)
    print("\n")
    var fMSS_merges2con = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/072423_mergesToConsider_scala",true)
    val pMSS_merges2con = new java.io.PrintWriter(fMSS_merges2con)    
    outputVecVecInts(pMSS_merges2con, mergesToConsider.toSeq)

    val mergesMade = perfomMergesIfPossible(mergesToConsider.toSeq)
    if (mergesMade.nonEmpty) {
      mergeSmallSiblings(smallPartCutoff)
    }
  }

  //-----072423 DEBUG ONLY FUNC-----
  def outputVecVecInts(p_mf: java.io.PrintWriter, valdict : Seq[Seq[Int]]){
    // var f = new FileWriter(graphDumpFile,true)
    // val p = new java.io.PrintWriter(f)
    p_mf.print("mergesToConsider\n")
    for(innerVec <- valdict){
      for(member <- innerVec){
        p_mf.print(member)
        p_mf.print("<2>")
      }
      p_mf.print("<1>")
    }
    p_mf.print("\n")
    p_mf.flush()
    p_mf.close()
  }

  def mergeSmallParts(smallPartCutoff: Int = 20, mergeThreshold: Double = 0.5) {
    val smallPartIDs = findSmallParts(smallPartCutoff)
    val mergesToConsider = smallPartIDs flatMap { id => {
      val numInputs = mg.inNeigh(id).size.toDouble
      val siblings = (mg.inNeigh(id) flatMap mg.outNeigh).distinct - id
      val legalSiblings = siblings filter { sibID => (!excludeSet.contains(sibID) && !clustersToKeep.contains(sibID)) }
      val orderConstrSibs = legalSiblings filter { _ < id }
      val myInputSet = mg.inNeigh(id).toSet
      val sibsScored = orderConstrSibs map {
        sibID => (mg.inNeigh(sibID).count(myInputSet) / numInputs, sibID)
      }
      val choices = sibsScored filter { _._1 >= mergeThreshold }
      val choicesOrdered = choices.sortWith{_._1 > _._1}
      val topChoice = choicesOrdered.find {
        case (score, sibID) => mg.mergeIsAcyclic(sibID, id)
      }
      if (topChoice.isEmpty) Seq()
      else Seq(Seq(topChoice.get._2, id))
    }}
    logger.info(s"  of ${smallPartIDs.size} small parts ${mergesToConsider.size} worth merging")
    val mergesMade = perfomMergesIfPossible(mergesToConsider.toSeq)
    if (mergesMade.nonEmpty) {
      mergeSmallParts(smallPartCutoff, mergeThreshold)
    }
  }

  def mergeSmallPartsDown(smallPartCutoff: Int = 20) {
    val smallPartIDs = findSmallParts(smallPartCutoff)
    val mergesToConsider = smallPartIDs flatMap { id => {
      val mergeableChildren = mg.outNeigh(id) filter {
        childID => mg.mergeIsAcyclic(id, childID) && (!excludeSet.contains(childID) && !clustersToKeep.contains(childID))
      }
      if (mergeableChildren.nonEmpty) {
        val orderedByEdgesRemoved = mergeableChildren.sortBy {
          childID => numEdgesRemovedByMerge(Seq(id, childID))
        }
        val topChoice = orderedByEdgesRemoved.last
        Seq(Seq(id, topChoice))
      } else Seq()
    }}
    logger.info(s"  of ${smallPartIDs.size} small parts ${mergesToConsider.size} worth merging down")
    val mergesMade = perfomMergesIfPossible(mergesToConsider.toSeq)
    if (mergesMade.nonEmpty) {
      mergeSmallPartsDown(smallPartCutoff)
    }
  }

  def partition(smallPartCutoff: Int = 20) {
    print("----------071123_2----------")
    for(e <- excludeSet){
      print(e)
      print(" ")
    }
    print("\n")
    mg.outputMGNeighbors()
    //sg.outputSGValidNodes()
    // var fMSS_merges = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallSiblings_merges_scala",true)
    // val pMSS_merges = new java.io.PrintWriter(fMSS_merges)
    // var fMSS_neighs = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallSiblings_neighs_scala",true)
    // val pMSS_neighs = new java.io.PrintWriter(fMSS_neighs)
    val toApply = Seq(
      ("mffc", {ap: AcyclicPart => ap.coarsenWithMFFCs()}),
      // ("debug1", {ap: AcyclicPart => ap.mg.outputMGMergeInfo_modfile(pMSS_merges)}),
      // ("debug2", {ap: AcyclicPart => ap.mg.outputGNeighbors_modfile(pMSS_neighs)}),
      ("single", {ap: AcyclicPart => ap.mergeSingleInputPartsIntoParents()}),
      ("siblings", {ap: AcyclicPart => ap.mergeSmallSiblings(smallPartCutoff)}),
      ("small", {ap: AcyclicPart => ap.mergeSmallParts(smallPartCutoff, 0.5)}),
      ("down", {ap: AcyclicPart => ap.mergeSmallPartsDown(smallPartCutoff)}),
      ("small2", {ap: AcyclicPart => ap.mergeSmallParts(2*smallPartCutoff, 0.25)}),
    )
    toApply foreach { case (label, func) => {
      val startTime = System.currentTimeMillis()
      func(this)
      val stopTime = System.currentTimeMillis()
      //mg.outputMGMergeInfo_modfile(label)
      logger.info(s"[$label] took: ${stopTime - startTime}")
      logger.info(s"Down to ${partsRemaining()} parts")
    }}
    //mg.outputMGNeighbors()
    //mg.outputMGMergeInfo()
    assert(checkPartioning())
  }

  // 042523 - start with big clusters from output file, then run normal clusting algo
  // def partition(smallPartCutoff: Int = 20) {
  //   val toApply = Seq(
  //     ("coarsenWithInputFile_addExclude", {ap: AcyclicPart => ap.coarsenWithInputFile_addExclude()}),
  //     ("mffc", {ap: AcyclicPart => ap.coarsenWithMFFCs_notInitial()}),
  //     ("single", {ap: AcyclicPart => ap.mergeSingleInputPartsIntoParents()}),
  //     ("siblings", {ap: AcyclicPart => ap.mergeSmallSiblings(smallPartCutoff)}),
  //     ("small", {ap: AcyclicPart => ap.mergeSmallParts(smallPartCutoff, 0.5)}),
  //     ("down", {ap: AcyclicPart => ap.mergeSmallPartsDown(smallPartCutoff)}),
  //     ("small2", {ap: AcyclicPart => ap.mergeSmallParts(2*smallPartCutoff, 0.25)}),
  //   )
  //   toApply foreach { case (label, func) => {
  //     val startTime = System.currentTimeMillis()
  //     print("\n@begin\n")
  //     func(this)
  //     print("@ step: "+label+" output\n")
  //     //mg.outputMGNeighbors()
  //     print("\n@end\n")
  //     TopologicalSort(mg)
  //     print("\n@mgcheckdone!\n")
  //     val stopTime = System.currentTimeMillis()
  //     logger.info(s"[$label] took: ${stopTime - startTime}")
  //     logger.info(s"Down to ${partsRemaining()} parts")
  //   }}
  //   assert(checkPartioning())
  // }

  def iterParts() = mg.iterGroups

  def checkPartioning() = {
    val includedSoFar = HashSet[NodeID]()
    val disjoint = mg.iterGroups forall { case (macroID, memberIDs) => {
      val overlap = includedSoFar.intersect(memberIDs.toSet).nonEmpty
      includedSoFar ++= memberIDs
      !overlap
    }}
    val complete = includedSoFar == mg.nodeRange.toSet
    disjoint && complete
  }
}


object AcyclicPart {
  def apply(g: Graph, excludeSet: Set[NodeID] = Set()) = {
    print("AcyclicPart made:\n")
    val ap = new AcyclicPart(MergeGraph(g), excludeSet)
    print("graphDumpFile?")
    //print(ap.mg.graphDumpFile)
    print("\n")
    print("graphDumpFile from g?")
    //print(g.graphDumpFile)
    print("\n")
    ap.mg.setGraphDumpFile(g.graphDumpFile)
    print("graphDumpFile after set frm g?")
    //print(ap.mg.graphDumpFile)
    print("\n")
    ap
  }
}

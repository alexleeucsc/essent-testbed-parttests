package essent

import essent.Graph.NodeID

import logger._

import collection.mutable.{ArrayBuffer, HashSet}

import scala.io.Source	
import MFFC.{Unclaimed,Excluded}	
import java.io._
import scala.sys.process._

class AcyclicPart(val mg: MergeGraph, excludeSet: Set[NodeID], useExtPart : Boolean) extends LazyLogging {

	//042523 - stores mergeIDs of big clusters we don't want to keep merging	
  var clustersToKeep = Set[NodeID] ()

  def partsRemaining() = (mg.mergeIDToMembers.keys.toSet - excludeSet).size

  // def findSmallParts(smallPartCutoff: Int) = mg.mergeIDToMembers.keys.toSeq filter {
  //   id => (mg.nodeSize(id) < smallPartCutoff) && (!excludeSet.contains(id)) && (!clustersToKeep.contains(id))
  // }
  def findSmallParts(smallPartCutoff: Int) : Seq[Int] = {
    val smallPartsSorted = mg.mergeIDToMembers.keys.toSeq filter {id => (mg.nodeSize(id) < smallPartCutoff) && (!excludeSet.contains(id)) && (!clustersToKeep.contains(id))}
    return smallPartsSorted.sorted
  }

  def perfomMergesIfPossible(mergesToConsider: Seq[Seq[NodeID]]) = {
    val mergesMade = mergesToConsider flatMap { mergeReq => {
      assert(mergeReq.size > 1)
      val partsStillExist = mergeReq.forall{ mg.mergeIDToMembers.contains(_) }
      if (partsStillExist && mg.mergeIsAcyclic(mergeReq.toSet)) {
        assert(mergeReq forall { id => !excludeSet.contains(id) })
        //DEBUG 080223
        //mg.mergeGroups(mergeReq.head, mergeReq.tail)
        val mergeReqSorted = mergeReq.sorted.reverse
        // print("m:\n")
        // for(m <- mergeReq){
        //   print(m)
        //   print(", ")}
        // print("\n")
        mg.mergeGroups(mergeReqSorted.head, mergeReqSorted.tail)

        Seq(mergeReq)
      } else Seq()
    }}
    mergesMade
  }

  def timeMergesOnly(mergesToConsider: Seq[Seq[NodeID]]) = {
    val mergesMade = mergesToConsider flatMap { mergeReq => {
      assert(mergeReq.size > 1)
      val partsStillExist = mergeReq.forall{ mg.mergeIDToMembers.contains(_) }
      if (partsStillExist && mg.mergeIsAcyclic(mergeReq.toSet)) {
        assert(mergeReq forall { id => !excludeSet.contains(id) })
        //DEBUG 080223
        //mg.mergeGroups(mergeReq.head, mergeReq.tail)
        val mergeReqSorted = mergeReq.sorted.reverse
        print("m:\n")
        for(m <- mergeReq){
          print(m)
          print(", ")}
        print("\n")
        mg.mergeGroups(mergeReqSorted.head, mergeReqSorted.tail)

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
    // print("mffc: \n")
    // for(i <- mffcResults){
    //   print(i)
    //   print(", ")
    // }
    mg.applyInitialAssignments(mffcResults)
    logger.info(s"  #mffcs found: ${mg.mergeIDToMembers.size - excludeSet.size}")
    logger.info(s"  largest mffc: ${(mg.mergeIDToMembers.values.map{_.size}).max}")
  }

  def mergeSingleInputPartsIntoParents(smallPartCutoff: Int = 20) {
    val smallPartIDs = findSmallParts(smallPartCutoff)

    // var fMSS_merges = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/080223_mergeSingleInputPartsIntoParents_merges_scala",true)	
    // val pMSS_merges = new java.io.PrintWriter(fMSS_merges)	
    // mg.outputMGMergeInfo_modfile(pMSS_merges)	
    // var fMSS_neighs = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/080223_mergeSingleInputPartsIntoParents_neighs_scala",true)	
    // val pMSS_neighs = new java.io.PrintWriter(fMSS_neighs)	
    // mg.outputGNeighbors_modfile(pMSS_neighs)   
    
    val singleInputIDs = smallPartIDs filter { id => (mg.inNeigh(id).size == 1) }
    val singleInputParents = (singleInputIDs flatMap mg.inNeigh).distinct
    val baseSingleInputIDs = singleInputIDs diff singleInputParents
    logger.info(s"  merging up ${baseSingleInputIDs.size} single-input parts")

    //debug 080523
    // print("baseSingleInputIDs\n")
    // for(c <- baseSingleInputIDs){
    //   print(c)
    //   print(", ")
    // }
    // print("\n")

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
	  
    // //-----071923 debug-----	
    // var fMSS_merges = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallSiblings_merges_scala",true)	
    // val pMSS_merges = new java.io.PrintWriter(fMSS_merges)	
    // mg.outputMGMergeInfo_modfile(pMSS_merges)	
    // var fMSS_neighs = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallSiblings_neighs_scala",true)	
    // val pMSS_neighs = new java.io.PrintWriter(fMSS_neighs)	
    // mg.outputGNeighbors_modfile(pMSS_neighs)

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
	  
    // //-----072423 DEBUG FILE-----	
    // print("072423 DEBUG: mergesToConsider.toSeq.size: \n")	
    // print(mergesToConsider.toSeq.size)	
    // print("\n")	
    // var fMSS_merges2con = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/072423_mergesToConsider_scala",true)	
    // val pMSS_merges2con = new java.io.PrintWriter(fMSS_merges2con)    	
    // outputVecVecInts(pMSS_merges2con, mergesToConsider.toSeq)
    val mergesToConsiderSorted = mergesToConsider.toSeq.sortWith{_(1) < _(1)}.sortWith{_(0) < _(0)}
    val mergesMade = perfomMergesIfPossible(mergesToConsiderSorted.toSeq)
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
    //-----071923 debug-----	
    // var fMSS_merges = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallParts_merges_scala",true)	
    // val pMSS_merges = new java.io.PrintWriter(fMSS_merges)	
    // mg.outputMGMergeInfo_modfile(pMSS_merges)	
    // var fMSS_neighs = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallParts_neighs_scala",true)	
    // val pMSS_neighs = new java.io.PrintWriter(fMSS_neighs)	
    // mg.outputGNeighbors_modfile(pMSS_neighs)
    
    // print("\nmergesToConsider_choices\n") // 072723 DEBUG
    // var startTime = System.currentTimeMillis()
    val mergesToConsider = smallPartIDs flatMap { id => {
      val numInputs = mg.inNeigh(id).size.toDouble
      val siblings = (mg.inNeigh(id) flatMap mg.outNeigh).distinct - id
      val legalSiblings = siblings filter { sibID => (!excludeSet.contains(sibID)  && !(clustersToKeep.contains(sibID)))}
      val orderConstrSibs = legalSiblings filter { _ < id }
      //val orderConstrSibs = legalSiblings
      val myInputSet = mg.inNeigh(id).toSet
      val sibsScored = orderConstrSibs map {
        sibID => (mg.inNeigh(sibID).count(myInputSet) / numInputs, sibID)
      }
      val choices = sibsScored filter { _._1 >= mergeThreshold }

      //DEBUG
      //val choicesOrdered = choices.sortWith{_._1 > _._1} //sorting first with _2 is to debug; this should make the scala iter more like the cpp one
      val choicesOrdered = choices.sortWith{_._2 > _._2}.sortWith{_._1 > _._1} //sorting first with _2 is to debug; this should make the scala iter more like the cpp one

      // for (c <- choicesOrdered){ // 072723 DEBUG
      //   print(c._2)
      //   print("<2>")
      // } // 072723 DEBUG
      // print("<1>")
      val topChoice = choicesOrdered.find {
        case (score, sibID) => mg.mergeIsAcyclic(sibID, id)
      }
      if (topChoice.isEmpty) Seq()
      else Seq(Seq(topChoice.get._2, id))
    }}
    // var stopTime = System.currentTimeMillis()
    // print("gather: ")
    // print(stopTime - startTime)
    // print("\n")
    //print("<1>") // 072723 DEBUG
    //print("\nmergeSmallParts_mergesToConsider\n")
    // for (mergeGroup <- mergesToConsider){
    //   for (m <- mergeGroup){
    //     print(m)
    //     print("<2>")
    //   }
    //   print("<1>")
    // }
    // print("\n")    
    logger.info(s"  of ${smallPartIDs.size} small parts ${mergesToConsider.size} worth merging")
    // startTime = System.currentTimeMillis()
    val mergesToConsiderSorted = mergesToConsider.sortWith{_(1) < _(1)}.sortWith{_(0) < _(0)}
    val mergesMade = perfomMergesIfPossible(mergesToConsiderSorted.toSeq)
    // stopTime = System.currentTimeMillis()
    // print("merge: ")
    // print(stopTime - startTime)
    // print("\n")
    if (mergesMade.nonEmpty) {
      mergeSmallParts(smallPartCutoff, mergeThreshold)
    }
  }

  def smallSiblingsTimeTest(smallPartCutoff: Int = 20, mergeThreshold: Double = 0.5) {
    val smallPartIDs = findSmallParts(smallPartCutoff)
    //-----071923 debug-----	
    // var fMSS_merges = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallParts_merges_scala",true)	
    // val pMSS_merges = new java.io.PrintWriter(fMSS_merges)	
    // mg.outputMGMergeInfo_modfile(pMSS_merges)	
    // var fMSS_neighs = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallParts_neighs_scala",true)	
    // val pMSS_neighs = new java.io.PrintWriter(fMSS_neighs)	
    // mg.outputGNeighbors_modfile(pMSS_neighs)
    
    // print("\nmergesToConsider_choices\n") // 072723 DEBUG
    var startTime = System.currentTimeMillis()
    val mergesToConsider = smallPartIDs flatMap { id => {
      val numInputs = mg.inNeigh(id).size.toDouble
      val siblings = (mg.inNeigh(id) flatMap mg.outNeigh).distinct - id
      val legalSiblings = siblings filter { sibID => (!excludeSet.contains(sibID)  && !(clustersToKeep.contains(sibID)))}
      val orderConstrSibs = legalSiblings filter { _ < id }
      //val orderConstrSibs = legalSiblings
      val myInputSet = mg.inNeigh(id).toSet
      val sibsScored = orderConstrSibs map {
        sibID => (mg.inNeigh(sibID).count(myInputSet) / numInputs, sibID)
      }
      val choices = sibsScored filter { _._1 >= mergeThreshold }

      //DEBUG
      //val choicesOrdered = choices.sortWith{_._1 > _._1} //sorting first with _2 is to debug; this should make the scala iter more like the cpp one
      val choicesOrdered = choices.sortWith{_._2 > _._2}.sortWith{_._1 > _._1} //sorting first with _2 is to debug; this should make the scala iter more like the cpp one

      // for (c <- choicesOrdered){ // 072723 DEBUG
      //   print(c._2)
      //   print("<2>")
      // } // 072723 DEBUG
      // print("<1>")
      val topChoice = choicesOrdered.find {
        case (score, sibID) => mg.mergeIsAcyclic(sibID, id)
      }
      if (topChoice.isEmpty) Seq()
      else Seq(Seq(topChoice.get._2, id))
    }}
    var stopTime = System.currentTimeMillis()
    print("gather: ")
    print(stopTime - startTime)
    print("\n")
    //print("<1>") // 072723 DEBUG
    //print("\nmergeSmallParts_mergesToConsider\n")
    // for (mergeGroup <- mergesToConsider){
    //   for (m <- mergeGroup){
    //     print(m)
    //     print("<2>")
    //   }
    //   print("<1>")
    // }
    // print("\n")    
    logger.info(s"  of ${smallPartIDs.size} small parts ${mergesToConsider.size} worth merging")
    startTime = System.currentTimeMillis()
    val mergesToConsiderSorted = mergesToConsider.sortWith{_(1) < _(1)}.sortWith{_(0) < _(0)}
    val mergesMade = timeMergesOnly(mergesToConsiderSorted.toSeq)
    stopTime = System.currentTimeMillis()
    print("merge: ")
    print(stopTime - startTime)
    print("\n")
    // if (mergesMade.nonEmpty) {
    //   mergeSmallParts(smallPartCutoff, mergeThreshold)
    // }
  }


  def mergeSmallPartsDown(smallPartCutoff: Int = 20) {
    val smallPartIDs = findSmallParts(smallPartCutoff)

    //---080723--- debug
    // var fMSS_merges = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/080623_mergeSmallPartsDown_merges_scala",true)	
    // val pMSS_merges = new java.io.PrintWriter(fMSS_merges)	
    // mg.outputMGMergeInfo_modfile(pMSS_merges)	
    // var fMSS_neighs = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/080623_mergeSmallPartsDown_neighs_scala",true)	
    // val pMSS_neighs = new java.io.PrintWriter(fMSS_neighs)	
    // mg.outputGNeighbors_modfile(pMSS_neighs)
    
    val mergesToConsider = smallPartIDs flatMap { id => {
      val mergeableChildren = mg.outNeigh(id) filter {
        childID => mg.mergeIsAcyclic(id, childID) && (!excludeSet.contains(childID) && !(clustersToKeep.contains(childID)))
      }
      if (mergeableChildren.nonEmpty) {
        //DEBUG 080723
        // val orderedByEdgesRemoved = mergeableChildren.sortBy {
        //   childID => numEdgesRemovedByMerge(Seq(id, childID))
        // }
        val orderedByEdgesRemoved = mergeableChildren.sorted.sortBy {
          childID => numEdgesRemovedByMerge(Seq(id, childID))
        }

        val topChoice = orderedByEdgesRemoved.last
        Seq(Seq(id, topChoice))
      } else Seq()
    }}
    logger.info(s"  of ${smallPartIDs.size} small parts ${mergesToConsider.size} worth merging down")
  
    // print("\nmergeSmallPartsDown_mergesToConsider\n")
    // for (mergeGroup <- mergesToConsider){
    //   for (m <- mergeGroup){
    //     print(m)
    //     print("<2>")
    //   }
    //   print("<1>")
    // }
    // print("\n")  

    val mergesMade = perfomMergesIfPossible(mergesToConsider.toSeq)
    if (mergesMade.nonEmpty) {
      mergeSmallPartsDown(smallPartCutoff)
    }
  }


  def timetest(){
    for( n <- 0 until mg.outNeigh.size){
      for(d <- 0 until mg.outNeigh.size){
      print("timetest run:\n ")
      var startTime = System.currentTimeMillis()
      mg.mergeIsAcyclic(Set[NodeID](n, d))
      var stopTime = System.currentTimeMillis()
      print(stopTime - startTime)
      print("\n")
      }
   }
  }

  def timetestSingle(){
    print("timetest run:\n ")
    var startTime = System.currentTimeMillis()
    mg.mergeIsAcyclic(Set[NodeID](90454, 90775))
    var stopTime = System.currentTimeMillis()
    print(stopTime - startTime)
    print("\n")
  }

  def partition(smallPartCutoff: Int = 20) {
    //mg.outputMGNeighbors()      //PROFILE SPEEDTEST ONLY 081723
    print("useExtPart: ", useExtPart)
    if(!useExtPart){
      val toApply = Seq(
        ("mffc", {ap: AcyclicPart => ap.coarsenWithMFFCs()}),
        ("single", {ap: AcyclicPart => ap.mergeSingleInputPartsIntoParents()}),
        ("siblings", {ap: AcyclicPart => ap.mergeSmallSiblings(smallPartCutoff)}),
        //("time test", {ap: AcyclicPart => ap.timetestSingle()}),
        ("small", {ap: AcyclicPart => ap.mergeSmallParts(smallPartCutoff, 0.5)}),
        ("down", {ap: AcyclicPart => ap.mergeSmallPartsDown(smallPartCutoff)}),
        ("small2", {ap: AcyclicPart => ap.mergeSmallParts(2*smallPartCutoff, 0.25)}),
      )
      toApply foreach { case (label, func) => {
        val startTime = System.currentTimeMillis()
        func(this)
        val stopTime = System.currentTimeMillis()
        logger.info(s"[$label] took: ${stopTime - startTime}")
        logger.info(s"Down to ${partsRemaining()} parts")
      }}
            var f = new FileWriter("092523_fastpart",true)
      var p = new java.io.PrintWriter(f)
      mg.outputGNeighbors_modfile(p)
      f = new FileWriter("092523_fastpart",true)
      p = new java.io.PrintWriter(f)
      mg.outputMGMergeInfo_modfile(p)
    }
    else{
      mg.outputMGNeighbors()
      val command = "externalPart_2_topotest/a.out 10 " + mg.graphDumpFile
      val process = Process(command).run()
      process.exitValue()
      print("external done\n")
      mg.readInGraph("")
      var f = new FileWriter("092523_fastpart",true)
      var p = new java.io.PrintWriter(f)
      mg.outputGNeighbors_modfile(p)
      f = new FileWriter("092523_fastpart",true)
      p = new java.io.PrintWriter(f)
      mg.outputMGMergeInfo_modfile(p)
    }
    assert(checkPartioning())
  }

  def iterParts() = mg.iterGroups

  def checkPartioning() = {
    val includedSoFar = HashSet[NodeID]()
    val disjoint = mg.iterGroups forall { case (macroID, memberIDs) => {
      val overlap = includedSoFar.intersect(memberIDs.toSet).nonEmpty
      includedSoFar ++= memberIDs
      !overlap
    }}
    val complete = includedSoFar == mg.nodeRange.toSet
    print(disjoint)
    print(complete)
    disjoint && complete
  }
}


object AcyclicPart {
  def apply(g: Graph, excludeSet: Set[NodeID] = Set(), useExtPart : Boolean = false) = {
    val ap = new AcyclicPart(MergeGraph(g), excludeSet, useExtPart)
    //ap.mg.setGraphDumpFile(g.graphDumpFile)
    ap
  }
}

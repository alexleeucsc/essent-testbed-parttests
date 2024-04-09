package essent

import Graph.NodeID

import collection.mutable.{ArrayBuffer, HashMap, HashSet, Stack}
import java.io._
import scala.io.Source	
// Fundamental assumptions:
//  * IDs match up with IDs in original Graph
//  * The mergeID is a member of the merged group and ID used
//  * MergeGraph will not add new nodes after built

class MergeGraph extends Graph {
  // node ID -> merge ID
  val idToMergeID = ArrayBuffer[NodeID]()

  // merge ID -> [member] node IDs (must include mergeID)
  val mergeIDToMembers = HashMap[NodeID, Seq[NodeID]]()

  // inherits outNeigh and inNeigh from Graph

  def outputMGNeighbors(){
    print("graphDumpFile: ")
    print(graphDumpFile)
    print("\n")
    var f = new FileWriter(graphDumpFile,true)
    val p = new java.io.PrintWriter(f)
    p.print("InNeigh\n")
    for(incomingNodes <- inNeigh){
      for(member <- incomingNodes){
        p.print(member)
        p.print("<2>")
      }
      p.print("<1>")
    }
    p.print("\n")
    p.print("OutNeigh\n")
    for(outgoingNodes <- outNeigh){
      for(member <- outgoingNodes){
        p.print(member)
        p.print("<2>")
      }
      p.print("<1>")
    }
    p.print("\n")
    p.flush()
  }
  def outputMGMergeInfo(){
    // print("\n----------f?----------\n")
    // print(graphDumpFile)
    // print("\n\n")
    // var f = new FileWriter(graphDumpFile,true)
    // val p = new java.io.PrintWriter(f)
    p.print("mergeIDToMembers\n")
    for((mergeID,members) <- mergeIDToMembers){
      p.print(mergeID)
      p.print("<2>")
      for(member <- members){
        p.print(member)
        p.print("<3>")
      }
      p.print("<1>")
    }
    p.print("\n")
    p.print("idToMergeID\n")
    for(mergeID <- idToMergeID){
      p.print(mergeID)
      p.print("<1>")
    }
    p.print("\n")
    p.flush()
  }
  def outputMGMergeInfo_modfile(p_mf: java.io.PrintWriter){
    p_mf.print("mergeIDToMembers\n")
    for((mergeID,members) <- mergeIDToMembers){
      p_mf.print(mergeID)
      p_mf.print("<2>")
      for(member <- members){
        p_mf.print(member)
        p_mf.print("<3>")
      }
      p_mf.print("<1>")
    }
    p_mf.print("\n")
    p_mf.print("idToMergeID\n")
    for(mergeID <- idToMergeID){
      p_mf.print(mergeID)
      p_mf.print("<1>")
    }
    p_mf.print("\n")
    p_mf.flush()
    p_mf.close()
  }
  def outputMGNeighbors_dagpMergeInfo(){
    print("graphDumpFile_dagpMergeInfo: ")
    print(graphDumpFile+"_dagpMergeInfo")
    print("\n")
    var f = new FileWriter(graphDumpFile+"_dagpMergeInfo",true)
    val p = new java.io.PrintWriter(f)
    p.print("InNeigh\n")
    for(incomingNodes <- inNeigh){
      for(member <- incomingNodes){
        p.print(member)
        p.print("<2>")
      }
      p.print("<1>")
    }
    p.print("\n")
    p.print("OutNeigh\n")
    for(outgoingNodes <- outNeigh){
      for(member <- outgoingNodes){
        p.print(member)
        p.print("<2>")
      }
      p.print("<1>")
    }
    p.print("\n")
    p.flush()
  }
  def outputMGMergeInfo_dagpMergeInfo(){
    // print("\n----------f?----------\n")
    // print(graphDumpFile)
    // print("\n\n")
    var f = new FileWriter(graphDumpFile+"_dagpMergeInfo",true)
    val p = new java.io.PrintWriter(f)
    p.print("mergeIDToMembers\n")
    for((mergeID,members) <- mergeIDToMembers){
      p.print(mergeID)
      p.print("<2>")
      for(member <- members){
        p.print(member)
        p.print("<3>")
      }
      p.print("<1>")
    }
    p.print("\n")
    p.print("idToMergeID\n")
    for(mergeID <- idToMergeID){
      p.print(mergeID)
      p.print("<1>")
    }
    p.print("\n")
    p.flush()
  }
  def readInGraph(graphFile : String){
    val lines=Source.fromFile("data/example.txt").getLines.toList
    // print("lines from ext file: ")
    // print(lines.size)
    // print("\n")
    for(l <- lines){
      // print("line proc: ")
      // print(l)
      // print("\n")
      if (l contains "mergeIDToMembers"){
        mergeIDToMembers.clear()
        val line1 = l.split(":")(1).split("<1>")
        line1.map{pair => mergeIDToMembers(pair.split("<2>")(0).toInt) = pair.split("<2>")(1).split("<3>").map(member => member.toInt)}
        // for(p <- mergeIDToMembers){
        //   print(p._1)
        //   print(":")
        //   print("[")
        //   p._2.map{
        //     l=>print(l)
        //     print(", ")
        //   }
        //   print("], ")
        // }
      }
      if (l contains "InNeigh"){
        val line1 = l.split(":")(1).split("<1>")
        val inNeighVals = line1.map{neighList => neighList.split("<2>").filter(member => member != "").map(member => member.toInt)}
        // print(inNeighVals.size)
        // print(" vs: ")
        // print(inNeigh.size)
        // print("\n")
        (0 until inNeighVals.size) map {idx => inNeigh(idx).clear}
        (0 until inNeighVals.size) map {idx => inNeigh(idx).appendAll(inNeighVals(idx))}
        print("readGraph inNeighVals: ")
        for(p <- inNeighVals){
          p.map{
            l=>print(l)
            print(", ")
          }
          print("\n")
        }
      }
      if (l contains "OutNeigh"){
        val line1 = l.split(":")(1).split("<1>")
        val outNeighVals= line1.map{neighList => neighList.split("<2>").filter(member => member != "").map(member => member.toInt)}
        (0 until outNeighVals.size) map {idx => outNeigh(idx).clear}
        (0 until outNeighVals.size) map {idx => outNeigh(idx).appendAll(outNeighVals(idx))}
        }
      if (l contains "idToMergeID"){
        val idToMergeIDVals = l.split(":")(1).split("<1>").map{member => member.toInt}
        (0 until idToMergeIDVals.size) map {idx => idToMergeID(idx) = idToMergeIDVals(idx)}
      }
    }
  }
  def readInGraphFromDagP(graphFile: String, excludedIDs : Set[Int], printStmt : Int, clusterTargetDagP : Int) {
    val lines = Source.fromFile(dotGraphDumpFile+".partsfile.part_"+clusterTargetDagP+".seed_1000.txt").getLines.toList
    print("readInGraphFromDagP\n")
    //set mergeIDs
    var node = 0
    val mergeReqsFromFile = Seq.fill(outNeigh.size)(collection.mutable.Buffer[Int]())
    for (l <- lines) {
      // print(l)
      // print("\n")
      mergeReqsFromFile(l.toInt) += node
      node = node + 1
    }
    //set inNeighs
    val startTime = System.currentTimeMillis()
    val inNeighCopy = inNeigh map {membSeq => membSeq map {memb => memb}}
    val outNeighCopy = outNeigh map {membSeq => membSeq map {memb => memb}}

    // for (node <- 0 until outNeigh.size) {
    //   //if(!(excludedIDs contains node) & !( forall {membId => !(excludedIDs contains membId)})) {
    //   val validNodesToMerge = mergeReqsFromFile(node) filter {id => !(excludedIDs contains id)}
    //     //if ((validNodesToMerge.size > 1) && (mergeIDToMembers contains node)) {
    //     if ((validNodesToMerge.size > 1)) {
    //       //mergeGroups(node, mergeReqsFromFile(node).filter { id => mergeIDToMembers contains id })
    //       mergeGroups(validNodesToMerge.head, validNodesToMerge.tail.toSeq)
    //     }
    //   //}
    // }

    for (nodesToMerge <- mergeReqsFromFile) {
      //if(!(excludedIDs contains node) & !( forall {membId => !(excludedIDs contains membId)})) {
      val validNodesToMerge = nodesToMerge filter {id => !(excludedIDs contains id)}
        //if ((validNodesToMerge.size > 1) && (mergeIDToMembers contains node)) {
        if ((validNodesToMerge.size > 1)) {
          //mergeGroups(node, mergeReqsFromFile(node).filter { id => mergeIDToMembers contains id })
          mergeGroups(validNodesToMerge.head, validNodesToMerge.tail.toSeq)
        }
      //}
    }

    val stopTime_2 = System.currentTimeMillis()
    //fix cluster containing 25

    //split all memb in group 25
    val membToSplit = printStmt
    val mergeIDToSplit = idToMergeID(membToSplit)
    for(memb <- mergeIDToMembers(mergeIDToSplit)){
      mergeIDToMembers(memb) = Seq(memb)
      idToMergeID(memb) = memb
      outNeigh(memb) = outNeighCopy(memb).flatMap{ id => if(mergeIDToMembers contains id){mergeIDToMembers(id)}else{ArrayBuffer(id)} }.distinct
      inNeigh(memb) = inNeighCopy(memb).flatMap{ id => if(mergeIDToMembers contains id){mergeIDToMembers(id)}else{ArrayBuffer(id)} }.distinct
    }
    val stopTime = System.currentTimeMillis()
    print("\n dagP reading in took: ")
    print(stopTime - startTime)
    print("\n dagP mergeGroups took: ")
    print(stopTime_2 - startTime)
    print("\nedgecut in: ")
    print(inNeigh.map{neighborSet => neighborSet.size}.sum)
    print("\nedgecut out: ")
    print(outNeigh.map{neighborSet => neighborSet.size}.sum)
    // val memb = 25

    // //remove printStmt clust from its cluster
    // val clustToBreak = idToMergeID(memb)
    // mergeIDToMembers(clustToBreak) = mergeIDToMembers(clustToBreak) diff ArrayBuffer(memb)

    // //put printStmt clust into its own cluster
    // mergeIDToMembers(memb) = Seq(memb)
    // idToMergeID(memb) = memb

    // //add inNeigh and outNeigh to printStmt clust
    // // outNeigh(memb) = outNeighCopy(memb).flatMap{ id => if(mergeIDToMembers contains id){mergeIDToMembers(id)}else{ArrayBuffer(id)} }.distinct
    // // assert(outNeigh(memb).size == 0)
    // //inNeigh(memb) = inNeighCopy(memb).flatMap{ id => if(mergeIDToMembers contains id){mergeIDToMembers(id)}else{ArrayBuffer(id)} }.distinct
    // inNeigh(memb) = inNeighCopy(memb).map{ id => idToMergeID(id) }.distinct.filter{ id => id != memb}
    // outNeigh(memb) = outNeighCopy(memb).map{ id => idToMergeID(id) }.distinct.filter{ id => id != memb}

    // //add outNeigh from ext clust, then assert that it has no 
    // // inNeigh(clustToBreak) = mergeIDToMembers(clustToBreak).flatMap{ currMemb => inNeighCopy(currMemb).flatMap{ id => if(mergeIDToMembers contains id){mergeIDToMembers(id)}else{ArrayBuffer[essent.Graph.NodeID](id)} }.distinct}.toBuffer
    // // outNeigh(clustToBreak) = mergeIDToMembers(clustToBreak).flatMap{ currMemb => if(outNeighCopy contains currMemb){outNeighCopy(currMemb).flatMap{ id => if(mergeIDToMembers contains id){mergeIDToMembers(id)}else{ArrayBuffer[essent.Graph.NodeID](id)}}}else{ArrayBuffer[essent.Graph.NodeID]()} .distinct}.toBuffer
    // var newIncomingNeigh = ArrayBuffer[Int]()
    // for(currMemb <- mergeIDToMembers(clustToBreak)){
    //   for(incomingNeigh <- inNeighCopy(currMemb)){
    //     //newIncomingNeigh.add(idToMergeID(incomingNeigh).toInt)
    //     if(!(idToMergeID(incomingNeigh).toInt == clustToBreak)){
    //       newIncomingNeigh+=idToMergeID(incomingNeigh).toInt
    //     }
    //   }
    // }
    // inNeigh(clustToBreak) = newIncomingNeigh.distinct
    // var newOutgoingNeigh = ArrayBuffer[Int]()
    // for(currMemb <- mergeIDToMembers(clustToBreak)){
    //   for(outGoingNeigh <- outNeighCopy(currMemb)){
    //     //newIncomingNeigh.add(idToMergeID(incomingNeigh).toInt)
    //     if(!(idToMergeID(outGoingNeigh).toInt == clustToBreak)){
    //       newOutgoingNeigh+=idToMergeID(outGoingNeigh).toInt
    //     }
    //   }
    // }
    // outNeigh(clustToBreak) = newOutgoingNeigh.distinct
    // def checkPartioning() = {
    //   val includedSoFar = HashSet[NodeID]()
    //   val disjoint = iterGroups forall { case (macroID, memberIDs) => {
    //     val overlap = includedSoFar.intersect(memberIDs.toSet).nonEmpty
    //     includedSoFar ++= memberIDs
    //     !overlap
    //   }}
    //   val complete = includedSoFar == nodeRange.toSet
    //   print(disjoint)
    //   print(complete)
    //   disjoint && complete
    // }
    // checkPartioning()
  }
  def buildFromGraph(g: Graph) {
    // FUTURE: cleaner way to do this with clone on superclass?
    outNeigh.appendAll(ArrayBuffer.fill(g.numNodes)(ArrayBuffer[NodeID]()))
    inNeigh.appendAll(ArrayBuffer.fill(g.numNodes)(ArrayBuffer[NodeID]()))
    g.nodeRange foreach { id => {
      g.outNeigh(id).copyToBuffer(outNeigh(id))
      g.inNeigh(id).copyToBuffer(inNeigh(id))
    }}
    ArrayBuffer.range(0, numNodes()).copyToBuffer(idToMergeID)
    nodeRange() foreach { id  => mergeIDToMembers(id) = Seq(id) }
  }

  def applyInitialAssignments(initialAssignments: ArrayBuffer[NodeID]) {
    // FUTURE: support negative (unassigned) initial assignments
    idToMergeID.clear()
    mergeIDToMembers.clear()
    initialAssignments.copyToBuffer(idToMergeID)
    val asMap = Util.groupIndicesByValue(initialAssignments)
    asMap foreach { case (mergeID, members) => {
      assert(members.contains(mergeID))
      mergeIDToMembers(mergeID) = members
      mergeNodesMutably(mergeID, members diff Seq(mergeID))
    }}
  }

  def applyInitialAssignments_returnMergeIDs(initialAssignments: ArrayBuffer[NodeID]) : Set[NodeID] = {
    // FUTURE: support negative (unassigned) initial assignments
    idToMergeID.clear()
    mergeIDToMembers.clear()
    initialAssignments.copyToBuffer(idToMergeID)
    val asMap = Util.groupIndicesByValue(initialAssignments)
    print("size of problem: "+asMap.size.toString)
    // var asMapIdx = 0
    asMap foreach { case (mergeID, members) => {
      assert(members.contains(mergeID))
      mergeIDToMembers(mergeID) = members
      mergeNodesMutably(mergeID, members diff Seq(mergeID))
      // asMapIdx = asMapIdx + 1
      // if (asMapIdx%100==0){
      //   print("asMapIdx: "+asMapIdx.toString())
      // }
    }}
    return asMap.keySet.filter(a=>asMap(a).size>7)
  }

  def mergeGroups(mergeDest: NodeID, mergeSources: Seq[NodeID]) {
    val newMembers = (mergeSources map mergeIDToMembers).flatten
    newMembers foreach { id => idToMergeID(id) = mergeDest}
    mergeIDToMembers(mergeDest) ++= newMembers
    mergeSources foreach { id => mergeIDToMembers.remove(id) }
    mergeNodesMutably(mergeDest, mergeSources)
  }

  def nodeSize(n: NodeID) = mergeIDToMembers.getOrElse(n,Seq()).size

  def iterGroups() = mergeIDToMembers
}


object MergeGraph {
  def apply(g: Graph): MergeGraph = {
    val mg = new MergeGraph
    mg.buildFromGraph(g)
    mg.setGraphDumpFile(g.graphDumpFile)
    mg.setDotGraphDumpFile(g.dotGraphDumpFile)
    print("\n----------dotgraphdumpfile mergeGraph make-----\n")
    print(mg.dotGraphDumpFile)
    print("\n\n")
    mg
  }

  def apply(g: Graph, initialAssignments: ArrayBuffer[NodeID]): MergeGraph = {
    // TODO: remove unecessary building if using initialAssignments
    val mg = apply(g)
    print("\n----------ng.dotgraphdumpfile?-----\n")
    print(mg.dotGraphDumpFile)
    print("\n\n")
    mg.setGraphDumpFile(g.graphDumpFile)
    mg.setDotGraphDumpFile(g.dotGraphDumpFile)
    print("\n----------ng.dotgraphdumpfile?-----\n")
    print(mg.dotGraphDumpFile)
    print("\n\n")
    mg.applyInitialAssignments(initialAssignments)
    mg
  }
}

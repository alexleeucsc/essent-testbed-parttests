package essent

import Graph.NodeID

import collection.mutable.{ArrayBuffer, HashMap, HashSet}
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
  def outputMGNeighborsEval(){
    print("graphDumpFile eval: ")
    print(graphDumpFile)
    print("\n")
    var f = new FileWriter(graphDumpFile+"_preMerge",true)
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

  def outputMGNeighbors_internalMergeInfo(){
    print("graphDumpFile_internalMergeInfo: ")
    print(graphDumpFile+"_internalMergeInfo")
    print("\n")
    var f = new FileWriter(graphDumpFile+"_internalMergeInfo",true)
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
  def outputMGMergeInfo_internalMergeInfo(){
    // print("\n----------f?----------\n")
    // print(graphDumpFile)
    // print("\n\n")
    var f = new FileWriter(graphDumpFile+"_internalMergeInfo",true)
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
  def readInGraph(graphFile : String){
    val startTime = System.currentTimeMillis()
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
        // print("readGraph inNeighVals: ")
        // for(p <- inNeighVals){
        //   p.map{
        //     l=>print(l)
        //     print(", ")
        //   }
        //   print("\n")
        // }
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
    val stopTime = System.currentTimeMillis()
    print("\n external reading in took: ")
    print(stopTime - startTime)    
  }

  def readInGraph_fromArg(graphFile : String){
    val startTime = System.currentTimeMillis()
    val lines=Source.fromFile(graphFile).getLines.toList
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
        // print("readGraph inNeighVals: ")
        // for(p <- inNeighVals){
        //   p.map{
        //     l=>print(l)
        //     print(", ")
        //   }
        //   print("\n")
        // }
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
    val stopTime = System.currentTimeMillis()
    print("\n external reading in took: ")
    print(stopTime - startTime)    
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
    mg
  }

  def apply(g: Graph, initialAssignments: ArrayBuffer[NodeID]): MergeGraph = {
    // TODO: remove unecessary building if using initialAssignments
    val mg = apply(g)
    print("\n----------ng.graphdumpfile?-----\n")
    print(mg.graphDumpFile)
    print("\n\n")
    mg.setGraphDumpFile(g.graphDumpFile)
    print("\n----------ng.graphdumpfile?-----\n")
    print(mg.graphDumpFile)
    print("\n\n")
    mg.applyInitialAssignments(initialAssignments)
    mg
  }
}

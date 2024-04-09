package essent

import collection.mutable.ArrayBuffer

//debugging
import java.io.FileWriter;   // Import the FileWriter class
import java.io.IOException;  // Import the IOException class to handle errors

// Directed graph class to be used as base for others
//  - uses numeric vertex identifiers (NodeID  which is type alias for Int)
//  - tracks edges both in outgoing and incomming directions

class Graph {
  // Access companion object's type aliases without prefix
  import Graph.{NodeID, AdjacencyList}
  
  // Internal data structures
  //----------------------------------------------------------------------------
  // numeric vertex ID -> list of incoming vertex IDs (dependencies)
  val inNeigh: AdjacencyList = ArrayBuffer[ArrayBuffer[NodeID]]()
  // numeric vertex ID -> list outgoing vertex IDs (consumers)
  val outNeigh: AdjacencyList = ArrayBuffer[ArrayBuffer[NodeID]]()

  //path to dump files:
  //----------------------------------------------------------------------------
  var graphDumpFile = "070723_graphdump_nofileset"
  var dotGraphDumpFile = "100523_dotgraphdump_nofileset"
  def setGraphDumpFile(graphDumpFileIn: String) = {
    if(graphDumpFileIn == "" || graphDumpFileIn.contains("data/")){
      graphDumpFile = graphDumpFileIn
    }
    else{
      graphDumpFile = "data/" + graphDumpFileIn
    }
      f = new FileWriter(graphDumpFile,true)
      p = new java.io.PrintWriter(f)
  }
  def setDotGraphDumpFile(dotGraphDumpFileIn: String) = {
    if (dotGraphDumpFileIn == "" || dotGraphDumpFileIn.contains("data/")) {
      dotGraphDumpFile = dotGraphDumpFileIn
    }
    else {
      dotGraphDumpFile = "data/" + dotGraphDumpFileIn
    }
    fd = new FileWriter(dotGraphDumpFile, true)
    pd = new java.io.PrintWriter(fd)
  }

  var f = new FileWriter(graphDumpFile,true)
  var p = new java.io.PrintWriter(f)
  var fd = new FileWriter(dotGraphDumpFile, true)
  var pd = new java.io.PrintWriter(fd)
  
  // Graph building
  //----------------------------------------------------------------------------
  def growNeighsIfNeeded(id: NodeID) {
    assert(id >= 0)
    if (id >= outNeigh.size) {
      val numElemsToGrow = id - outNeigh.size + 1
      outNeigh.appendAll(ArrayBuffer.fill(numElemsToGrow)(ArrayBuffer[NodeID]()))
      inNeigh.appendAll(ArrayBuffer.fill(numElemsToGrow)(ArrayBuffer[NodeID]()))
    }
  }

  def addEdge(sourceID: NodeID, destID: NodeID) {
    growNeighsIfNeeded(math.max(sourceID, destID))
    outNeigh(sourceID) += destID
    inNeigh(destID) += sourceID
  }

  def addEdgeIfNew(sourceID: NodeID, destID: NodeID) {
    if ((sourceID >= outNeigh.size) || !outNeigh(sourceID).contains(destID))
      addEdge(sourceID, destID)
  }


  // Accessors
  //----------------------------------------------------------------------------
  def nodeRange() = 0 until outNeigh.size


  // Traversals / Queries
  //----------------------------------------------------------------------------
  // TODO: make NodeSet type?
  // TODO: speed advantages of using BitSet in places?
  // TODO: speed advantages of using less Set-like structures?
  def extPathExists(source: NodeID, dest: NodeID): Boolean = extPathExists(Set(source), Set(dest))

  def extPathExists(sourceSet: Set[NodeID], destSet: Set[NodeID]): Boolean = {
    val sourcesOnFringe = sourceSet filter {
      id => outNeigh(id) exists { neigh => !sourceSet.contains(neigh) }
    }
    val startingExtFrontier = sourcesOnFringe flatMap outNeigh diff destSet
    def traverseUntilIntersect(frontier: Set[NodeID], reached: Set[NodeID]): Boolean = {
      if (frontier.isEmpty) false
      else {
        // print("\tfrontier:\n")
        // for(f <- frontier){
        //   print("\t")
        //   print(f)
        // }
        // print("\n")
        val nextFrontier = frontier flatMap outNeigh diff reached
        if ((nextFrontier & destSet).nonEmpty) true
        else traverseUntilIntersect(nextFrontier, reached ++ nextFrontier)
      }
    }
    traverseUntilIntersect(startingExtFrontier, sourceSet ++ startingExtFrontier)
  }

  def mergeIsAcyclic(u: NodeID, v: NodeID): Boolean = !extPathExists(u,v) && !extPathExists(v,u)

  // TODO: speed up by doing a single traversal
  def mergeIsAcyclic(ids: Set[NodeID]): Boolean = {
    // var startTime = System.currentTimeMillis()
    val isCyclic = ids forall { source => !extPathExists(Set(source), ids - source) }
    // var stopTime = System.currentTimeMillis()
    // print("\t\tMIA ")
    // print(ids.size)
    // print(": ")
    // print(stopTime - startTime)
    // print("\n")
    return isCyclic
  }


  // Mutators
  //----------------------------------------------------------------------------
  def removeDuplicateEdges() {
    // will not remove self-loops
    def uniquifyNeighs(neighs: AdjacencyList) {
      (0 until neighs.size) foreach { id => neighs(id) = neighs(id).distinct }
    }
    uniquifyNeighs(outNeigh)
    uniquifyNeighs(inNeigh)
  }

  def mergeNodesMutably(mergeDest: NodeID, mergeSources: Seq[NodeID]) {
    val mergedID = mergeDest
    val idsToRemove = mergeSources
    val idsToMerge = mergeSources :+ mergeDest
    val combinedInNeigh = idsToMerge.flatMap(inNeigh).distinct diff idsToMerge
    val combinedOutNeigh = idsToMerge.flatMap(outNeigh).distinct diff idsToMerge
    // TODO: reduce redundancy with AddEdgeIfNew
    combinedInNeigh foreach { inNeighID => {
      outNeigh(inNeighID) --= idsToRemove
      if (!outNeigh(inNeighID).contains(mergedID)) outNeigh(inNeighID) += mergedID
    }}
    combinedOutNeigh foreach { outNeighID => {
      inNeigh(outNeighID) --= idsToRemove
      if (!inNeigh(outNeighID).contains(mergedID)) inNeigh(outNeighID) += mergedID
    }}
    inNeigh(mergedID) = combinedInNeigh.to[ArrayBuffer]
    outNeigh(mergedID) = combinedOutNeigh.to[ArrayBuffer]
    idsToRemove foreach { deleteID => {
      inNeigh(deleteID).clear()
      outNeigh(deleteID).clear()
    }}
  }


  // Stats
  //----------------------------------------------------------------------------
  // assumes outNeigh and inNeigh grow together (they should)
  def numNodes() = outNeigh.size

  def computeDegrees(neighs: AdjacencyList) = {
    neighs map { _.size }
  }

  def numEdges() = computeDegrees(outNeigh).sum
  def outputGNeighbors_modfile(p_mf: java.io.PrintWriter){
    // var f = new FileWriter(graphDumpFile,true)
    // val p = new java.io.PrintWriter(f)
    p_mf.print("InNeigh\n")
    for(incomingNodes <- inNeigh){
      for(member <- incomingNodes){
        p_mf.print(member)
        p_mf.print("<2>")
      }
      p_mf.print("<1>")
    }
    p_mf.print("\n")
    p_mf.print("OutNeigh\n")
    for(outgoingNodes <- outNeigh){
      for(member <- outgoingNodes){
        p_mf.print(member)
        p_mf.print("<2>")
      }
      p_mf.print("<1>")
    }
    p_mf.print("\n")
    p_mf.flush()
    p_mf.close()
  }
  def outputGNeighbors_modfile_toDot(p_mf: java.io.PrintWriter){
    // var f = new FileWriter(graphDumpFile,true)
    // val p = new java.io.PrintWriter(f)
    p_mf.print("digraph G {\n")
    for(nodeName <- 0 until outNeigh.size){
      p_mf.print(nodeName)
      p_mf.print(";\n")
    }
    for(nodeName <- 0 until outNeigh.size){
      for(dest <- outNeigh(nodeName)){
        p_mf.print(nodeName)
        p_mf.print("->")
        p_mf.print(dest)
        p_mf.print(" ;\n")
      }
    }
    p_mf.print("}\n")
    p_mf.flush()
    p_mf.close()
  }
}

object Graph {
  type NodeID = Int
  type AdjacencyList = ArrayBuffer[ArrayBuffer[NodeID]]
}

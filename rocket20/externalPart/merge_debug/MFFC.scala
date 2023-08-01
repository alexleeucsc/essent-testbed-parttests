package MFFC

import Graph._

import collection.mutable.ArrayBuffer

//debugging
import java.io.FileWriter;   // Import the FileWriter class
import java.io.IOException;  // Import the IOException class to handle errors

class MFFC(val g: Graph) {
  import MFFC.{Unclaimed,Excluded}
  import Graph.{NodeID, AdjacencyList}

  // numeric vertex ID -> MFFC ID
  val mffc = ArrayBuffer.fill(g.numNodes)(Unclaimed)

  def overrideMFFCs(newAssignments: ArrayBuffer[NodeID]) {
    mffc.clear()
    newAssignments.copyToBuffer(mffc)
  }

  def findMFFCs(): ArrayBuffer[NodeID] = {
    val unvisitedSinks = g.nodeRange filter {
      id => mffc(id) == Unclaimed && g.outNeigh(id).isEmpty
    }
    val visited = g.nodeRange filter { id => mffc(id) != Unclaimed }
    val fringe = (visited flatMap(g.inNeigh)).distinct
    val unvisitedFringe = fringe filter { mffc(_) == Unclaimed }
    val newMFFCseeds = unvisitedSinks.toSet ++ unvisitedFringe
    //debug 070623
    FileWriter myWriter = new FileWriter("~/alexlee/essent-testbed-document/essent-testbed-parttests/essent/mffcdebugdump.txt")
    myWriter.write("----------ITER----------\n")
    myWriter.print("unvisitedSinks\n")
    for (n <- unvisitedSinks){
      myWriter.print(n)
      myWriter.print(" ")
    }
    myWriter.print("\n")
    myWriter.write("visited\n")
    for (n <- visited){
      myWriter.write(n)
      myWriter.write(" ")
    }
    myWriter.write("\n")
    myWriter.write("fringe\n")
    for (n <- fringe){
      myWriter.write(n)
      myWriter.write(" ")
    }
    myWriter.write("\n")
    myWriter.write("unvisitedFringe\n")
    for (n <- unvisitedFringe){
      myWriter.write(n)
      myWriter.write(" ")
    }
    myWriter.write("\n")
    myWriter.write("newMFFCseeds\n")
    for (n <- newMFFCseeds){
      myWriter.write(n)
      myWriter.write(" ")
    }
    myWriter.write("\n\n\n")
    myWriter.close()
    //debug 070623
    if (newMFFCseeds.isEmpty) {
      mffc
    } else {
      newMFFCseeds foreach { id => mffc(id) = id }
      maximizeFFCs(newMFFCseeds)
      findMFFCs()
    }
  }

  def maximizeFFCs(fringe: Set[NodeID]) {
    val fringeAncestors = fringe flatMap g.inNeigh filter { mffc(_) == Unclaimed }
    val newMembers = fringeAncestors flatMap { parent => {
      val childrenMFFCs = (g.outNeigh(parent) map mffc).distinct
      print(childrenMFFCs)
      if ((childrenMFFCs.size == 1) && (childrenMFFCs.head != Unclaimed)) {
        print(parent, " set to ", childrenMFFCs.head,"\n")
        mffc(parent) = childrenMFFCs.head
        Seq(parent)
      } else Seq()
    }}
    if (newMembers.nonEmpty)
      maximizeFFCs(newMembers)
  }
}


object MFFC {
  import Graph.{NodeID, AdjacencyList}
  val Unclaimed = -1
  val Excluded  = -2

  def apply(g: Graph, excludeSet: Set[NodeID] = Set()): ArrayBuffer[NodeID] = {
    val worker = new MFFC(g)
    excludeSet foreach { id => worker.mffc(id) = Excluded }
    val mffc = worker.findMFFCs()
    excludeSet foreach { id => mffc(id) = id }
    mffc
  }
}

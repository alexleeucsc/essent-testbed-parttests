package essent

import Graph._

import collection.mutable.ArrayBuffer

//debugging
import java.io.FileWriter;   // Import the FileWriter class
import java.io.IOException;  // Import the IOException class to handle errors

class MFFC(val g: Graph) {
  import MFFC.{Unclaimed,Excluded}

  // numeric vertex ID -> MFFC ID
  val mffc = ArrayBuffer.fill(g.numNodes)(Unclaimed)
    // var myWriterf = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/mffcdebugdump",true)
    // val myWriter = new java.io.PrintWriter(myWriterf)
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
    // print("----------071023----------")
    // for(v <- visited){
    //   print(v)
    //   print("\t")
    //   print("v_vs_neigh: ")
    //   for(node <- g.inNeigh(v)){
    //     print(node)
    //     print("\t")
    //   }
    //   print("\n")
    // }
    val unvisitedFringe = fringe filter { mffc(_) == Unclaimed }
    val newMFFCseeds = unvisitedSinks.toSet ++ unvisitedFringe
    //debug 070623
    // var fdebug1 = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/mffcdebugdump.txt", true)
    // var printdebug1 = new java.io.PrintWriter(fdebug1)
    // //g.outputGNeighbors_tmp710()
    // printdebug1.print("----------ITER----------\n")
    // printdebug1.print("unvisitedSinks\n")
    // for (n <- unvisitedSinks){
    //   printdebug1.print(n)
    //   printdebug1.print(" ")
    // }
    // printdebug1.print("\n")
    // printdebug1.print("visited\n")
    // for (n <- visited){
    //   printdebug1.print(n)
    //   printdebug1.print(" ")
    // }
    // printdebug1.print("\n")
    // printdebug1.print("fringe\n")
    // for (n <- fringe){
    //   printdebug1.print(n)
    //   printdebug1.print(" ")
    // }
    // printdebug1.print("\n")
    // printdebug1.print("unvisitedFringe\n")
    // for (n <- unvisitedFringe){
    //   printdebug1.print(n)
    //   printdebug1.print(" ")
    // }
    // printdebug1.print("\n")
    // printdebug1.print("newMFFCseeds\n")
    // for (n <- newMFFCseeds){
    //   printdebug1.print(n)
    //   printdebug1.print(" ")
    // }
    // printdebug1.print("\n\n\n")
    // printdebug1.close()
    //debug 070623
    //System.exit(0)
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
      if ((childrenMFFCs.size == 1) && (childrenMFFCs.head != Unclaimed)) {
        mffc(parent) = childrenMFFCs.head
        Seq(parent)
      } else Seq()
    }}
    if (newMembers.nonEmpty)
      maximizeFFCs(newMembers)
  }
}


object MFFC {
  val Unclaimed = -1
  val Excluded  = -2

  def apply(g: Graph, excludeSet: Set[NodeID] = Set()): ArrayBuffer[NodeID] = {
    val worker = new MFFC(g)
    // g.outputGNeighbors_tmp710()
    // print("----------071123_1----------")
    // for(e <- excludeSet){
    //   print(e)
    //   print(" ")
    // }
    // print("\n")
    excludeSet foreach { id => worker.mffc(id) = Excluded }
    val mffc = worker.findMFFCs()
    print("----------071323_mffc----------")
    for(m <- mffc){
      print(m)
      print(" ")
    }
    print("\n")
    var fdebug1 = new FileWriter("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/mffcdebugdump.txt", true)
    var printdebug1 = new java.io.PrintWriter(fdebug1)
    printdebug1.print("mffcs\n")
    for (n <- mffc){
      printdebug1.print(n)
      printdebug1.print(" ")
    }
    printdebug1.print("\n")
    printdebug1.flush()
    printdebug1.close()
    excludeSet foreach { id => mffc(id) = id }
    mffc
  }
}

package essent.passes

import firrtl._
import firrtl.ir._
import firrtl.options.Dependency
import firrtl.passes._

import java.io.{File, FileWriter}
import scala.collection.mutable.HashMap

/*
  This pass was used to convert .fir files generated by old versions of firrtl before moving to firrtl v1.4.
  The legacy firrtl files refer to DefNodes outside the (Conditionally) scope they were declared in.
  The fix this pass does is to declare the DefNode in question as a Wire and mark it Invalid at the higher scope.

  This pass should be forced into early into the pass lineup and used to output the new .fir immediately, and then
  disabled.
 */


object LegacyInvalidNodesForConds extends Pass {
  def desc = "Finds Nodes declared within Conditionals but used outside and makes them into Invalid Wires"

  override def prerequisites = firrtl.stage.Forms.ChirrtlForm
  override def optionalPrerequisites = Seq.empty
  override def optionalPrerequisiteOf = Seq(Dependency(firrtl.passes.CheckChirrtl), Dependency(firrtl.passes.CheckHighForm))
  override def invalidates(a: Transform) = false

  var totalLeaks = 0
  def findNodesDeclaredInStmt(s: Statement): Map[String, Type] = {
    val namesFound = HashMap[String, Type]()
    def findNodeNamesHelper(stmt: Statement): Unit = {
      stmt match {
        case d: DefNode => namesFound(d.name) = d.value.tpe
        case _ => stmt foreachStmt findNodeNamesHelper
      }
    }
    findNodeNamesHelper(s)
    namesFound.toMap
  }

  // NOTE: assumes leaked names follow Conditionally Block and are not too far apart
  def scanBlock(b: Block): Map[String, Type] = {
    var nodesFromLastCond = Map[String, Type]()
    val leakedNodes = HashMap[String, Type]()
    def scanExpr(e: Expression): Unit = {
      e match {
        case Reference(name, tpe, _, _) if (nodesFromLastCond.contains(name)) =>  // COMMENT out for firrtl 1.3.1
//        case Reference(name, tpe) if (nodesFromLastCond.contains(name)) =>      // UNCOMMENT for firrtl 1.3.1
          leakedNodes(name) = nodesFromLastCond(name)
        case _ => e foreachExpr scanExpr
      }
    }
    b.stmts foreach { _ match {
      case c: Conditionally => nodesFromLastCond = findNodesDeclaredInStmt(c)
      case s => s foreachExpr scanExpr
    }}
    leakedNodes.toMap
  }

  def generateInvalidWires(nodesToInvalidate: Map[String, Type]): Seq[Statement] = nodesToInvalidate.keys.toSeq flatMap {
    name => Seq[Statement](DefWire(NoInfo, name, nodesToInvalidate(name)), IsInvalid(NoInfo, Reference(name, nodesToInvalidate(name))))
  }

  def convStmts(nodesToConvert: Map[String, Type])(s: Statement): Statement = {
    s match {
      case DefNode(info, name, value) if (nodesToConvert.contains(name)) =>
//        println(s"for $name found ${value.tpe} and ${nodesToConvert(name)}")
        Connect(info, Reference(name, nodesToConvert(name)), value)
      case _ => s mapStmt convStmts(nodesToConvert)
    }
  }

  def processMod(m: Module): Module = m.body match {
      case b: Block => {
        val leakedNodesFound = scanBlock(b)
        if (leakedNodesFound.nonEmpty) {
//          println(s"${m.name} - $leakedNodesFound.keys")
          totalLeaks += leakedNodesFound.size
          val newBody = Block(generateInvalidWires(leakedNodesFound) ++ b.stmts map convStmts(leakedNodesFound))
          m.copy(body = newBody)
        } else m
      }
    }

  def run(c: Circuit): Circuit = {
    val typed = firrtl.passes.CInferTypes.run(c)
    val modulesx = typed.modules.map {
      case m: Module => processMod(m)
      case m: ExtModule => m
    }
    println(s"Found a total of ${totalLeaks} leaked names")
    val result = Circuit(c.info, modulesx, c.main)
    val convWriter = new FileWriter(new File(s"${c.main}.conv.fir"))
    convWriter.write(result.serialize)
    convWriter.close()
    result
  }
}

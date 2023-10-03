package essent

import essent.Graph.NodeID

import logger._

import collection.mutable.{ArrayBuffer, HashSet}

import scala.io.Source	
import MFFC.{Unclaimed,Excluded}	
import java.io._

abstract class cplus(val mg: MergeGraph, excludeSet: Set[NodeID]) extends LazyLogging {
    def fill_buffer(arr1 : Array[Int], arr1Len : Int) : Int
}


object cplus {
  def apply(g: Graph, excludeSet: Set[NodeID] = Set()) = {
  }
}

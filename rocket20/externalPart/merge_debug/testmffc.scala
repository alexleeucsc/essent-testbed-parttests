import MFFC._
import Graph._
import collection.mutable.ArrayBuffer
//https://stackoverflow.com/questions/3075951/scala-importing-class

object testmffc {
    def main(args: Array[String]) = {
        println("Hello, world")
        val g = new Graph()
        // g.addEdge(0,1)
        // g.addEdge(1,2)
        // g.addEdge(3,4)
        // g.addEdge(4,5)
        // g.addEdge(4,2)
        // g.addEdge(6,7)
        //
        g.addEdge(1,0);
        g.addEdge(2,0);
        g.addEdge(3,0);
        g.addEdge(4,1);
        g.addEdge(4,2);
        g.addEdge(5,4);
        g.addEdge(6,3);
        g.addEdge(6,7);
        g.addEdge(7,8);
        g.addEdge(7,9);
        val mffcWorker = new MFFC(g)
        val initialMFFC =  ArrayBuffer(-1,-1, -1, -1, -1, -1,-1,-1, -1, -1)
        val expectedMFFC = ArrayBuffer( 2, 2, 2, 2, 2, 2,-1,-1)
        mffcWorker.overrideMFFCs(initialMFFC)
        mffcWorker.findMFFCs()
        print( mffcWorker.mffc )
    }
}